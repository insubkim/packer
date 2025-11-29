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

#include "file.h"
#include "print_utils.h"

size_t read_file(const char *filename, char **file_buffer)
{
    assert(filename != NULL);
    // open file
    int fd = open(filename, O_RDWR);
    if (fd < 0) 
        return print_error(FILE_NOT_FOUND, ERRNO_TRUE) & 0x0;
    
    // get file size
    size_t file_size = lseek(fd, 0, SEEK_END);
    print_debug("file [%s] size : %llu\n", filename, (unsigned long long)file_size);

    *file_buffer = malloc(file_size);
    if (!*file_buffer)
        return print_error(MEMORY_ALLOCATION_FAILED, ERRNO_FALSE) & 0x0;

    // read file
    ssize_t read_bytes = read(fd, *file_buffer, file_size);
    if (read_bytes != file_size)
    {
        free(*file_buffer);
        return print_error(FILE_NOT_FOUND, ERRNO_TRUE) & 0x0;
    }

    close(fd);
    return file_size;
}