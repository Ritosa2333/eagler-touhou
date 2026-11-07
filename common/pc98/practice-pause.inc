; In-game pause menu. The host supplies input and acknowledges replay I/O;
; all presentation, navigation and suspension happen in MAIN.
practice_pause:
    call practice_pause_screen_save
    call practice_pause_gfx_save
    mov byte [cs:practice_pause_open],1
    mov byte [cs:practice_pause_choice],0
    mov word [cs:practice_pause_keys],0xffff
    call practice_pause_draw
.loop:
    inc dword [cs:SOLO_MAILBOX+16]
    PRACTICE_PAUSE_WAIT
    cmp byte [cs:practice_pause_request],0
    je .input
    cmp byte [cs:practice_pause_ack],0
    je .loop
    cmp byte [cs:practice_pause_ack],1
    je .finish
    mov byte [cs:practice_pause_request],0
    mov byte [cs:practice_pause_ack],0
    mov word [cs:practice_pause_keys],0xffff
    jmp .loop
.input:
    mov dx,[cs:practice_pause_input]
    mov bx,[cs:practice_pause_keys]
    mov [cs:practice_pause_keys],dx
    not bx
    and bx,dx
    test bl,8
    jnz .finish
    test bl,1
    jz .down
    dec byte [cs:practice_pause_choice]
    and byte [cs:practice_pause_choice],3
    call practice_pause_draw
    jmp .loop
.down:
    test bl,2
    jz .confirm
    inc byte [cs:practice_pause_choice]
    and byte [cs:practice_pause_choice],3
    call practice_pause_draw
    jmp .loop
.confirm:
    test bl,4
    jz .loop
    mov al,[cs:practice_pause_choice]
    test al,al
    jz .finish
    mov [cs:practice_pause_request],al ; 1 save and quit, 2 quit, 3 retry
    jmp .loop
.finish:
    call practice_pause_gfx_restore
    call practice_pause_screen_restore
    mov byte [cs:practice_pause_open],0
    mov byte [cs:practice_pause_request],0
    mov byte [cs:practice_pause_ack],0
    ret
practice_pause_draw:
    call practice_pause_gfx_buttons
    push ds
    push cs
    pop ds
    mov ax,0xa000
    mov es,ax
    mov bl,0xe1
    mov di,8*160+15*2
    mov si,practice_pause_option_resume
    call practice_text_write
    mov di,10*160+15*2
    mov si,practice_pause_option_save
    call practice_text_write
    mov di,12*160+15*2
    mov si,practice_pause_option_exit
    call practice_text_write
    mov di,14*160+15*2
    mov si,practice_pause_option_retry
    call practice_text_write
    xor bx,bx
    mov bl,[cs:practice_pause_choice]
    shl bx,1
    mov di,[cs:practice_pause_positions+bx]
    mov cx,26
.selected:
    cmp word [cs:practice_pause_gfx],0
    je .fallback
    mov word [es:di+0x2000],0x01
    jmp .next
.fallback:
    mov word [es:di+0x2000],0xe5
.next:
    add di,2
    loop .selected
    pop ds
    ret
practice_pause_option_resume: db '          RESUME          ',0
practice_pause_option_save: db '       SAVE & EXIT        ',0
practice_pause_option_exit: db '       QUIT NO REP        ',0
practice_pause_option_retry: db '         RESTART          ',0
practice_pause_positions: dw 8*160+15*2,10*160+15*2,12*160+15*2,14*160+15*2
practice_pause_open: db 0
practice_pause_choice: db 0
practice_pause_request: db 0
practice_pause_ack: db 0
practice_pause_input: dw 0
practice_pause_keys: dw 0

; Text VRAM is fixed to the screen even when the graphics GDC scrolls.
; Save every covered cell before painting an opaque panel, so stage titles
; and sprites cannot overlap the menu and are restored exactly on resume.
practice_pause_screen_save:
    mov word [cs:practice_pause_buffer],0
    mov bx,(56*25*4+15)/16
    mov ah,0x48
    int 0x21
    jc .done
    mov [cs:practice_pause_buffer],ax
    mov fs,ax
    mov ax,0xa000
    mov es,ax
    xor di,di
    xor bx,bx
    mov dx,25
.row:
    mov cx,56
.cell:
    mov ax,[es:di]
    mov [fs:bx],ax
    mov ax,[es:di+0x2000]
    mov [fs:bx+2],ax
    mov word [es:di],0x20
    mov word [es:di+0x2000],0x00
    add bx,4
    add di,2
    loop .cell
    add di,48
    dec dx
    jnz .row
