/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_parser.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/12/11 23:07:32 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ELF_PARSER_H
# define ELF_PARSER_H

typedef struct s_elf
{
    Elf64_Ehdr    *ehdr;
    Elf64_Phdr    *phdrs;
    Elf64_Shdr    *shdrs;
    char          *section_strtab;
}   t_elf;

t_elf parse_elf(char *file_buffer);
int   check_pt_note(t_elf elf);
Elf64_Addr find_max_vaddr(t_elf elf);

const static char *ph_types[10] = 
{
    "PT_NULL", "PT_LOAD", "PT_DYNAMIC", "PT_INTERP", "PT_NOTE",
    "PT_SHLIB", "PT_PHDR", "PT_LOPROC", "PT_HIPROC", "PT_GNU_STACK" 
};

const static char *ptype_to_str(uint32_t p_type)
{
    switch (p_type)
    {
        case PT_NULL:        return "PT_NULL";
        case PT_LOAD:        return "PT_LOAD";
        case PT_DYNAMIC:     return "PT_DYNAMIC";
        case PT_INTERP:      return "PT_INTERP";
        case PT_NOTE:        return "PT_NOTE";
        case PT_SHLIB:       return "PT_SHLIB";
        case PT_PHDR:        return "PT_PHDR";
        case PT_LOPROC:      return "PT_LOPROC";
        case PT_HIPROC:      return "PT_HIPROC";
        case PT_GNU_STACK:   return "PT_GNU_STACK";
        default:             // assert(0);
                            return "UNKNOWN";
    }
}

const static char *sh_types[16] = 
{
    "SHT_NULL", "SHT_PROGBITS", "SHT_SYMTAB", "SHT_STRTAB", "SHT_RELA",
    "SHT_HASH", "SHT_DYNAMIC", "SHT_NOTE", "SHT_NOBITS", "SHT_REL", 
    "SHT_SHLIB", "SHT_DYNSYM", "SHT_LOPROC", "SHT_HIPROC", "SHT_LOUSER", "SHT_HIUSER"
};

const static char *shtype_to_str(uint32_t sh_type)
{
    switch (sh_type)
    {
        case SHT_NULL:        return "SHT_NULL";
        case SHT_PROGBITS:    return "SHT_PROGBITS";
        case SHT_SYMTAB:      return "SHT_SYMTAB";
        case SHT_STRTAB:      return "SHT_STRTAB";
        case SHT_RELA:        return "SHT_RELA";
        case SHT_HASH:        return "SHT_HASH";
        case SHT_DYNAMIC:     return "SHT_DYNAMIC";
        case SHT_NOTE:        return "SHT_NOTE";
        case SHT_NOBITS:      return "SHT_NOBITS";
        case SHT_REL:         return "SHT_REL";
        case SHT_SHLIB:       return "SHT_SHLIB";
        case SHT_DYNSYM:      return "SHT_DYNSYM";
        case SHT_LOPROC:      return "SHT_LOPROC";
        case SHT_HIPROC:      return "SHT_HIPROC";
        case SHT_LOUSER:      return "SHT_LOUSER";
        case SHT_HIUSER:      return "SHT_HIUSER";
        default:              // assert(0); 
                              return "UNKNOWN";
    }
}

#endif