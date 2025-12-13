/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/12/11 23:30:03 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "file.h"
#include "print_utils.h"
#include "elf_parser.h"
#include "stub.h"

#define PAGE_SIZE 0x1000
#define DEBUG 1
#define ORIGINAL_ENTRY_POINT 0x1122334455667788
#define OUTPUT_FILENAME "woody"

uint64_t align_up(uint64_t val, uint64_t align)
{
    return (val + align - 1) & ~(align - 1);
}

int patch_placeholder(uint64_t *target, unsigned int target_len, Elf64_Addr original_entry, uint64_t placeholder)
{
    int patched = FALSE;

    for (unsigned int i = 0; i < target_len - 8; i++)
    {
        uint64_t *ptr = target + i;
        
        if (*ptr == placeholder)
        {
            *ptr = (uint64_t)original_entry;
            patched = TRUE;
            print_debug("    [+] Stub patched at offset %d with OEP 0x%lx\n", i, original_entry);
            break;
        }
    }
    if (!patched)
        print_error(PLACEHOLDER_FIND_FAILED, ERRNO_FALSE);

    return patched;
}

int main(int argc, char *argv[])
{
    int exit_code = 0;

    int fd_out = 0;
    
    ssize_t write_size = 0;

    char *file_buffer = NULL;
    size_t file_size = 0;
    
    t_elf elf = {0};

    Elf64_Addr original_entry;
    Elf64_Addr max_vaddr;
    Elf64_Addr new_stub_vaddr;

    Elf64_Phdr *target_phdr = NULL;

    if (argc != 2) 
        return print_error(WRONG_ARGS, ERRNO_FALSE);
    
    // Read file
    file_size = read_file(argv[1], &file_buffer);
    if (file_size == 0)
        return -1;

    // Parse ELF
    elf = parse_elf(file_buffer);

    // TODO check elf validity

    // Check EP and PT_NOTE
    if (elf.ehdr->e_entry == 0 || check_pt_note(elf) == FALSE)
    {
        exit_code = print_error(INVALID_ELF, ERRNO_TRUE);
        goto cleanup;
    }

    // 원본 Entry Point 저장
    original_entry = elf.ehdr->e_entry;
    // 가장 높은 가상 주소(Vaddr) 찾기
    max_vaddr = find_max_vaddr(elf);
    // 로드 가능한 세그먼트의 끝 주소 계산
    new_stub_vaddr = align_up(max_vaddr, PAGE_SIZE);

    // 파일 오프셋 패딩 계산 
    // 이를 맞추기 위해 파일 크기도 Page Align 시킴.
    uint64_t new_file_offset = align_up(file_size, PAGE_SIZE);
    uint64_t padding_size = new_file_offset - file_size;

    print_debug("    Original Entry: 0x%lx\n", original_entry);
    print_debug("    Max Vaddr: 0x%lx -> New Stub Vaddr: 0x%lx\n", max_vaddr, new_stub_vaddr);
    print_debug("    File Size: %ld -> New Offset: %ld (Padding: %ld)\n", file_size, new_file_offset, padding_size);
    
    unsigned char *patched_stub = malloc(stub_bin_len);
    if (!patched_stub)
    {
        print_error(MEMORY_ALLOCATION_FAILED, ERRNO_FALSE);
        exit_code = -1;
        goto cleanup;
    }
    
    memcpy(patched_stub, stub_bin, stub_bin_len);

    // 스텁 내 원본 Entry Point 점프 주소 패치
    if (patch_placeholder(patched_stub, stub_bin_len, original_entry, ORIGINAL_ENTRY_POINT) == FALSE)
    {
        exit_code = -1;
        goto cleanup;
    }

    // PT_NOTE -> PT_LOAD 변환
    for (int i = 0; i < elf.ehdr->e_phnum; i++)
    {
        if (elf.phdrs[i].p_type == PT_NOTE)
        {
            target_phdr = &elf.phdrs[i];
            break;
        }
    }

    // stub 실행 가능하도록 PT_LOAD 설정
    target_phdr->p_type = PT_LOAD;
    target_phdr->p_flags = PF_R | PF_X | PF_W; // Read | Write | Execute
    target_phdr->p_offset = new_file_offset;   // 패딩 뒤 위치
    target_phdr->p_vaddr = new_stub_vaddr;     // 메모리 로드 주소
    target_phdr->p_paddr = new_stub_vaddr;
    target_phdr->p_filesz = stub_bin_len;  // 스텁 크기
    target_phdr->p_memsz = stub_bin_len;
    target_phdr->p_align = PAGE_SIZE;
    
    print_debug("    [+] PT_NOTE converted to PT_LOAD\n");

    // Entry Point를 Stub의 시작 주소로 변경
    elf.ehdr->e_entry = new_stub_vaddr;
    print_debug("    [+] Entry Point updated to 0x%lx\n", elf.ehdr->e_entry);

    // 출력 파일 이름 생성 (woody)
    fd_out = open(OUTPUT_FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd_out < 0)
    {
        print_error(FILE_NOT_FOUND, ERRNO_TRUE); // Write error handling
        free(patched_stub);
        goto cleanup;
    }

    // 1) 수정된 헤더가 포함된 원본 파일 내용 쓰기
    if (write(fd_out, file_buffer, file_size) != (ssize_t)file_size)
    {
        print_error(PATCH_HEADER_WRITE_ERROR, ERRNO_TRUE); 
        goto cleanup_write;
    }

    // 2) 패딩 쓰기 (0x00)
    if (padding_size > 0)
    {
        char *padding = calloc(1, padding_size);
        
        if (write(fd_out, padding, padding_size) != (ssize_t)padding_size)
        {
            print_error(PADDING_WRITE_ERROR, ERRNO_TRUE); 
            free(padding);
            goto cleanup_write;
        }
        free(padding);
    }

    // 3) 스텁 코드 쓰기
    if (write(fd_out, patched_stub, stub_bin_len) != (ssize_t)stub_bin_len)
    {
        print_error(STUB_WRITE_ERROR, ERRNO_TRUE); 
        goto cleanup_write;
    }

    print_debug("\n[SUCCESS] Packed file created: %s\n", OUTPUT_FILENAME);

cleanup_write:
    close(fd_out);
    free(patched_stub);

cleanup:
    free(file_buffer);

    return exit_code;
}
