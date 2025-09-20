#include <stdio.h>
#include <stdlib.h>
#include <elf.h>


/*  elf type    
    ElfN_Addr       Unsigned program address, uintN_t
    ElfN_Off        Unsigned file offset, uintN_t
    ElfN_Section    Unsigned section index, uint16_t
    ElfN_Versym     Unsigned version symbol information, uint16_t
    Elf_Byte        unsigned char
    ElfN_Half       uint16_t
    ElfN_Sword      int32_t
    ElfN_Word       uint32_t
    ElfN_Sxword     int64_t
    ElfN_Xword      uint64_t
*/

// typedef struct
// {
//   unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
//   Elf64_Half	e_type;			/* Object file type */
//   Elf64_Half	e_machine;		/* Architecture */
//   Elf64_Word	e_version;		/* Object file version */
//   Elf64_Addr	e_entry;		/* Entry point virtual address */
//   Elf64_Off	e_phoff;		/* Program header table file offset */
//   Elf64_Off	e_shoff;		/* Section header table file offset */
//   Elf64_Word	e_flags;		/* Processor-specific flags */
//   Elf64_Half	e_ehsize;		/* ELF header size in bytes */
//   Elf64_Half	e_phentsize;		/* Program header table entry size */
//   Elf64_Half	e_phnum;		/* Program header table entry count */
//   Elf64_Half	e_shentsize;		/* Section header table entry size */
//   Elf64_Half	e_shnum;		/* Section header table entry count */
//   Elf64_Half	e_shstrndx;		/* Section header string table index */
// } Elf64_Ehdr;

// typedef struct
// {
//   Elf64_Word	p_type;			/* Segment type */
//   Elf64_Word	p_flags;		/* Segment flags */
//   Elf64_Off	p_offset;		/* Segment file offset */
//   Elf64_Addr	p_vaddr;		/* Segment virtual address */
//   Elf64_Addr	p_paddr;		/* Segment physical address */
//   Elf64_Xword	p_filesz;		/* Segment size in file */
//   Elf64_Xword	p_memsz;		/* Segment size in memory */
//   Elf64_Xword	p_align;		/* Segment alignment */
// } Elf64_Phdr;

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
    printf(" Magic: %02x %02x %02x %02x\n",
           ehdr.e_ident[0], ehdr.e_ident[1],
           ehdr.e_ident[2], ehdr.e_ident[3]);
    printf(" Class: %s\n", ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" :
                             ehdr.e_ident[EI_CLASS] == ELFCLASS32 ? "ELF32" : "Invalid");
    printf(" Data: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little Endian" :
                           ehdr.e_ident[EI_DATA] == ELFDATA2MSB ? "Big Endian" : "Invalid");
    printf(" Version: %d\n", ehdr.e_ident[EI_VERSION]);
    printf(" OS/ABI: %d\n", ehdr.e_ident[EI_OSABI]);
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
    printf(" Type: 0x%x [%s]\n", ehdr.e_type, elf_type[ehdr.e_type]);
    printf(" Machine: 0x%x\n", ehdr.e_machine);
    printf(" Entry point: 0x%lx\n", (unsigned long)ehdr.e_entry);
    printf(" Program header offset: %ld\n", (long)ehdr.e_phoff);
    printf(" Section header offset: %ld\n", (long)ehdr.e_shoff);
    printf(" Flags: 0x%x\n", ehdr.e_flags);
    printf(" ELF header size: %d\n", ehdr.e_ehsize);
    printf(" Program header entry size: %d\n", ehdr.e_phentsize);
    printf(" Program header entry count: %d\n", ehdr.e_phnum);
    printf(" Section header entry size: %d\n", ehdr.e_shentsize);
    printf(" Section header entry count: %d\n", ehdr.e_shnum);
    printf(" Section header string table index: %d\n", ehdr.e_shstrndx);

    
    Elf64_Phdr eph;  
    if (fread(&eph, 1, sizeof(eph), fp) != sizeof(eph)) {
        perror("fread");
        fclose(fp);
        return 1;
    }
/*
                 PT_NULL
                        The array element is unused and the other
                        members' values are undefined.  This lets the
                        program header have ignored entries.

                 PT_LOAD
                        The array element specifies a loadable segment,
                        described by p_filesz and p_memsz.  The bytes
                        from the file are mapped to the beginning of the
                        memory segment.  If the segment's memory size
                        p_memsz is larger than the file size p_filesz,
                        the "extra" bytes are defined to hold the value 0
                        and to follow the segment's initialized area.
                        The file size may not be larger than the memory
                        size.  Loadable segment entries in the program
                        header table appear in ascending order, sorted on
                        the p_vaddr member.

                 PT_DYNAMIC
                        The array element specifies dynamic linking
                        information.

                 PT_INTERP
                        The array element specifies the location and size
                        of a null-terminated pathname to invoke as an
                        interpreter.  This segment type is meaningful
                        only for executable files (though it may occur
                        for shared objects).  However it may not occur
                        more than once in a file.  If it is present, it
                        must precede any loadable segment entry.

                 PT_NOTE
                        The array element specifies the location of notes
                        (ElfN_Nhdr).

                 PT_SHLIB
                        This segment type is reserved but has unspecified
                        semantics.  Programs that contain an array
                        element of this type do not conform to the ABI.

                 PT_PHDR
                        The array element, if present, specifies the
                        location and size of the program header table
                        itself, both in the file and in the memory image
                        of the program.  This segment type may not occur
                        more than once in a file.  Moreover, it may occur
                        only if the program header table is part of the
                        memory image of the program.  If it is present,
                        it must precede any loadable segment entry.

                 PT_LOPROC
                 PT_HIPROC
                        Values in the inclusive range [PT_LOPROC,
                        PT_HIPROC] are reserved for processor-specific
                        semantics.

                 PT_GNU_STACK
                        GNU extension which is used by the Linux kernel
                        to control the state of the stack via the flags
                        set in the p_flags member.
*/
    const char *ph_types[10] = 
    {
        "PT_NULL", "PT_LOAD", "PT_DYNAMIC", "PT_INTERP", "PT_NOTE",
        "PT_SHLIB", "PT_PHDR", "PT_LOPROC", "PT_HIPROC", "PT_GNU_STACK" 
    };
    printf(" Program Header type :0x%x [%s]\n", eph.p_type, ph_types[eph.p_type]);

    fclose(fp);
    return 0;
}
