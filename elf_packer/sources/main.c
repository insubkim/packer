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
#include "file.h"
#include "print_utils.h"
#include "elf_parser.h"
#include "stub.h"

int main(int argc, char *argv[])
{
    int exit_code = 0;

    if (argc != 2) 
        return print_error(WRONG_ARGS, ERRNO_FALSE);
    
    // read file
    char *file_buffer = NULL;
    size_t size = read_file(argv[1], &file_buffer);
    if (size == 0)
        return -1;

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