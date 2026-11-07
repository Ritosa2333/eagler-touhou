; In-guest target result menu. The host only acknowledges replay I/O and the
; requested DOS transition; menu drawing, focus and input remain in MAIN.
; Caller preserves registers, flags and segments. AH/AL keyboard state is read
; from the PC-98 BIOS key bitmap, so no browser form owns menu navigation.
practice_result_tick:
    cmp byte [cs:practice_outcome],0
    je .done
    cmp byte [cs:practice_result_visible],0
    jne .ack
    cmp byte [cs:practice_assists_drawn],0
    je .save_screen
    mov byte [cs:practice_assists_open],0
    call practice_assists_draw
.save_screen:
    mov ax,0xa000
    mov es,ax
    mov di,8*160
    mov cx,240
    call practice_text_save
    mov byte [cs:practice_result_visible],1
    mov byte [cs:practice_result_choice],0
    mov word [cs:practice_result_keys],0xffff
    call practice_result_draw
.ack:
    cmp byte [cs:practice_result_request],0
    je .input
    cmp byte [cs:practice_result_ack],0
    je .done
    cmp byte [cs:practice_result_ack],1
    jne .cancel_request
    cmp byte [cs:practice_result_request],2
    je .cancel_request
    mov ax,0xa000
    mov es,ax
    mov di,8*160
    mov cx,240
    call practice_text_restore
    mov byte [cs:practice_result_visible],0
    mov byte [cs:hold],0
.cancel_request:
    mov byte [cs:practice_result_request],0
    mov byte [cs:practice_result_ack],0
    mov word [cs:practice_result_keys],0xffff
    ret
.input:
    mov dx,[cs:practice_result_input]
    mov bx,[cs:practice_result_keys]
    mov [cs:practice_result_keys],dx
    not bx
    and bx,dx
    test bl,8
    jz .up
    mov byte [cs:practice_result_choice],0
    call practice_result_draw
    ret
.up:
    test bl,1
    jz .next
    cmp byte [cs:practice_result_choice],0
    jne .decrement
    mov byte [cs:practice_result_choice],3
.decrement:
    dec byte [cs:practice_result_choice]
    call practice_result_draw
    ret
.next:
    test bl,2
    jz .accept
    inc byte [cs:practice_result_choice]
    cmp byte [cs:practice_result_choice],3
    jb .redraw
    mov byte [cs:practice_result_choice],0
.redraw:
    call practice_result_draw
    ret
.accept:
    test bl,4
    jz .done
    mov al,[cs:practice_result_choice]
    inc al
    mov [cs:practice_result_request],al
.done:
    ret

practice_result_draw:
    push ds
    push cs
    pop ds
    mov ax,0xa000
    mov es,ax
    mov bl,0xe1
    mov di,8*160+20*2
    mov si,practice_result_clear
    cmp byte [cs:practice_outcome],1
    je .title
    mov si,practice_result_failed
.title:
    call practice_text_write
    mov di,9*160+20*2
    mov si,practice_result_options
    call practice_text_write
    mov di,10*160+20*2
    mov si,practice_result_cursor
    call practice_text_write
    xor bx,bx
    mov bl,[cs:practice_result_choice]
    shl bx,1
    mov di,[cs:practice_result_positions+bx]
    mov word [es:di], '>'
    mov word [es:di+0x2000],0xe1
    pop ds
    ret
practice_result_clear: db 'PRACTICE CLEAR ',0
practice_result_failed: db 'PRACTICE FAILED',0
practice_result_options: db ' RETRY       SAVE       SELECT',0
practice_result_cursor: db '                              ',0
practice_result_positions: dw 10*160+20*2,10*160+32*2,10*160+43*2
practice_result_visible: db 0
practice_result_choice: db 0
practice_result_request: db 0 ; 1 retry, 2 save, 3 select target
practice_result_ack: db 0     ; 1 success, 2 reject
practice_result_input: dw 0   ; held directions, confirm and cancel
practice_result_keys: dw 0
