#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

static int tests_passed = 0;
static int tests_failed = 0;

static void test_pass(const char *name) {
    printf("[%sPASS%s] %s\n", GREEN, RESET, name);
    tests_passed++;
}

static void test_fail(const char *name, const char *reason) {
    printf("[%sFAIL%s] %s: %s\n", RED, RESET, name, reason);
    tests_failed++;
}

/* Run elf_viewer and capture exit code */
static int run_viewer(const char *filepath) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "./elf_viewer %s > /dev/null 2>&1", filepath);
    int status = system(cmd);
    return WEXITSTATUS(status);
}

/* Run elf_viewer and capture output */
static int run_viewer_with_output(const char *filepath, char *output, size_t output_size) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "./elf_viewer %s 2>&1", filepath);

    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;

    size_t total = 0;
    while (total < output_size - 1) {
        size_t n = fread(output + total, 1, output_size - 1 - total, fp);
        if (n == 0) break;
        total += n;
    }
    output[total] = '\0';

    return pclose(fp);
}

/* Create a minimal valid ELF64 file for testing */
static int create_minimal_elf(const char *path) {
    unsigned char elf[] = {
        // ELF Header (64 bytes)
        0x7f, 'E', 'L', 'F',           // Magic
        2,                              // Class: 64-bit
        1,                              // Data: Little Endian
        1,                              // Version
        0,                              // OS/ABI
        0, 0, 0, 0, 0, 0, 0, 0,        // Padding
        2, 0,                           // Type: ET_EXEC
        0x3e, 0,                        // Machine: x86-64
        1, 0, 0, 0,                     // Version
        0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // Entry point
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Program header offset
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Section header offset
        0, 0, 0, 0,                     // Flags
        64, 0,                          // ELF header size
        56, 0,                          // Program header entry size
        1, 0,                           // Program header count
        64, 0,                          // Section header entry size
        0, 0,                           // Section header count
        0, 0,                           // Section header string index

        // Program Header (56 bytes) - PT_LOAD
        1, 0, 0, 0,                     // Type: PT_LOAD
        5, 0, 0, 0,                     // Flags: PF_R | PF_X
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Offset
        0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // Virtual address
        0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // Physical address
        0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // File size
        0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Memory size
        0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Alignment
    };

    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fwrite(elf, 1, sizeof(elf), fp);
    fclose(fp);
    return 0;
}

/* Create an invalid file (not ELF) */
static int create_non_elf(const char *path) {
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "This is not an ELF file\n");
    fclose(fp);
    return 0;
}

/* Create truncated ELF (only magic) */
static int create_truncated_elf(const char *path) {
    unsigned char data[] = { 0x7f, 'E', 'L', 'F', 2, 1, 1, 0 };
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fwrite(data, 1, sizeof(data), fp);
    fclose(fp);
    return 0;
}

/*=== Test Cases ===*/

void test_no_arguments(void) {
    int status = system("./elf_viewer > /dev/null 2>&1");
    if (WEXITSTATUS(status) == 1) {
        test_pass("No arguments returns error");
    } else {
        test_fail("No arguments returns error", "Expected exit code 1");
    }
}

void test_nonexistent_file(void) {
    int ret = run_viewer("/nonexistent/file/path");
    if (ret == 1) {
        test_pass("Nonexistent file returns error");
    } else {
        test_fail("Nonexistent file returns error", "Expected exit code 1");
    }
}

void test_non_elf_file(void) {
    const char *path = "/tmp/test_non_elf.bin";
    create_non_elf(path);

    char output[4096];
    run_viewer_with_output(path, output, sizeof(output));

    if (strstr(output, "Not an ELF file")) {
        test_pass("Non-ELF file detected");
    } else {
        test_fail("Non-ELF file detected", "Expected 'Not an ELF file' message");
    }

    unlink(path);
}

void test_truncated_elf(void) {
    const char *path = "/tmp/test_truncated.elf";
    create_truncated_elf(path);

    int ret = run_viewer(path);
    if (ret != 0) {
        test_pass("Truncated ELF returns error");
    } else {
        test_fail("Truncated ELF returns error", "Expected non-zero exit");
    }

    unlink(path);
}

void test_valid_elf_magic(void) {
    const char *path = "/tmp/test_valid.elf";
    create_minimal_elf(path);

    char output[8192];
    int ret = run_viewer_with_output(path, output, sizeof(output));

    if (strstr(output, "Magic: 7f 45 4c 46")) {
        test_pass("Valid ELF magic parsed");
    } else {
        test_fail("Valid ELF magic parsed", "Magic not found in output");
    }

    unlink(path);
}

