// tiny_loader.c
// Build: gcc -Wall -O2 tiny_loader.c -o tiny_loader
// Usage: ./tiny_loader /path/to/simple_binary
//
// Limitations:
// - Works for simple statically linked ET_EXEC or PIE (ET_DYN) that don't require interpreter or shared-libs.
// - Does NOT implement dynamic linker responsibilities (DT_*), TLS, relocations, auxv stack, vdso, setuid handling, etc.
// - Running arbitrary binaries this way can be dangerous.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <elf.h>
#include <inttypes.h>

static size_t page_size;
static inline size_t page_down(size_t x){ return x & ~(page_size-1); }
static inline size_t page_up(size_t x){ return (x + page_size - 1) & ~(page_size-1); }

int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return 2;
    }

    page_size = sysconf(_SC_PAGESIZE);

    const char *path = argv[1];
    int fd = open(path, O_RDONLY);
    if(fd < 0){ perror("open"); return 3; }

    struct stat st;
    if(fstat(fd, &st) < 0){ perror("fstat"); close(fd); return 4; }

    // Read ELF header
    Elf64_Ehdr eh;
    if(read(fd, &eh, sizeof(eh)) != sizeof(eh)){ perror("read ehdr"); close(fd); return 5; }

    if(memcmp(eh.e_ident, ELFMAG, SELFMAG) != 0){
        fprintf(stderr,"Not an ELF file\n"); close(fd); return 6;
    }

    if(eh.e_ident[EI_CLASS] != ELFCLASS64){
        fprintf(stderr,"This loader only supports 64-bit ELF\n"); close(fd); return 7;
    }

    // Read program headers
    Elf64_Phdr *ph = malloc(eh.e_phnum * sizeof(Elf64_Phdr));
    if(!ph){ perror("malloc"); close(fd); return 8; }

    if(lseek(fd, eh.e_phoff, SEEK_SET) < 0){ perror("lseek phoff"); close(fd); return 9; }
    if(read(fd, ph, eh.e_phnum * sizeof(Elf64_Phdr)) != (ssize_t)(eh.e_phnum * sizeof(Elf64_Phdr))){
        perror("read phdrs"); close(fd); return 10;
    }

    // Compute load area for ET_DYN (PIE) or check for ET_EXEC addresses
    uintptr_t base = 0;
    int is_pie = (eh.e_type == ET_DYN);
    uintptr_t min_vaddr = (uintptr_t)-1, max_vaddr = 0;

    for(int i=0;i<eh.e_phnum;i++){
        if(ph[i].p_type != PT_LOAD) continue;
        if(ph[i].p_vaddr < min_vaddr) min_vaddr = ph[i].p_vaddr;
        if(ph[i].p_vaddr + ph[i].p_memsz > max_vaddr) max_vaddr = ph[i].p_vaddr + ph[i].p_memsz;
    }

    if(min_vaddr == (uintptr_t)-1){
        fprintf(stderr,"No PT_LOAD segments\n"); close(fd); return 11;
    }

    size_t total_mem = page_up(max_vaddr - page_down(min_vaddr));

    if(is_pie){
        // Reserve an address region for the image (let kernel pick address)
        void *res = mmap(NULL, total_mem + page_size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if(res == MAP_FAILED){ perror("mmap reserve"); close(fd); return 12; }
        // choose base so that min_vaddr maps to res + (page_down(min_vaddr) offset)
        base = (uintptr_t)res - page_down(min_vaddr);
    } else {
        // ET_EXEC: keep base as 0 and map at absolute p_vaddr (may require privileges)
        base = 0;
    }

    // Map each PT_LOAD
    for(int i=0;i<eh.e_phnum;i++){
        if(ph[i].p_type != PT_LOAD) continue;

        off_t file_offset = page_down(ph[i].p_offset);
        uintptr_t vaddr_page = page_down(ph[i].p_vaddr);
        size_t offset_in_page = ph[i].p_vaddr - vaddr_page;
        size_t filesz = ph[i].p_filesz + (ph[i].p_vaddr - vaddr_page);
        size_t memsz = ph[i].p_memsz + (ph[i].p_vaddr - vaddr_page);

        void *map_addr = (void *)(base + vaddr_page);
        size_t map_len = page_up(filesz);

        int prot = 0;
        if(ph[i].p_flags & PF_R) prot |= PROT_READ;
        if(ph[i].p_flags & PF_W) prot |= PROT_WRITE;
        if(ph[i].p_flags & PF_X) prot |= PROT_EXEC;

        // Map file-backed portion (filesz)
        void *m = mmap(map_addr, map_len, prot, MAP_PRIVATE | (is_pie ? MAP_FIXED : MAP_FIXED), fd, file_offset);
        if(m == MAP_FAILED){
            fprintf(stderr,"mmap segment failed: offset=%"PRIu64" vaddr=0x%jx len=%zu prot=%d errno=%d %s\n",
                    (uint64_t)file_offset, (uintmax_t)vaddr_page, map_len, prot, errno, strerror(errno));
            close(fd); return 13;
        }

        // If memsz > filesz -> need to ensure extra (BSS) is zeroed and mapped
        if(memsz > filesz){
            uintptr_t bss_start = base + vaddr_page + page_up(filesz);
            size_t bss_len = page_up(memsz) - page_up(filesz);

            if(bss_len > 0){
                void *b = mmap((void*)bss_start, bss_len, prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
                if(b == MAP_FAILED){
                    perror("mmap bss");
                    close(fd); return 14;
                }
            }
        }

        // Ensure final protections (in case file mapped with RW and exec needs to be removed)
        // some systems require mprotect to set PROT_EXEC after mapping.
        uintptr_t seg_start = base + vaddr_page;
        size_t seg_len = page_up(memsz);
        if(mprotect((void*)seg_start, seg_len, prot) < 0){
            perror("mprotect");
            // not fatal - continue
        }
    }

    // Close file before jumping
    close(fd);

    // Calculate entry point
    uintptr_t entry = (uintptr_t)eh.e_entry + base;
    printf("Loaded image at base=0x%jx entry=0x%jx\n", (uintmax_t)base, (uintmax_t)entry);

    // Flush instruction cache on some archs? On x86 it's fine.
    // Cast entry to function and call. Be careful: the target expects proper stack/auxv.
    void (*entry_fn)(void) = (void(*)(void))entry;

    // Optionally drop privileges, set up argv/envp/auxv if you want to approximate execve.
    // For demo, just jump.
    printf("jumping to entry point... (control may not return)\n");
    fflush(stdout);

    // Use assembly to set up a bare stack? We'll just call — many binaries expect argc/argv/auxv and will crash.
    entry_fn();

    // If it returns:
    fprintf(stderr, "child returned\n");
    return 0;
}


