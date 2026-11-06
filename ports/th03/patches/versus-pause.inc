versus_pause:
    pushad
    push es
    mov byte [cs:menu_choice],0
    mov byte [cs:menu_result],0
    mov byte [cs:menu_action],0
    xor al,al
    out 0x7c,al
    mov al,[0x658f]
    out 0xa6,al
    mov word [cs:menu_left],2
    cmp byte [cs:menu_seat],0
    je .positioned
    mov word [cs:menu_left],42
.positioned:
    call versus_save
    call versus_draw
    mov byte [cs:menu_state],2
.wait:
    inc dword [cs:ticks]
    push word 1
    call far [cs:0x3103]
    mov al,[cs:menu_action]
    mov byte [cs:menu_action],0
    cmp al,1
    je .up
    cmp al,2
    je .down
    cmp al,3
    je .confirm
    cmp al,4
    je .resume
    cmp al,5
    je .round
    cmp al,6
    je .match
    jmp .wait
.up:
    cmp byte [cs:menu_allowed],0
    je .wait
    cmp byte [cs:menu_choice],0
    jne .decrement
    mov byte [cs:menu_choice],3
.decrement:
    dec byte [cs:menu_choice]
    call versus_draw
    jmp .wait
.down:
    cmp byte [cs:menu_allowed],0
    je .wait
    inc byte [cs:menu_choice]
    cmp byte [cs:menu_choice],3
    jb .redraw
    mov byte [cs:menu_choice],0
.redraw:
    call versus_draw
    jmp .wait
.confirm:
    mov al,[cs:menu_choice]
    inc al
    jmp .done
.resume:
    mov al,1
    jmp .done
.round:
    cmp byte [cs:menu_allowed],0
    je .wait
    mov al,2
    jmp .done
.match:
    cmp byte [cs:menu_allowed],0
    je .wait
    mov al,3
.done:
    mov [cs:menu_result],al
    call versus_restore
    mov al,[0x658e]
    out 0xa6,al
    mov byte [cs:menu_state],3
    pop es
    popad
    ret
versus_save:
    xor bx,bx
    xor si,si
.plane:
    mov es,[cs:versus_planes+si]
    mov di,48*80
    add di,[cs:menu_left]
    mov dx,72
.row:
    mov cx,36
.pixel:
    mov al,[es:di]
    mov [cs:versus_background+bx],al
    inc bx
    inc di
    loop .pixel
    add di,44
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    ret
versus_restore:
    xor bx,bx
    xor si,si
.plane:
    mov es,[cs:versus_planes+si]
    mov di,48*80
    add di,[cs:menu_left]
    mov dx,72
.row:
    mov cx,36
.pixel:
    mov al,[cs:versus_background+bx]
    mov [es:di],al
    inc bx
    inc di
    loop .pixel
    add di,44
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    ret
versus_draw:
    xor si,si
.plane:
    mov es,[cs:versus_planes+si]
    mov byte [cs:menu_row],0
    mov di,50*80
    add di,[cs:menu_left]
.button:
    mov bp,di
    xor ax,ax
    mov al,[cs:menu_row]
    imul bx,ax,720
    add bx,versus_glyphs
    cmp byte [cs:menu_language],0
    je .font_ready
    add bx,versus_glyphs_jp-versus_glyphs
.font_ready:
    mov dx,20
.row:
    mov cx,36
.pixel:
    mov al,[cs:menu_row]
    cmp al,[cs:menu_choice]
    jne .unselected
    mov al,[cs:bx]
    not al
    jmp .write
.unselected:
    xor al,al
    cmp si,0
    je .blue
    cmp si,6
    jne .glyph
.blue:
    mov al,0xff
.glyph:
    or al,[cs:bx]
.write:
    mov [es:di],al
    inc bx
    inc di
    loop .pixel
    add di,44
    dec dx
    jnz .row
    mov di,bp
    add di,23*80
    inc byte [cs:menu_row]
    cmp byte [cs:menu_row],3
    jb .button
    add si,2
    cmp si,8
    jb .plane
    ret
menu_left: dw 0
menu_row: db 0
versus_planes: dw 0xa800,0xb000,0xb800,0xe000
%include "patches/versus-pause-font.inc"
versus_background: times (36*72*4) db 0
