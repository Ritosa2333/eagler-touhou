; Draw the compact trainer into the guest text plane. The text under the
; three rows is saved once and restored on close. Caller preserves registers.
practice_assists_draw:
    push ds
    push es
    push cs
    pop ds
    mov ax, 0xa000
    mov es, ax
    xor di, di
    cmp byte [cs:practice_assists_open], 0
    je .close
    cmp byte [cs:practice_assists_drawn], 0
    jne .labels
    mov cx, 240
    call practice_text_save
    mov byte [cs:practice_assists_drawn], 1
.labels:
    mov bl, 0xe1
    mov di, 100
    mov si, practice_assists_line1
    call practice_text_write
    mov di, 260
    mov si, practice_assists_line2
    call practice_text_write
    mov di, 420
    mov si, practice_assists_line3
    call practice_text_write
    ; Positions are cell offsets of the six single-digit toggle values.
    mov bx, [cs:practice_assists]
    mov di, practice_assists_positions
    mov cx, 6
.values:
    mov si, [cs:di]
    mov ax, '0'
    test bl, 1
    jz .put
    inc ax
.put:
    mov [es:si], ax
    mov word [es:si+0x2000], 0xe1
    shr bl, 1
    add di, 2
    loop .values
    jmp .done
.close:
    cmp byte [cs:practice_assists_drawn], 0
    je .done
    mov cx, 240
    call practice_text_restore
    mov byte [cs:practice_assists_drawn], 0
.done:
    pop es
    pop ds
    ret
practice_assists_drawn: db 0
practice_assists_line1: db 'F1 HIT: 0     F2 LIVES: 0',0
practice_assists_line2: db 'F3 BOMB: 0    F4 POWER: 0',0
practice_assists_line3: db 'F5 TIME: 0    U ENEMY: 0',0
practice_assists_positions: dw 100+8*2,100+24*2,260+9*2,260+24*2,420+9*2,420+23*2
