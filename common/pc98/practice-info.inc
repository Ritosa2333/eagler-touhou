; Native replacement information panel on the right HUD, in text VRAM.
; Host updates the six counters and visibility at the existing frame boundary.
practice_info_draw:
    push ds
    push es
    push cs
    pop ds
    mov ax,0xa000
    mov es,ax
    cmp byte [cs:practice_info_enabled],0
    jne .show
    cmp byte [cs:practice_info_drawn],0
    je .done
    mov di,11*160+52*2
    mov si,practice_info_saved
    mov dx,12
.restore_row:
    mov cx,20
.restore_cell:
    mov ax,[cs:si]
    mov [es:di],ax
    mov ax,[cs:si+2]
    mov [es:di+0x2000],ax
    add si,4
    add di,2
    loop .restore_cell
    add di,120
    dec dx
    jnz .restore_row
    mov byte [cs:practice_info_drawn],0
    jmp .done
.show:
    cmp byte [cs:practice_info_drawn],0
    jne .paint
    mov di,11*160+52*2
    mov si,practice_info_saved
    mov dx,12
.save_row:
    mov cx,20
.save_cell:
    mov ax,[es:di]
    mov [cs:si],ax
    mov ax,[es:di+0x2000]
    mov [cs:si+2],ax
    add si,4
    add di,2
    loop .save_cell
    add di,120
    dec dx
    jnz .save_row
    mov byte [cs:practice_info_drawn],1
.paint:
    mov di,11*160+52*2
    mov bp,practice_info_labels
    xor bx,bx
.row:
    push bx
    push di
    mov cx,20
.background:
    mov word [es:di],0x20
    mov word [es:di+0x2000],0x05
    add di,2
    loop .background
    pop di
    add di,2*2
    mov si,[cs:bp]
    mov bl,0xe1
    call practice_text_write
    pop bx
    cmp bx,16
    jae .next_row
    mov ax,[cs:practice_info_values+bx]
    push bx
    push di
    add di,15*2
    mov cx,5
.digit:
    xor dx,dx
    push cx
    mov cx,10
    div cx
    pop cx
    add dl,'0'
    mov [es:di],dl
    mov byte [es:di+1],0
    mov word [es:di+0x2000],0xe1
    sub di,2
    loop .digit
    pop di
    pop bx
.next_row:
    add bx,2
    add bp,2
    sub di,2*2
    add di,160
    cmp bx,24
    jb .row
.done:
    pop es
    pop ds
    ret
practice_info_enabled: db 0
practice_info_drawn: db 0
practice_info_values: times 12 dw 0
practice_info_labels: dw .lives,.stock,.miss,.bomb,.power,.rank,.time,.keys,.blank,.blank,.blank,.blank
.lives: db 'LIVES',0
.stock: db 'BOMBS',0
.miss: db 'MISS',0
.bomb: db 'BOMB',0
.power: db 'POWER',0
.rank: db 'RANK',0
.time: db 'TIME',0
.keys: db 'KEYS',0
.blank: db ' ',0
practice_info_saved: times 20*12*4 db 0
