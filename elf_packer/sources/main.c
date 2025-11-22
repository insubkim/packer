/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/11/17 21:07:58 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int main(int argc, char *argv[])
{
    int exit_code = 0;

    if (argc != 2) 
        return print_error(WRONG_ARGS, ERRNO_FALSE);
    
    // open file
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) 
        return print_error(FILE_NOT_FOUND, ERRNO_TRUE);
    
    size_t file_size = lseek(fd, 0, SEEK_END);
    assert(file_size != (size_t)-1);
    print_debug("file [%s] size : %llu\n", argv[1], (unsigned long long)file_size);

    // read file
    lseek(fd, 0, SEEK_SET);
    char *file_buffer = malloc(file_size);
    assert(file_buffer != NULL);
    ssize_t read_bytes = read(fd, file_buffer, file_size);
    assert(read_bytes == (ssize_t)file_size);
    close(fd);

    // parse elf
    t_elf elf = parse_elf(file_buffer);

    // check ep, check pt_note
    if (elf.ehdr->e_entry == FALSE || check_pt_note(elf) == FALSE)
    {
        exit_code = print_error(INVALID_ELF, ERRNO_TRUE);
        goto cleanup;
    }

    // create stub
    // set original ep to end of stub jmp point
    // set stub info to pt_note
    // add padding
    // add stub
    // ( for stub : stub will get count, offset, size to decypt original text section )
    // change ep to stub entry
    // write packed exe

cleanup:
    free(file_buffer);

    return exit_code;
}