#include <stdio.h>
#include <stdlib.h>
#include "elf_parser.h"

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
    if (read_elf_header(fp, &ehdr) != 0) {
        perror("fread");
        fclose(fp);
        return 1;
    }

    if (validate_elf_magic(&ehdr) != 0) {
        fprintf(stderr, "Not an ELF file\n");
        fclose(fp);
        return 1;
    }

    print_elf_header(&ehdr);

    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf64_Phdr phdr;
        if (read_program_header(fp, &phdr) != 0) {
            perror("fread program header");
            fclose(fp);
            return 1;
        }
        print_program_header(&phdr);
    }

    fclose(fp);
    return 0;
}
