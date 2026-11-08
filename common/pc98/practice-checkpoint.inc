; A same-MAIN checkpoint for selected Boss attacks. The stack is above the
; saved data range. DOS owns the buffer and frees it when MAIN exits.
%ifdef PRACTICE_RESTART_TH02
%define PRACTICE_RESIDENT_PTR 0x27fc
%define PRACTICE_RESIDENT_SIZE 48
%else
%define PRACTICE_RESIDENT_PTR 0xba86
%define PRACTICE_RESIDENT_SIZE 74
%endif
%if PRACTICE_CHECKPOINT_BYTES <= 0 || PRACTICE_CHECKPOINT_BYTES + PRACTICE_RESIDENT_SIZE >= 65536
%error Invalid MAIN data checkpoint range
%endif

practice_checkpoint_save:
    ; A restored attempt reaches this hook again. Keep the original entry,
    ; rather than drifting forward by a frame on every subsequent retry.
    cmp byte [cs:practice_checkpoint_valid],0
    jne .done
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:practice_initial_entry],2
    jne .done
    pushf
    pushad
    push ds
    push es
    cld
    mov byte [cs:practice_checkpoint_valid],0
    cmp word [cs:practice_checkpoint_buffer],0
    jne .allocated
    mov bx,(PRACTICE_CHECKPOINT_BYTES+PRACTICE_RESIDENT_SIZE+15)/16
    mov ah,0x48
    int 0x21
    jc .exit
    mov [cs:practice_checkpoint_buffer],ax
.allocated:
    mov es,[cs:practice_checkpoint_buffer]
    xor si,si
    xor di,di
    mov cx,PRACTICE_CHECKPOINT_BYTES
    rep movsb
    les bx,[PRACTICE_RESIDENT_PTR]
    mov ax,es
    mov ds,ax
    mov si,bx
    mov es,[cs:practice_checkpoint_buffer]
    mov di,PRACTICE_CHECKPOINT_BYTES
    mov cx,PRACTICE_RESIDENT_SIZE
    rep movsb
    mov byte [cs:practice_checkpoint_valid],1
.exit:
    pop es
    pop ds
    popad
    popf
.done:
    ret

practice_checkpoint_restore:
    pushf
    pushad
    push ds
    push es
    cld
    push ds
    pop es
    mov ds,[cs:practice_checkpoint_buffer]
    xor si,si
    xor di,di
    mov cx,PRACTICE_CHECKPOINT_BYTES
    rep movsb
    push es
    pop ds
    les bx,[PRACTICE_RESIDENT_PTR]
    mov di,bx
    mov ds,[cs:practice_checkpoint_buffer]
    mov si,PRACTICE_CHECKPOINT_BYTES
    mov cx,PRACTICE_RESIDENT_SIZE
    rep movsb
    pop es
    pop ds
    popad
    popf
    ret

practice_checkpoint_buffer: dw 0
practice_checkpoint_valid: db 0
practice_checkpoint_requested: db 0
