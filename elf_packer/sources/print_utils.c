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

#include "print_utils.h"

inline int print_error(error_t error, int use_errno)
{
    if (use_errno)
        perror("Error");
    else
    {
        if (error == WRONG_ARGS)
            fprintf(stderr, "Usage: elf_packer <input_elf_file>\n");
        else if (error == FILE_NOT_FOUND)
            fprintf(stderr, "Error: File not found.\n");
        else if (error == INVALID_ELF)
            fprintf(stderr, "Error: Invalid ELF file.\n");
        // Add more error messages as needed
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