#include <stdio.h>
#include <stdlib.h>
#include <elf.h>



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
    printf(" Type: 0x%x\n", ehdr.e_type);
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

    fclose(fp);
    return 0;
}
