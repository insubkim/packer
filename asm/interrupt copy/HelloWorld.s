section .text
global _start
	msg db 'Hello World','$';
	len equ $ -msg
_start:
	mov edx, len
	mov ecx, msg
	mov ebx, 1
	mov eax, 4
	int 0x80
	mov ebx,7
	mov eax,1
	int 0x80
