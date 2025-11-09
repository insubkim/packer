section .text
global _start
_start:
	call callee
    db 'inskim',0x0a
callee:
	pop rsi
	push rax
	push rbx
	push rcx
	push rdx
	mov rdx, 7 ;mov rdx, len
	mov rdi, 1
	mov rax, 1
	syscall
	pop rdx
	pop rcx
	pop rbx
	pop rax
	mov rax, 0x400440
	jmp rax 
