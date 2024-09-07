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
