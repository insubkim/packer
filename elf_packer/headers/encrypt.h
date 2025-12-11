/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encrypt.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: insub <insub@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 20:51:01 by insub             #+#    #+#             */
/*   Updated: 2025/12/11 21:59:30 by insub            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENCRYPT_H
# define ENCRYPT_H

#include <stdint.h>
#include <stddef.h>

void xor_encrypt_decrypt(uint8_t *data, size_t size, uint64_t key);

#endif