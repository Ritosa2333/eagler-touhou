; PC-98 text-plane primitive used by the guest practice menu.
; ES:DI = first character cell. The attribute
; plane is 0x2000 bytes after the character plane. CX = cell count.
; Saves both planes into CS:practice_text_saved; the matching restore uses
; the same ES:DI and CX. The caller must stay within the 240-cell buffer.
practice_text_save:
    push ax
    push bx
    push cx
    push di
    push si
    mov si, practice_text_saved
.loop:
    jcxz .done
    mov ax, [es:di]
    mov [cs:si], ax
    mov ax, [es:di+0x2000]
    mov [cs:si+2], ax
    add si, 4
    add di, 2
    dec cx
    jmp .loop
.done:
    pop si
    pop di
    pop cx
    pop bx
    pop ax
    ret

practice_text_restore:
    push ax
    push cx
    push di
    push si
    mov si, practice_text_saved
.loop:
    jcxz .done
    mov ax, [cs:si]
    mov [es:di], ax
    mov ax, [cs:si+2]
    mov [es:di+0x2000], ax
    add si, 4
    add di, 2
    dec cx
    jmp .loop
.done:
    pop si
    pop di
    pop cx
    pop ax
    ret

; DS:SI points to a zero-terminated ASCII string, ES:DI to its first cell,
; BL is the PC-98 text attribute. Only single-byte glyphs are accepted.
practice_text_write:
    pushf
    push ax
    push di
    push si
    cld
.loop:
    lodsb
    test al, al
    jz .done
    xor ah, ah
    mov [es:di], ax
    mov al, bl
    mov [es:di+0x2000], ax
    add di, 2
    jmp .loop
.done:
    pop si
    pop di
    pop ax
    popf
    ret

practice_text_saved: times 240*4 db 0
