#include "elf_parser.h"
#include <string.h>

int validate_elf_magic(const Elf64_Ehdr *ehdr) {
    if (!ehdr) return -1;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr->e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr->e_ident[EI_MAG3] != ELFMAG3) {
        return -1;
    }
    return 0;
}

const char* ptype_to_str(uint32_t type) {
    switch (type) {
        case PT_NULL:    return "PT_NULL";
        case PT_LOAD:    return "PT_LOAD";
        case PT_DYNAMIC: return "PT_DYNAMIC";
        case PT_INTERP:  return "PT_INTERP";
        case PT_NOTE:    return "PT_NOTE";
        case PT_SHLIB:   return "PT_SHLIB";
        case PT_PHDR:    return "PT_PHDR";
#ifdef PT_TLS
        case PT_TLS:     return "PT_TLS";
#endif
#ifdef PT_GNU_EH_FRAME
        case PT_GNU_EH_FRAME: return "PT_GNU_EH_FRAME";
#endif
#ifdef PT_GNU_STACK
        case PT_GNU_STACK:    return "PT_GNU_STACK";
#endif
#ifdef PT_GNU_RELRO
        case PT_GNU_RELRO:    return "PT_GNU_RELRO";
#endif
        default: return "UNKNOWN";
    }
}

const char* elf_class_to_str(uint8_t elf_class) {
    switch (elf_class) {
        case ELFCLASS32: return "ELF32";
        case ELFCLASS64: return "ELF64";
        default:         return "Invalid";
    }
}

const char* elf_data_to_str(uint8_t data) {
    switch (data) {
        case ELFDATA2LSB: return "Little Endian";
        case ELFDATA2MSB: return "Big Endian";
        default:          return "Invalid";
    }
}

const char* elf_type_to_str(uint16_t type) {
    switch (type) {
        case ET_NONE: return "ET_NONE";
        case ET_REL:  return "ET_REL";
        case ET_EXEC: return "ET_EXEC";
        case ET_DYN:  return "ET_DYN";
        case ET_CORE: return "ET_CORE";
        default:      return "UNKNOWN";
    }
}

void parse_phdr_flags(uint32_t flags, char *out, size_t out_size) {
    if (out_size < 4) return;
    out[0] = (flags & PF_R) ? 'R' : ' ';
    out[1] = (flags & PF_W) ? 'W' : ' ';
    out[2] = (flags & PF_X) ? 'E' : ' ';
    out[3] = '\0';
}

int read_elf_header(FILE *fp, Elf64_Ehdr *ehdr) {
    if (!fp || !ehdr) return -1;
    if (fread(ehdr, 1, sizeof(*ehdr), fp) != sizeof(*ehdr)) {
        return -1;
    }
    return 0;
}

int read_program_header(FILE *fp, Elf64_Phdr *phdr) {
    if (!fp || !phdr) return -1;
    if (fread(phdr, 1, sizeof(*phdr), fp) != sizeof(*phdr)) {
        return -1;
    }
    return 0;
}

void print_elf_header(const Elf64_Ehdr *ehdr) {
    if (!ehdr) return;

    printf("ELF Header:\n");
    printf("\tMagic: %02x %02x %02x %02x\n",
           ehdr->e_ident[0], ehdr->e_ident[1],
           ehdr->e_ident[2], ehdr->e_ident[3]);
    printf("\tClass: %s\n", elf_class_to_str(ehdr->e_ident[EI_CLASS]));
    printf("\tData: %s\n", elf_data_to_str(ehdr->e_ident[EI_DATA]));
    printf("\tVersion: %d\n", ehdr->e_ident[EI_VERSION]);
    printf("\tOS/ABI: %d\n", ehdr->e_ident[EI_OSABI]);
    printf("\tType: 0x%x [%s]\n", ehdr->e_type, elf_type_to_str(ehdr->e_type));
    printf("\tMachine: 0x%x\n", ehdr->e_machine);
    printf("\tEntry point: 0x%lx\n", (unsigned long)ehdr->e_entry);
    printf("\tProgram header offset: %ld\n", (long)ehdr->e_phoff);
    printf("\tSection header offset: %ld\n", (long)ehdr->e_shoff);
    printf("\tFlags: 0x%x\n", ehdr->e_flags);
    printf("\tELF header size: %d\n", ehdr->e_ehsize);
    printf("\tProgram header entry size: %d | sizeof Elf64_Phdr: %zu\n",
           ehdr->e_phentsize, sizeof(Elf64_Phdr));
    printf("\tProgram header entry count: %d\n", ehdr->e_phnum);
    printf("\tSection header entry size: %d\n", ehdr->e_shentsize);
    printf("\tSection header entry count: %d\n", ehdr->e_shnum);
    printf("\tSection header string table index: %d\n", ehdr->e_shstrndx);
}

void print_program_header(const Elf64_Phdr *phdr) {
    if (!phdr) return;

    char flags[4];
    parse_phdr_flags(phdr->p_flags, flags, sizeof(flags));

    printf("Program Header:\n");
    printf("\tProgram Header type: 0x%x [%s]\n", phdr->p_type, ptype_to_str(phdr->p_type));
    printf("\tProgram Header flag: 0x%x [%s]\n", phdr->p_flags, flags);
    printf("\tProgram Header offset: 0x%lx\n", (unsigned long)phdr->p_offset);
    printf("\tProgram Header virtual address: 0x%lx\n", (unsigned long)phdr->p_vaddr);
    printf("\tProgram Header physical address: 0x%lx\n", (unsigned long)phdr->p_paddr);
    printf("\tProgram Header file size: 0x%lx\n", (unsigned long)phdr->p_filesz);
    printf("\tProgram Header memory size: 0x%lx\n", (unsigned long)phdr->p_memsz);
    printf("\tProgram Header alignment: 0x%lx\n", (unsigned long)phdr->p_align);
}