.done:
    ret
practice_pause_screen_restore:
    mov ax,[cs:practice_pause_buffer]
    test ax,ax
    jz .done
    mov fs,ax
    mov ax,0xa000
    mov es,ax
    xor di,di
    xor bx,bx
    mov dx,25
.row:
    mov cx,56
.cell:
    mov ax,[fs:bx]
    mov [es:di],ax
    mov ax,[fs:bx+2]
    mov [es:di+0x2000],ax
    add bx,4
    add di,2
    loop .cell
    add di,48
    dec dx
    jnz .row
    mov es,[cs:practice_pause_buffer]
    mov ah,0x49
    int 0x21
    mov word [cs:practice_pause_buffer],0
.done:
    ret
practice_pause_buffer: dw 0

; Draw into the displayed graphics page, accounting for its actual scroll.
practice_pause_gfx_save:
    mov word [cs:practice_pause_gfx],0
    mov bx,(52*368*2+26*22*4*2+(6*400+52*32)*4+15)/16
    mov ah,0x48
    int 0x21
    jc .done
    mov [cs:practice_pause_gfx],ax
    mov gs,ax
    PRACTICE_CAPTURE_SCROLL
    mov [cs:practice_pause_scroll],ax
    mov al,4
    out 0x6a,al
    xor al,al
    out 0x7c,al
    mov al,[PRACTICE_PAGE_FRONT]
    out 0xa6,al
    ; Save the full black margins, then pack the shaded playfield.
    mov byte [cs:practice_pause_copy],0
    call practice_pause_gfx_borders
    xor si,si
    xor bx,bx
.plane:
    mov es,[cs:practice_pause_planes+si]
    mov ax,[cs:practice_pause_scroll]
    add ax,16
    cmp ax,400
    jb .save_y
    sub ax,400
.save_y:
    imul di,ax,80
    add di,2
    mov dx,368
.row:
    mov byte [cs:practice_pause_mask],0x55
    test dl,1
    jz .masked
    mov byte [cs:practice_pause_mask],0xaa
.masked:
    mov cx,26
.pair:
    call practice_pause_pack_pixel
    mov [cs:practice_pause_nibble],al
    inc di
    call practice_pause_pack_pixel
    shl al,4
    or al,[cs:practice_pause_nibble]
    mov [gs:bx],al
    inc bx
    inc di
    loop .pair
    add di,28
    cmp di,32000
    jb .nowrap
    sub di,32000
.nowrap:
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    call practice_pause_gfx_button_copy
.done:
    ret
practice_pause_gfx_buttons:
    cmp word [cs:practice_pause_gfx],0
    je .done
    mov al,[PRACTICE_PAGE_FRONT]
    out 0xa6,al
    xor si,si
    cld
.plane:
    mov es,[cs:practice_pause_planes+si]
    mov ax,[cs:practice_pause_scroll]
    add ax,128
    cmp ax,400
    jb .y
    sub ax,400
.y:
    imul di,ax,80
    add di,15
    xor bp,bp
.button:
    xor al,al
    cmp si,0
    je .colour
    cmp si,6
    jne .highlight
.colour:
    mov al,0xff
.highlight:
    mov bx,bp
    cmp bl,[cs:practice_pause_choice]
    jne .rows
    mov al,0xff
.rows:
    mov dx,22
.row:
    mov cx,26
    rep stosb
    add di,54
    cmp di,32000
    jb .nowrap
    sub di,32000
.nowrap:
    dec dx
    jnz .row
    add di,800
    cmp di,32000
    jb .gap
    sub di,32000
.gap:
    inc bp
    cmp bp,4
    jb .button
    add si,2
    cmp si,8
    jb .plane
.done:
    ret
practice_pause_gfx_restore:
    mov ax,[cs:practice_pause_gfx]
    test ax,ax
    jz .done
    mov gs,ax
    mov al,[PRACTICE_PAGE_FRONT]
    out 0xa6,al
    mov byte [cs:practice_pause_copy],1
    call practice_pause_gfx_button_copy
    xor si,si
    xor bx,bx
.plane:
    mov es,[cs:practice_pause_planes+si]
    mov ax,[cs:practice_pause_scroll]
    add ax,16
    cmp ax,400
    jb .restore_y
    sub ax,400
.restore_y:
    imul di,ax,80
    add di,2
    mov dx,368
.row:
    mov byte [cs:practice_pause_mask],0x55
    test dl,1
    jz .masked
    mov byte [cs:practice_pause_mask],0xaa
.masked:
    mov cx,26
