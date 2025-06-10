section .text
global _start
_start:
	call callee
    db 'Hello World',0x0a
callee:
	pop rsi
	mov rdx, 12 ;mov rdx, len
	mov rdi, 1
	mov rax, 1
	syscall
	mov rdi, 0x0
    mov rax, 0x3c
    syscall