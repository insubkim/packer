[BITS 64]

global _start

_start:
    ; 1. 레지스터 저장
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11

    ; 2. "inskim" 출력
    mov rax, 1                  ; sys_write
    mov rdi, 1                  ; fd = 1
    lea rsi, [rel msg]          ; RIP-relative address
    mov rdx, 7                  ; length
    syscall

    ; 3. 레지스터 복구
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi

    ; 4. 원본 OEP로 점프
    mov rax, 0x1122334455667788 ; Placeholder
    jmp rax

    ; 데이터 (코드 바로 뒤에 밀착)
msg: db "inskim", 0x0a