void test_elf_class(void) {
    const char *path = "/tmp/test_valid.elf";
    create_minimal_elf(path);

    char output[8192];
    run_viewer_with_output(path, output, sizeof(output));

    if (strstr(output, "Class: ELF64")) {
        test_pass("ELF64 class detected");
    } else {
        test_fail("ELF64 class detected", "Class not correctly identified");
    }

    unlink(path);
}

void test_elf_endianness(void) {
    const char *path = "/tmp/test_valid.elf";
    create_minimal_elf(path);

    char output[8192];
    run_viewer_with_output(path, output, sizeof(output));

    if (strstr(output, "Little Endian")) {
        test_pass("Little Endian detected");
    } else {
        test_fail("Little Endian detected", "Endianness not correctly identified");
    }

    unlink(path);
}

void test_elf_type(void) {
    const char *path = "/tmp/test_valid.elf";
    create_minimal_elf(path);

    char output[8192];
    run_viewer_with_output(path, output, sizeof(output));

    if (strstr(output, "ET_EXEC")) {
        test_pass("ELF type ET_EXEC detected");
    } else {
        test_fail("ELF type ET_EXEC detected", "Type not correctly identified");
    }

    unlink(path);
}

void test_program_header(void) {
    const char *path = "/tmp/test_valid.elf";
    create_minimal_elf(path);

    char output[8192];
    run_viewer_with_output(path, output, sizeof(output));

    if (strstr(output, "PT_LOAD")) {
        test_pass("Program header PT_LOAD detected");
    } else {
        test_fail("Program header PT_LOAD detected", "PT_LOAD not found");
    }

    unlink(path);
}

void test_program_header_flags(void) {
    const char *path = "/tmp/test_valid.elf";
    create_minimal_elf(path);

    char output[8192];
    run_viewer_with_output(path, output, sizeof(output));

    // Flags should be R E (read + execute)
    if (strstr(output, "R") && strstr(output, "E")) {
        test_pass("Program header flags parsed");
    } else {
        test_fail("Program header flags parsed", "Flags not correctly identified");
    }

    unlink(path);
}

void test_real_elf_file(void) {
    // Test with the actual hello_world binary if it exists
    if (access("./hello_world", F_OK) == 0) {
        char output[16384];
        int ret = run_viewer_with_output("./hello_world", output, sizeof(output));

        if (ret == 0 && strstr(output, "ELF Header:")) {
            test_pass("Real ELF file (hello_world) parsed");
        } else {
            test_fail("Real ELF file (hello_world) parsed", "Failed to parse");
        }
    } else {
        printf("[SKIP] Real ELF file test: hello_world not found\n");
    }
}

void test_self_binary(void) {
    // Test with the viewer itself
    char output[16384];
    int ret = run_viewer_with_output("./elf_viewer", output, sizeof(output));

    if (ret == 0 && strstr(output, "ELF Header:")) {
        test_pass("Self binary (elf_viewer) parsed");
    } else {
        test_fail("Self binary (elf_viewer) parsed", "Failed to parse self");
    }
}

int main(void) {
    printf("========================================\n");
    printf("   ELF Header Viewer Test Suite\n");
    printf("========================================\n\n");

    // Check if elf_viewer exists
    if (access("./elf_viewer", X_OK) != 0) {
        fprintf(stderr, "Error: ./elf_viewer not found or not executable.\n");
        fprintf(stderr, "Please compile first: gcc -o elf_viewer elf-header-viwer.c\n");
        return 1;
    }

    printf("--- Error Handling Tests ---\n");
    test_no_arguments();
    test_nonexistent_file();
    test_non_elf_file();
    test_truncated_elf();

    printf("\n--- ELF Header Parsing Tests ---\n");
    test_valid_elf_magic();
    test_elf_class();
    test_elf_endianness();
    test_elf_type();

    printf("\n--- Program Header Tests ---\n");
    test_program_header();
    test_program_header_flags();

    printf("\n--- Integration Tests ---\n");
    test_real_elf_file();
    test_self_binary();

    printf("\n========================================\n");
    printf("Results: %s%d passed%s, %s%d failed%s\n",
           GREEN, tests_passed, RESET,
           tests_failed > 0 ? RED : GREEN, tests_failed, RESET);
    printf("========================================\n");

    return tests_failed > 0 ? 1 : 0;
}
