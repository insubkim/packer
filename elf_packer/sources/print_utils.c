/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/12/11 23:30:39 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "print_utils.h"

inline int print_error(error_t error, int use_errno)
{
    if (use_errno)
        perror("Error");
    switch (error)
    {
    case WRONG_ARGS:
            fprintf(stderr, "Usage: elf_packer <input_elf_file>\n");
            break;
    case FILE_NOT_FOUND:
            fprintf(stderr, "Error: File not found.\n");
            break;
    case INVALID_ELF:
            fprintf(stderr, "Error: Invalid ELF file.\n");
            break;
    case MEMORY_ALLOCATION_FAILED:
            fprintf(stderr, "Error: Memory allocation failed.\n");
            break;
    case PLACEHOLDER_FIND_FAILED:
            fprintf(stderr, "Error: Failed to find placeholder in stub.\n");
            break;
    case PADDING_WRITE_ERROR:
            fprintf(stderr, "Error: Failed to write padding to output file.\n");
            break;
    case PATCH_HEADER_WRITE_ERROR:
            fprintf(stderr, "Error: Failed to write patched header to output file.\n");
            break;
    case STUB_WRITE_ERROR:
            fprintf(stderr, "Error: Failed to write stub to output file.\n");
            break;
    default:
        assert(0 && "Unknown error type");        
    }
    return -1;
}

inline int print_debug(const char *format, ...)
{
    if (!DEBUG)
        return 0;
    
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}