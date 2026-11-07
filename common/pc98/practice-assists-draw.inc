; Six native text rows at the upper left, matching uth05's compact list.
; Save only the cells occupied by the menu, not unrelated game text.
%define ASSIST_XY (160+16)
%define ASSIST_WIDTH 20
practice_assists_draw:
    push ds
    push es
    push cs
    pop ds
    mov ax, 0xa000
    mov es, ax
    cmp byte [cs:practice_assists_open], 0
    je .close
    cmp byte [cs:practice_assists_drawn], 0
    jne .labels
    mov si, practice_text_saved
    mov di, ASSIST_XY
    mov dx, 6
.save_row:
    mov cx, ASSIST_WIDTH
.save_cell:
    mov ax, [es:di]
    mov [cs:si], ax
    mov ax, [es:di+0x2000]
    mov [cs:si+2], ax
    add si, 4
    add di, 2
    loop .save_cell
    add di, 160-ASSIST_WIDTH*2
    dec dx
    jnz .save_row
    mov byte [cs:practice_assists_drawn], 1
.labels:
    mov si, practice_assists_labels
    mov di, ASSIST_XY
    mov dl, [cs:practice_assists]
    mov dh, 6
.row:
    mov bx, 0xe1                 ; white; enabled entries are green
    test dl, 1
    jz .write
    mov bl, 0x81
.write:
    mov cx, ASSIST_WIDTH
.cell:
    lodsb
    xor ah, ah
    mov [es:di], ax
    mov [es:di+0x2000], bx
    add di, 2
    loop .cell
    test dl, 1
    jz .next
    mov word [es:di-ASSIST_WIDTH*2], '['
    mov bx, di
    sub bx, 4
.trim:
    cmp word [es:bx], ' '
    jne .bracket
    sub bx, 2
    jmp .trim
.bracket:
    mov word [es:bx+2], ']'
.next:
    add di, 160-ASSIST_WIDTH*2
    shr dl, 1
    dec dh
    jnz .row
    jmp .done
.close:
    cmp byte [cs:practice_assists_drawn], 0
    je .done
    mov si, practice_text_saved
    mov di, ASSIST_XY
    mov dx, 6
.restore_row:
    mov cx, ASSIST_WIDTH
.restore_cell:
    mov ax, [cs:si]
    mov [es:di], ax
    mov ax, [cs:si+2]
    mov [es:di+0x2000], ax
    add si, 4
    add di, 2
    loop .restore_cell
    add di, 160-ASSIST_WIDTH*2
    dec dx
    jnz .restore_row
    mov byte [cs:practice_assists_drawn], 0
.done:
    pop es
    pop ds
    ret
practice_assists_drawn: db 0
practice_assists_labels:
    db ' F1: INVINCIBLE     '
    db ' F2: LOCK LIVES     '
    db ' F3: LOCK BOMB      '
    db ' F4: LOCK POWER     '
    db ' F5: LOCK TIME      '
    db ' U: ENEMY INVINC.   '
