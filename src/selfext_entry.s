/*
 * selfext_entry.s - Entry point for self extracting archive.
 *
 * This file is part of the UAR project.
 * Copyright (C) 2024  OSN Developers.
 * Author:  Ar Rakin <rakinar2@onesoftnet.eu.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

.section .data

.globl __uar_data_start
.globl __uar_data_end
.globl __uar_data_size

__uar_data_start:
    .incbin "../test.uar"
__uar_data_end:
    .byte 0x00
__uar_data_size:
    .quad __uar_data_end - __uar_data_start

.text

.globl uar_get_data_start
.type uar_get_data_start, @function

uar_get_data_start:
    lea __uar_data_start(%rip), %rax
    ret

.section .note.GNU-stack,"",@progbits
