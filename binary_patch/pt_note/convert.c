#include <elf.h>
#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

const static char *infile = "./hello_world_inskim";
const static char *outfile = "./hello_world_inskim_converted";

const static int added_file_size = 0xad;
const static int added_file_offset = 20480;
const static int added_file_addr = 0x800000;

int main()
{
    int fd = open(infile, O_RDONLY);
    
    size_t file_size = lseek(fd, 0, SEEK_END);
    printf("file size : %llu", file_size);

    size_t mapped_size = (file_size + 0xfff) & ~(0xfff);
    printf("mapped size : %llu", mapped_size);

    void *p = mmap(NULL, mapped_size, PROT_READ | PROT_WRITE, 
                    MAP_PRIVATE, fd, 0);
    
    Elf64_Ehdr *ehdr = p;
    ehdr->e_entry=added_file_addr;

    for (size_t pi = 0; pi < ehdr->e_phnum; pi++)
    {
        Elf64_Phdr *phdr = ((Elf64_Phdr *)(p + ehdr->e_phoff)) + pi;
        if (phdr->p_type == PT_NOTE)
        {
            phdr->p_align = 0x200000;
            phdr->p_filesz = added_file_size;
            phdr->p_memsz = added_file_size;
            phdr->p_offset = added_file_offset;
            phdr->p_type = PT_LOAD;
            phdr->p_flags = PF_X | PF_R;
            phdr->p_vaddr = added_file_addr;
            phdr->p_paddr = added_file_addr;
            break;
        }
    }

    FILE *fp = fopen(outfile, "wb");
    fwrite(p, file_size, 1, fp);
    return 0;
}