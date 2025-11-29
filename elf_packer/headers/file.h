/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:50:57 by insub             #+#    #+#             */
/*   Updated: 2025/11/17 20:51:41 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef FILE_H
# define FILE_H

#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

size_t read_file(const char *filename, char **file_buffer);

#endif
