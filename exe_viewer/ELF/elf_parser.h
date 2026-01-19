#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <elf.h>

/* ELF validation */
int validate_elf_magic(const Elf64_Ehdr *ehdr);

/* Type to string conversions */
const char* ptype_to_str(uint32_t type);
const char* elf_class_to_str(uint8_t elf_class);
const char* elf_data_to_str(uint8_t data);
const char* elf_type_to_str(uint16_t type);

/* Flag parsing */
void parse_phdr_flags(uint32_t flags, char *out, size_t out_size);

/* Header reading (returns 0 on success, -1 on error) */
int read_elf_header(FILE *fp, Elf64_Ehdr *ehdr);
int read_program_header(FILE *fp, Elf64_Phdr *phdr);

/* Printing */
void print_elf_header(const Elf64_Ehdr *ehdr);
void print_program_header(const Elf64_Phdr *phdr);

#endif /* ELF_PARSER_H */
