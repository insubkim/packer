/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/12/11 23:30:23 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_UTILS_H
# define PRINT_UTILS_H
# define ERRNO_TRUE 1
# define ERRNO_FALSE 0  

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef enum e_error
{
    WRONG_ARGS,
    FILE_NOT_FOUND,
    INVALID_ELF,
    MEMORY_ALLOCATION_FAILED,
    PLACEHOLDER_FIND_FAILED,
    PADDING_WRITE_ERROR,
    PATCH_HEADER_WRITE_ERROR,
    STUB_WRITE_ERROR
} error_t;

int print_error(error_t error, int use_errno);
int print_debug(const char *format, ...);

#endif