.pair:
    mov al,[gs:bx]
    and al,0x0f
    call practice_pause_unpack_pixel
    or [es:di],al
    inc di
    mov al,[gs:bx]
    shr al,4
    call practice_pause_unpack_pixel
    or [es:di],al
    inc bx
    inc di
    loop .pair
    add di,28
    cmp di,32000
    jb .nowrap
    sub di,32000
.nowrap:
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    call practice_pause_gfx_borders
    mov al,[PRACTICE_PAGE_BACK]
    out 0xa6,al
    mov es,[cs:practice_pause_gfx]
    mov ah,0x49
    int 0x21
    mov word [cs:practice_pause_gfx],0
.done:
    ret
practice_pause_planes: dw 0xa800,0xb000,0xb800,0xe000
practice_pause_gfx: dw 0
practice_pause_scroll: dw 0

; Pack the four erased alternating bits of one pixel byte into AL.
practice_pause_pack_pixel:
    mov al,[es:di]
    mov ah,al
    and ah,[cs:practice_pause_mask]
    mov [es:di],ah
    mov ah,[cs:practice_pause_mask]
    not ah
    and al,ah
    cmp byte [cs:practice_pause_mask],0x55
    jne .even
    shr al,1
.even:
    and al,0x55
    mov ah,al
    shr ah,1
    or al,ah
    and al,0x33
    mov ah,al
    shr ah,2
    or al,ah
    and al,0x0f
    ret
practice_pause_unpack_pixel:
    mov ah,al
    shl ah,2
    or al,ah
    and al,0x33
    mov ah,al
    shl ah,1
    or al,ah
    and al,0x55
    cmp byte [cs:practice_pause_mask],0x55
    jne .even
    shl al,1
.even:
    ret
; The other half of each button pixel is already in the shaded buffer. Save
; the remaining alternating bits in nibbles, after shading and before paint.
practice_pause_gfx_button_copy:
    mov bx,52*368*2
    xor si,si
.plane:
    mov es,[cs:practice_pause_planes+si]
    mov ax,[cs:practice_pause_scroll]
    add ax,128
    cmp ax,400
    jb .y
    sub ax,400
.y:
    imul di,ax,80
    add di,15
    mov bp,4
.button:
    mov dx,22
.row:
    mov byte [cs:practice_pause_mask],0xaa
    test dl,1
    jz .mask_ready
    mov byte [cs:practice_pause_mask],0x55
.mask_ready:
    mov cx,13
.cell:
    cmp byte [cs:practice_pause_copy],0
    jne .restore
    call practice_pause_pack_pixel
    mov [cs:practice_pause_nibble],al
    inc di
    call practice_pause_pack_pixel
    shl al,4
    or al,[cs:practice_pause_nibble]
    mov [gs:bx],al
    jmp .next
.restore:
    mov al,[gs:bx]
    and al,0x0f
    call practice_pause_unpack_pixel
    mov [es:di],al
    inc di
    mov al,[gs:bx]
    shr al,4
    call practice_pause_unpack_pixel
    mov [es:di],al
.next:
    inc bx
    inc di
    loop .cell
    add di,54
    cmp di,32000
    jb .nowrap
    sub di,32000
.nowrap:
    dec dx
    jnz .row
    add di,800
    cmp di,32000
    jb .gap
    sub di,32000
.gap:
    dec bp
    jnz .button
    add si,2
    cmp si,8
    jb .plane
    ret
; Keep off-screen tiles outside the playfield black during the pause. The
; buffer stores both vertical strips and the top/bottom margins.
practice_pause_gfx_borders:
    mov bx,52*368*2+26*22*4*2
    xor si,si
.plane:
    mov es,[cs:practice_pause_planes+si]
    mov ax,[cs:practice_pause_scroll]
    imul di,ax,80
    mov dx,400
.row:
    mov cx,2
    call .copy
    cmp dx,384
    ja .horizontal
    cmp dx,16
    jbe .horizontal
    add di,52
    jmp .right
.horizontal:
    mov cx,52
    call .copy
.right:
    mov cx,4
    call .copy
    add di,22
    cmp di,32000
    jb .nowrap
    sub di,32000
.nowrap:
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    ret
.copy:
    cmp byte [cs:practice_pause_copy],0
    jne .restore
    mov al,[es:di]
    mov [gs:bx],al
    mov byte [es:di],0
    jmp .next
.restore:
    mov al,[gs:bx]
    mov [es:di],al
.next:
    inc bx
    inc di
    loop .copy
    ret
practice_pause_mask: db 0
practice_pause_nibble: db 0
practice_pause_copy: db 0
