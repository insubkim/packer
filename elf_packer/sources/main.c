/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/12/11 22:57:29 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "file.h"
#include "print_utils.h"
#include "elf_parser.h"
#include "stub.h"
#include <string.h>

#define PAGE_SIZE 0x1000
#define DEBUG 1
#define PLACEHOLDER 0x1122334455667788
#define KEY 0xCCCCCCCCCCCCCCCC

uint64_t align_up(uint64_t val, uint64_t align)
{
    return (val + align - 1) & ~(align - 1);
}

int main(int argc, char *argv[])
{
    int exit_code = 0;

    if (argc != 2) 
        return print_error(WRONG_ARGS, ERRNO_FALSE);
    
    // 1. Read file
    char *file_buffer = NULL;
    size_t file_size = read_file(argv[1], &file_buffer);
    if (file_size == 0)
        return -1;

    // 2. Parse ELF
    t_elf elf = parse_elf(file_buffer);

    // 3. Check EP and PT_NOTE
    if (elf.ehdr->e_entry == 0 || check_pt_note(elf) == FALSE)
    {
        exit_code = print_error(INVALID_ELF, ERRNO_TRUE);
        goto cleanup;
    }

    print_debug("\n[+] Starting Packing Process...\n");

    // 원본 Entry Point 저장
    Elf64_Addr original_entry = elf.ehdr->e_entry;

    // 가장 높은 가상 주소(Vaddr) 찾기
    Elf64_Addr max_vaddr = 0;
    for (int i = 0; i < elf.ehdr->e_phnum; i++)
    {
        if (elf.phdrs[i].p_type == PT_LOAD)
        {
            Elf64_Addr end_vaddr = elf.phdrs[i].p_vaddr + elf.phdrs[i].p_memsz;
            if (end_vaddr > max_vaddr)
                max_vaddr = end_vaddr;
        }
    }
    
    Elf64_Addr new_stub_vaddr = align_up(max_vaddr, PAGE_SIZE);

    // (3) 파일 오프셋 패딩 계산 
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

    uint64_t placeholder = PLACEHOLDER; 
    int patched = FALSE;

    for (unsigned int i = 0; i < stub_bin_len - 8; i++)
    {
        uint64_t *ptr = (uint64_t *)&patched_stub[i];
        if (*ptr == placeholder)
        {
            *ptr = (uint64_t)original_entry;
            patched = TRUE;
            print_debug("    [+] Stub patched at offset %d with OEP 0x%lx\n", i, original_entry);
            break;
        }
    }
    
    if (!patched)
    {
        print_debug("    [!] Error: Placeholder pattern not found in stub!\n");
        exit_code = -1;
        goto cleanup;
    }

    // PT_LOAD 섹션을 찾아서 암호화
    for (int i = 0; i < elf.ehdr->e_phnum; i++)
    {
        Elf64_Phdr *phdr = &elf.phdrs[i];
        
        if (phdr->p_type != PT_LOAD || (phdr->p_flags & PF_X) == FALSE)
            continue;

        //  첫 PT_LOAD 는 헤더 이후 암호화.
        if (phdr->p_offset == 0) {
            encrypt_start = phdr->p_offset + header_size;
            encrypt_len   = phdr->p_filesz - header_size;
        } else {
            encrypt_start = phdr->p_offset;
            encrypt_len   = phdr->p_filesz;
        }
        
        // 3. XOR 암호화 수행
        xor_encrypt(buffer + encrypt_start, encrypt_len, key);
        
        // stub start_address, key, size 변경

        break; // 첫 번째 실행 가능한 PT_LOAD만 암호화
    }
    
    // PT_NOTE -> PT_LOAD 변환
    Elf64_Phdr *target_phdr = NULL;
    for (int i = 0; i < elf.ehdr->e_phnum; i++)
    {
        if (elf.phdrs[i].p_type == PT_NOTE)
        {
            target_phdr = &elf.phdrs[i];
            break;
        }
    }

    if (target_phdr)
    {
        target_phdr->p_type = PT_LOAD;
        target_phdr->p_flags = PF_R | PF_X | PF_W; // Read | Write | Execute
        target_phdr->p_offset = new_file_offset;   // 패딩 뒤 위치
        target_phdr->p_vaddr = new_stub_vaddr;     // 메모리 로드 주소
        target_phdr->p_paddr = new_stub_vaddr;
        target_phdr->p_filesz = stub_bin_len;  // 스텁 크기
        target_phdr->p_memsz = stub_bin_len;
        target_phdr->p_align = PAGE_SIZE;

        print_debug("    [+] PT_NOTE converted to PT_LOAD\n");
    }

    // Entry Point를 Stub의 시작 주소로 변경
    elf.ehdr->e_entry = new_stub_vaddr;
    print_debug("    [+] Entry Point updated to 0x%lx\n", elf.ehdr->e_entry);

    // 출력 파일 이름 생성 (woody)
    char *out_name = "woody";
    int fd_out = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd_out < 0)
    {
        print_error(FILE_NOT_FOUND, ERRNO_TRUE); // Write error handling
        free(patched_stub);
        goto cleanup;
    }

    // 1) 수정된 헤더가 포함된 원본 파일 내용 쓰기
    write(fd_out, file_buffer, file_size);

    // 2) 패딩 쓰기 (0x00)
    if (padding_size > 0)
    {
        char *padding = calloc(1, padding_size);
        write(fd_out, padding, padding_size);
        free(padding);
    }

    // 3) 스텁 코드 쓰기
    write(fd_out, patched_stub, stub_bin_len);

    close(fd_out);
    free(patched_stub);
    print_debug("\n[SUCCESS] Packed file created: %s\n", out_name);

cleanup:
    free(file_buffer);

    return exit_code;
}
