; In-game pause menu. The host supplies input and acknowledges replay I/O;
; all presentation, navigation and suspension happen in MAIN.
%ifndef PRACTICE_NETWORK_PAUSE
practice_pause:
    call practice_pause_screen_save
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
    call practice_pause_screen_restore
    mov byte [cs:practice_pause_open],0
    mov byte [cs:practice_pause_request],0
    mov byte [cs:practice_pause_ack],0
    ret
%endif
practice_pause_draw:
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
%ifdef PRACTICE_NETWORK_PAUSE
    mov si,practice_pause_option_exit
%else
    mov si,practice_pause_option_save
%endif
    call practice_text_write
%ifdef PRACTICE_NETWORK_PAUSE
    mov di,12*160+15*2
    mov si,practice_pause_option_retry
    call practice_text_write
%else
    mov di,12*160+15*2
    mov si,practice_pause_option_exit
    call practice_text_write
    mov di,14*160+15*2
    mov si,practice_pause_option_retry
    call practice_text_write
%endif
    xor bx,bx
    mov bl,[cs:practice_pause_choice]
    shl bx,1
    mov di,[cs:practice_pause_positions+bx]
    mov cx,26
.selected:
    mov word [es:di+0x2000],0xe5
    add di,2
    loop .selected
    pop ds
    ret
practice_pause_option_resume: db '          RESUME          ',0
practice_pause_option_save: db '       SAVE & EXIT        ',0
%ifdef PRACTICE_NETWORK_PAUSE
practice_pause_option_exit: db '           QUIT           ',0
%else
practice_pause_option_exit: db '       QUIT NO REP        ',0
%endif
practice_pause_option_retry: db '         RESTART          ',0
practice_pause_positions: dw 8*160+15*2,10*160+15*2,12*160+15*2,14*160+15*2
%ifdef PRACTICE_NETWORK_PAUSE
practice_pause_labels: dw practice_pause_option_resume,practice_pause_option_exit,practice_pause_option_retry
%else
practice_pause_labels: dw practice_pause_option_resume,practice_pause_option_save,practice_pause_option_exit,practice_pause_option_retry
%endif
practice_pause_open: db 0
practice_pause_choice: db 0
practice_pause_request: db 0
practice_pause_ack: db 0
practice_pause_input: dw 0
practice_pause_keys: dw 0

; Preserve only the text cells occupied by the four option rows.
practice_pause_screen_save:
    push cs
    pop fs
    mov ax,0xa000
    mov es,ax
    mov di,8*160+15*2
    mov bx,practice_pause_buffer
%ifdef PRACTICE_NETWORK_PAUSE
    mov dx,3
%else
    mov dx,4
%endif
.row:
    mov cx,26
.cell:
    mov ax,[es:di]
    mov [fs:bx],ax
    mov ax,[es:di+0x2000]
    mov [fs:bx+2],ax
    mov word [es:di],0x20
    mov word [es:di+0x2000],0
    add bx,4
    add di,2
    loop .cell
    add di,320-52
    dec dx
    jnz .row
    ret
practice_pause_screen_restore:
    push cs
    pop fs
    mov ax,0xa000
    mov es,ax
    mov di,8*160+15*2
    mov bx,practice_pause_buffer
    xor bp,bp
%ifdef PRACTICE_NETWORK_PAUSE
    mov dx,3
%else
    mov dx,4
%endif
.row:
    mov si,[cs:practice_pause_labels+bp]
    mov cx,26
.cell:
    mov al,[cs:si]
    cmp [es:di],al
    jne .next_cell
    mov ax,[fs:bx]
    mov [es:di],ax
    mov ax,[fs:bx+2]
    mov [es:di+0x2000],ax
.next_cell:
    inc si
    add bx,4
    add di,2
    loop .cell
    add di,320-52
    add bp,2
    dec dx
    jnz .row
    ret
%ifdef PRACTICE_NETWORK_PAUSE
practice_pause_buffer: times 26*4*3 db 0
%else
practice_pause_buffer: times 26*4*4 db 0
%endif
