/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/11/17 21:07:58 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include "print_utils.h"
#include "elf_parser.h"

t_elf   parse_elf(char *file_buffer)
{
    t_elf elf = {0};

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_buffer;
    elf.ehdr = ehdr;
    elf.phdrs = (Elf64_Phdr *)(file_buffer + ehdr->e_phoff);
    elf.shdrs = (Elf64_Shdr *)(file_buffer + ehdr->e_shoff);
    elf.section_strtab = file_buffer + elf.shdrs[ehdr->e_shstrndx].sh_offset;

    print_debug("ELF Entry Point: 0x%lx\n", elf.ehdr->e_entry);
    print_debug("Number of Program Headers: %d\n", elf.ehdr->e_phnum);
    print_debug("Number of Section Headers: %d\n", elf.ehdr->e_shnum);
    for (int i = 0; i < elf.ehdr->e_phnum; i++)
    {
        Elf64_Phdr *phdr = &elf.phdrs[i];
        print_debug("Program Header %d: Type: %s, Offset: 0x%lx, Vaddr: 0x%lx, Filesz: 0x%lx\n",
            i, ptype_to_str(phdr->p_type), phdr->p_offset, phdr->p_vaddr, phdr->p_filesz);
    }
    
    for (int i = 0; i < elf.ehdr->e_shnum; i++)
    {
        Elf64_Shdr *shdr = &elf.shdrs[i];
        const char *section_name = elf.section_strtab + shdr->sh_name;
        print_debug("Section Header %d: Name: %s, Type: %s, Offset: 0x%lx, Addr: 0x%lx, Size: 0x%lx\n",
            i, section_name, shtype_to_str(shdr->sh_type), shdr->sh_offset, shdr->sh_addr, shdr->sh_size);
    }

    return elf;
}

int check_pt_note(t_elf elf)
{
    for (int i = 0; i < elf.ehdr->e_phnum; i++)
    {
        Elf64_Phdr *phdr = &elf.phdrs[i];
        if (phdr->p_type == PT_NOTE)
            return TRUE;
    }

    print_debug("No PT_NOTE segment found.\n");

    return FALSE;
}