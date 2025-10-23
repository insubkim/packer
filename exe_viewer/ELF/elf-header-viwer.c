#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <elf.h>

static const char* ptype_to_str(uint32_t t) {
    switch (t) {
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
        // GNU extensions (glibc elf.h에 정의)
#ifdef PT_GNU_EH_FRAME
        case PT_GNU_EH_FRAME: return "PT_GNU_EH_FRAME";
#endif
#ifdef PT_GNU_STACK
        case PT_GNU_STACK:    return "PT_GNU_STACK";
#endif
#ifdef PT_GNU_RELRO
        case PT_GNU_RELRO:    return "PT_GNU_RELRO";
#endif
        default: assert(0); return "UNKNOWN";// error
    }
}

static void read_elf_program_header(FILE *fp)
{
    Elf64_Phdr eph;  
    if (fread(&eph, 1, sizeof(eph), fp) != sizeof(eph)) {
        perror("fread");
        fclose(fp);
        exit(1);
    }

    printf("Program Header:\n");
    const char *ph_types[10] = 
    {
        "PT_NULL", "PT_LOAD", "PT_DYNAMIC", "PT_INTERP", "PT_NOTE",
        "PT_SHLIB", "PT_PHDR", "PT_LOPROC", "PT_HIPROC", "PT_GNU_STACK" 
    };
    // printf("\tProgram Header type :0x%x [%s]\n", eph.p_type, ph_types[eph.p_type]);
    printf("\tProgram Header type :0x%x [%s]\n", eph.p_type, ptype_to_str(eph.p_type));
    char ph_flags[4] = {0, }; 
    ph_flags[0] = eph.p_flags & PF_R ? 'R' : ' ';
    ph_flags[1] = eph.p_flags & PF_W ? 'W' : ' ';
    ph_flags[2] = eph.p_flags & PF_X ? 'E' : ' ';
    printf("\tProgram Header flag :0x%x [%s]\n", eph.p_flags, ph_flags);
    printf("\tProgram Header offset :0x%x size : %llu\n", eph.p_offset, sizeof(eph.p_offset));
    printf("\tProgram Header virtual address :0x%x\n", eph.p_vaddr);
    printf("\tProgram Header segment's physical address :0x%x\n", eph.p_paddr);
    printf("\tProgram Header file image of the segment :0x%x\n", eph.p_filesz);
    printf("\tProgram Header memory image of the segment :0x%x\n", eph.p_memsz);
    printf("\tProgram Header aligned ? :0x%x\n", eph.p_align);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
        perror("fread");
        fclose(fp);
        return 1;
    }

    // Check magic
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Not an ELF file\n");
        fclose(fp);
        return 1;
    }

    printf("ELF Header:\n");
    printf("\tMagic: %02x %02x %02x %02x\n",
           ehdr.e_ident[0], ehdr.e_ident[1],
           ehdr.e_ident[2], ehdr.e_ident[3]);
    printf("\tClass: %s\n", ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" :
                             ehdr.e_ident[EI_CLASS] == ELFCLASS32 ? "ELF32" : "Invalid");
    printf("\tData: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" :
                           ehdr.e_ident[EI_DATA] == ELFDATA2MSB ? "Big Endian" : "Invalid");
    printf("\tVersion: %d\n", ehdr.e_ident[EI_VERSION]);
    printf("\tOS/ABI: %d\n", ehdr.e_ident[EI_OSABI]);
    /*
            ET_NONE An unknown type.
            ET_REL A relocatable file.
            ET_EXEC An executable file.
            ET_DYN A shared object.
            ET_CORE A core file.
    */
    const char *elf_type[5] = {
        "ET_NONE", "ET_REL", "ET_EXEC", "ET_DYN", "ET_CORE"
    };
    printf("\tType: 0x%x [%s]\n", ehdr.e_type, elf_type[ehdr.e_type]);
    printf("\tMachine: 0x%x\n", ehdr.e_machine);
    printf("\tEntry point: 0x%lx\n", (unsigned long)ehdr.e_entry);
    printf("\tProgram header offset: %ld\n", (long)ehdr.e_phoff);
    printf("\tSection header offset: %ld\n", (long)ehdr.e_shoff);
    printf("\tFlags: 0x%x\n", ehdr.e_flags);
    printf("\tELF header size: %d\n", ehdr.e_ehsize);
    printf("\tProgram header entry size: %d | sizeof Elf64_Phdr: %d\n", ehdr.e_phentsize, sizeof(Elf64_Phdr));
    printf("\tProgram header entry count: %d\n", ehdr.e_phnum);
    printf("\tSection header entry size: %d\n", ehdr.e_shentsize);
    printf("\tSection header entry count: %d\n", ehdr.e_shnum);
    printf("\tSection header string table index: %d\n", ehdr.e_shstrndx);

    // read program header for entry count
    for (int i = 0; i < ehdr.e_phnum; i++)
        read_elf_program_header(fp);





    fclose(fp);
    return 0;
}
