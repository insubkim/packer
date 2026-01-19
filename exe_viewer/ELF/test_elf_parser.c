#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "elf_parser.h"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("  Running %s... ", #name); \
    test_##name(); \
    printf("%sPASSED%s\n", GREEN, RESET); \
    tests_passed++; \
} while(0)

#define ASSERT_EQ(expected, actual) do { \
    if ((expected) != (actual)) { \
        printf("%sFAILED%s\n", RED, RESET); \
        printf("    Expected: %d, Got: %d\n", (int)(expected), (int)(actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(expected, actual) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf("%sFAILED%s\n", RED, RESET); \
        printf("    Expected: \"%s\", Got: \"%s\"\n", (expected), (actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        printf("%sFAILED%s\n", RED, RESET); \
        printf("    Expected non-NULL\n"); \
        tests_failed++; \
        return; \
    } \
} while(0)

/*=== validate_elf_magic tests ===*/

TEST(validate_elf_magic_valid) {
    Elf64_Ehdr ehdr = {0};
    ehdr.e_ident[EI_MAG0] = ELFMAG0;  // 0x7f
    ehdr.e_ident[EI_MAG1] = ELFMAG1;  // 'E'
    ehdr.e_ident[EI_MAG2] = ELFMAG2;  // 'L'
    ehdr.e_ident[EI_MAG3] = ELFMAG3;  // 'F'

    ASSERT_EQ(0, validate_elf_magic(&ehdr));
}

TEST(validate_elf_magic_invalid_first_byte) {
    Elf64_Ehdr ehdr = {0};
    ehdr.e_ident[EI_MAG0] = 0x00;  // Wrong
    ehdr.e_ident[EI_MAG1] = ELFMAG1;
    ehdr.e_ident[EI_MAG2] = ELFMAG2;
    ehdr.e_ident[EI_MAG3] = ELFMAG3;

    ASSERT_EQ(-1, validate_elf_magic(&ehdr));
}

TEST(validate_elf_magic_invalid_elf_string) {
    Elf64_Ehdr ehdr = {0};
    ehdr.e_ident[EI_MAG0] = ELFMAG0;
    ehdr.e_ident[EI_MAG1] = 'X';  // Wrong
    ehdr.e_ident[EI_MAG2] = ELFMAG2;
    ehdr.e_ident[EI_MAG3] = ELFMAG3;

    ASSERT_EQ(-1, validate_elf_magic(&ehdr));
}

TEST(validate_elf_magic_null_ptr) {
    ASSERT_EQ(-1, validate_elf_magic(NULL));
}

TEST(validate_elf_magic_all_zeros) {
    Elf64_Ehdr ehdr = {0};
    ASSERT_EQ(-1, validate_elf_magic(&ehdr));
}

/*=== ptype_to_str tests ===*/

TEST(ptype_to_str_pt_null) {
    ASSERT_STR_EQ("PT_NULL", ptype_to_str(PT_NULL));
}

TEST(ptype_to_str_pt_load) {
    ASSERT_STR_EQ("PT_LOAD", ptype_to_str(PT_LOAD));
}

TEST(ptype_to_str_pt_dynamic) {
    ASSERT_STR_EQ("PT_DYNAMIC", ptype_to_str(PT_DYNAMIC));
}

TEST(ptype_to_str_pt_interp) {
    ASSERT_STR_EQ("PT_INTERP", ptype_to_str(PT_INTERP));
}

TEST(ptype_to_str_pt_note) {
    ASSERT_STR_EQ("PT_NOTE", ptype_to_str(PT_NOTE));
}

TEST(ptype_to_str_pt_phdr) {
    ASSERT_STR_EQ("PT_PHDR", ptype_to_str(PT_PHDR));
}

TEST(ptype_to_str_unknown) {
    ASSERT_STR_EQ("UNKNOWN", ptype_to_str(0xDEADBEEF));
}

/*=== elf_class_to_str tests ===*/

TEST(elf_class_to_str_32) {
    ASSERT_STR_EQ("ELF32", elf_class_to_str(ELFCLASS32));
}

TEST(elf_class_to_str_64) {
    ASSERT_STR_EQ("ELF64", elf_class_to_str(ELFCLASS64));
}

TEST(elf_class_to_str_invalid) {
    ASSERT_STR_EQ("Invalid", elf_class_to_str(99));
}

/*=== elf_data_to_str tests ===*/

TEST(elf_data_to_str_little_endian) {
    ASSERT_STR_EQ("Little Endian", elf_data_to_str(ELFDATA2LSB));
}

TEST(elf_data_to_str_big_endian) {
    ASSERT_STR_EQ("Big Endian", elf_data_to_str(ELFDATA2MSB));
}

TEST(elf_data_to_str_invalid) {
    ASSERT_STR_EQ("Invalid", elf_data_to_str(99));
}

/*=== elf_type_to_str tests ===*/

TEST(elf_type_to_str_none) {
    ASSERT_STR_EQ("ET_NONE", elf_type_to_str(ET_NONE));
}

TEST(elf_type_to_str_rel) {
    ASSERT_STR_EQ("ET_REL", elf_type_to_str(ET_REL));
}

TEST(elf_type_to_str_exec) {
    ASSERT_STR_EQ("ET_EXEC", elf_type_to_str(ET_EXEC));
}

TEST(elf_type_to_str_dyn) {
    ASSERT_STR_EQ("ET_DYN", elf_type_to_str(ET_DYN));
}

TEST(elf_type_to_str_core) {
    ASSERT_STR_EQ("ET_CORE", elf_type_to_str(ET_CORE));
}

TEST(elf_type_to_str_unknown) {
    ASSERT_STR_EQ("UNKNOWN", elf_type_to_str(0xFFFF));
}

/*=== parse_phdr_flags tests ===*/

TEST(parse_phdr_flags_read_only) {
    char flags[4];
    parse_phdr_flags(PF_R, flags, sizeof(flags));
    ASSERT_STR_EQ("R  ", flags);
}

TEST(parse_phdr_flags_read_write) {
    char flags[4];
    parse_phdr_flags(PF_R | PF_W, flags, sizeof(flags));
    ASSERT_STR_EQ("RW ", flags);
}

TEST(parse_phdr_flags_read_exec) {
    char flags[4];
    parse_phdr_flags(PF_R | PF_X, flags, sizeof(flags));
    ASSERT_STR_EQ("R E", flags);
}

TEST(parse_phdr_flags_all) {
    char flags[4];
    parse_phdr_flags(PF_R | PF_W | PF_X, flags, sizeof(flags));
    ASSERT_STR_EQ("RWE", flags);
}

TEST(parse_phdr_flags_none) {
    char flags[4];
    parse_phdr_flags(0, flags, sizeof(flags));
    ASSERT_STR_EQ("   ", flags);
}

/*=== read_elf_header tests ===*/

TEST(read_elf_header_null_fp) {
    Elf64_Ehdr ehdr;
    ASSERT_EQ(-1, read_elf_header(NULL, &ehdr));
}

TEST(read_elf_header_null_ehdr) {
    FILE *fp = tmpfile();
    ASSERT_EQ(-1, read_elf_header(fp, NULL));
    fclose(fp);
}

TEST(read_elf_header_empty_file) {
    FILE *fp = tmpfile();
    Elf64_Ehdr ehdr;
    ASSERT_EQ(-1, read_elf_header(fp, &ehdr));
    fclose(fp);
}

TEST(read_elf_header_valid) {
    FILE *fp = tmpfile();

    // Write a minimal ELF header
    Elf64_Ehdr write_ehdr = {0};
    write_ehdr.e_ident[EI_MAG0] = ELFMAG0;
    write_ehdr.e_ident[EI_MAG1] = ELFMAG1;
    write_ehdr.e_ident[EI_MAG2] = ELFMAG2;
    write_ehdr.e_ident[EI_MAG3] = ELFMAG3;
    write_ehdr.e_ident[EI_CLASS] = ELFCLASS64;
    write_ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    write_ehdr.e_type = ET_EXEC;
    fwrite(&write_ehdr, 1, sizeof(write_ehdr), fp);
    rewind(fp);

    Elf64_Ehdr read_ehdr;
    ASSERT_EQ(0, read_elf_header(fp, &read_ehdr));
    ASSERT_EQ(0, validate_elf_magic(&read_ehdr));
    ASSERT_EQ(ET_EXEC, read_ehdr.e_type);

    fclose(fp);
}

/*=== read_program_header tests ===*/

TEST(read_program_header_null_fp) {
    Elf64_Phdr phdr;
    ASSERT_EQ(-1, read_program_header(NULL, &phdr));
}

TEST(read_program_header_null_phdr) {
    FILE *fp = tmpfile();
    ASSERT_EQ(-1, read_program_header(fp, NULL));
    fclose(fp);
}

TEST(read_program_header_valid) {
    FILE *fp = tmpfile();

    Elf64_Phdr write_phdr = {0};
    write_phdr.p_type = PT_LOAD;
    write_phdr.p_flags = PF_R | PF_X;
    write_phdr.p_vaddr = 0x400000;
    fwrite(&write_phdr, 1, sizeof(write_phdr), fp);
    rewind(fp);

    Elf64_Phdr read_phdr;
    ASSERT_EQ(0, read_program_header(fp, &read_phdr));
    ASSERT_EQ(PT_LOAD, read_phdr.p_type);
    ASSERT_EQ(PF_R | PF_X, read_phdr.p_flags);

    fclose(fp);
}

/*=== Integration test ===*/

TEST(integration_full_elf_parse) {
    FILE *fp = tmpfile();

    // Write ELF header
    Elf64_Ehdr ehdr = {0};
    ehdr.e_ident[EI_MAG0] = ELFMAG0;
    ehdr.e_ident[EI_MAG1] = ELFMAG1;
    ehdr.e_ident[EI_MAG2] = ELFMAG2;
    ehdr.e_ident[EI_MAG3] = ELFMAG3;
    ehdr.e_ident[EI_CLASS] = ELFCLASS64;
    ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr.e_type = ET_DYN;
    ehdr.e_phnum = 2;
    ehdr.e_phoff = sizeof(Elf64_Ehdr);
    fwrite(&ehdr, 1, sizeof(ehdr), fp);

    // Write program headers
    Elf64_Phdr phdr1 = {0};
    phdr1.p_type = PT_LOAD;
    phdr1.p_flags = PF_R | PF_X;
    fwrite(&phdr1, 1, sizeof(phdr1), fp);

    Elf64_Phdr phdr2 = {0};
    phdr2.p_type = PT_LOAD;
    phdr2.p_flags = PF_R | PF_W;
    fwrite(&phdr2, 1, sizeof(phdr2), fp);

    rewind(fp);

    // Read and verify
    Elf64_Ehdr read_ehdr;
    ASSERT_EQ(0, read_elf_header(fp, &read_ehdr));
    ASSERT_EQ(0, validate_elf_magic(&read_ehdr));
    ASSERT_EQ(ET_DYN, read_ehdr.e_type);
    ASSERT_EQ(2, read_ehdr.e_phnum);

    Elf64_Phdr read_phdr;
    ASSERT_EQ(0, read_program_header(fp, &read_phdr));
    ASSERT_EQ(PT_LOAD, read_phdr.p_type);
    ASSERT_EQ(PF_R | PF_X, read_phdr.p_flags);

    ASSERT_EQ(0, read_program_header(fp, &read_phdr));
    ASSERT_EQ(PT_LOAD, read_phdr.p_type);
    ASSERT_EQ(PF_R | PF_W, read_phdr.p_flags);

    fclose(fp);
}

int main(void) {
    printf("========================================\n");
    printf("   ELF Parser Unit Tests\n");
    printf("========================================\n\n");

    printf("[validate_elf_magic]\n");
    RUN_TEST(validate_elf_magic_valid);
    RUN_TEST(validate_elf_magic_invalid_first_byte);
    RUN_TEST(validate_elf_magic_invalid_elf_string);
    RUN_TEST(validate_elf_magic_null_ptr);
    RUN_TEST(validate_elf_magic_all_zeros);

    printf("\n[ptype_to_str]\n");
    RUN_TEST(ptype_to_str_pt_null);
    RUN_TEST(ptype_to_str_pt_load);
    RUN_TEST(ptype_to_str_pt_dynamic);
    RUN_TEST(ptype_to_str_pt_interp);
    RUN_TEST(ptype_to_str_pt_note);
    RUN_TEST(ptype_to_str_pt_phdr);
    RUN_TEST(ptype_to_str_unknown);

    printf("\n[elf_class_to_str]\n");
    RUN_TEST(elf_class_to_str_32);
    RUN_TEST(elf_class_to_str_64);
    RUN_TEST(elf_class_to_str_invalid);

    printf("\n[elf_data_to_str]\n");
    RUN_TEST(elf_data_to_str_little_endian);
    RUN_TEST(elf_data_to_str_big_endian);
    RUN_TEST(elf_data_to_str_invalid);

    printf("\n[elf_type_to_str]\n");
    RUN_TEST(elf_type_to_str_none);
    RUN_TEST(elf_type_to_str_rel);
    RUN_TEST(elf_type_to_str_exec);
    RUN_TEST(elf_type_to_str_dyn);
    RUN_TEST(elf_type_to_str_core);
    RUN_TEST(elf_type_to_str_unknown);

    printf("\n[parse_phdr_flags]\n");
    RUN_TEST(parse_phdr_flags_read_only);
    RUN_TEST(parse_phdr_flags_read_write);
    RUN_TEST(parse_phdr_flags_read_exec);
    RUN_TEST(parse_phdr_flags_all);
    RUN_TEST(parse_phdr_flags_none);

    printf("\n[read_elf_header]\n");
    RUN_TEST(read_elf_header_null_fp);
    RUN_TEST(read_elf_header_null_ehdr);
    RUN_TEST(read_elf_header_empty_file);
    RUN_TEST(read_elf_header_valid);

    printf("\n[read_program_header]\n");
    RUN_TEST(read_program_header_null_fp);
    RUN_TEST(read_program_header_null_phdr);
    RUN_TEST(read_program_header_valid);

    printf("\n[Integration]\n");
    RUN_TEST(integration_full_elf_parse);

    printf("\n========================================\n");
    printf("Results: %s%d passed%s, %s%d failed%s\n",
           GREEN, tests_passed, RESET,
           tests_failed > 0 ? RED : GREEN, tests_failed, RESET);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
