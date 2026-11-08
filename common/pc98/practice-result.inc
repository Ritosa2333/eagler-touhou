; In-guest target result menu. The host only acknowledges replay I/O and the
; requested DOS transition; menu drawing, focus and input remain in MAIN.
; Caller preserves registers, flags and segments. AH/AL keyboard state is read
; from the PC-98 BIOS key bitmap, so no browser form owns menu navigation.
practice_result_tick:
    cmp byte [cs:practice_outcome],0
    je .done
    cmp byte [cs:practice_result_selecting],0
    jne .done
    cmp byte [cs:practice_result_visible],0
    jne .ack
    cmp byte [cs:practice_assists_drawn],0
    je .save_screen
    mov byte [cs:practice_assists_open],0
    call practice_assists_draw
.save_screen:
    call practice_result_save
    mov byte [cs:practice_result_visible],1
    mov byte [cs:practice_result_choice],0
    mov byte [cs:practice_result_confirm_armed],0
    ; Wait until the input bridge has sampled the visible menu. Gameplay shot
    ; may still be held even though the old result-input mailbox was zero.
    mov word [cs:practice_result_input],0xffff
    mov word [cs:practice_result_keys],0xffff
    call practice_result_draw
.ack:
%ifdef PRACTICE_NATIVE_FORM_START
    cmp byte [cs:practice_result_request],1
    je .native_retry
%endif
    cmp byte [cs:practice_result_request],0
    je .input
    cmp byte [cs:practice_result_ack],0
    je .done
    cmp byte [cs:practice_result_ack],1
    jne .cancel_request
    cmp byte [cs:practice_result_request],2
    je .cancel_request
    call practice_result_restore
    mov byte [cs:practice_result_visible],0
    mov byte [cs:hold],0
.cancel_request:
    mov byte [cs:practice_result_request],0
    mov byte [cs:practice_result_ack],0
    mov word [cs:practice_result_keys],0xffff
    ret
.input:
    mov dx,[cs:practice_result_input]
    cmp dx,0xffff
    je .done
    mov bx,[cs:practice_result_keys]
    mov [cs:practice_result_keys],dx
    not bx
    and bx,dx
    test dl,4
    jnz .shot_held
    or byte [cs:practice_result_confirm_armed],1
.shot_held:
    test byte [cs:practice_result_confirm_armed],1
    jnz .enter_check
    and bl,0xfb
.enter_check:
    test dl,16
    jnz .enter_held
    or byte [cs:practice_result_confirm_armed],2
.enter_held:
    test byte [cs:practice_result_confirm_armed],2
    jnz .confirm_ready
    and bl,0xef
.confirm_ready:
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
    test bl,20
    jz .done
    mov al,[cs:practice_result_choice]
%ifdef PRACTICE_NATIVE_FORM_START
    test al,al
    jz .native_retry
%endif
    cmp al,2
    je .select_target
    inc al
    mov [cs:practice_result_request],al
.done:
    ret
%ifdef PRACTICE_NATIVE_FORM_START
.native_retry:
    call practice_result_restore
    mov byte [cs:practice_result_visible],0
    mov byte [cs:practice_result_selecting],1
    mov byte [cs:practice_form_request],1
    call practice_form_native_start
    mov byte [cs:practice_form_request],0
    mov byte [cs:practice_restart_committed],0
    ret
%endif
.select_target:
    call practice_result_restore
    mov byte [cs:practice_result_visible],0
    mov word [cs:practice_form_input],0
    mov byte [cs:practice_form_request],0
    mov byte [cs:practice_result_selecting],1
    call practice_form
%ifdef PRACTICE_NATIVE_FORM_START
    cmp byte [cs:practice_restart_committed],0
    je .select_legacy
    mov byte [cs:practice_restart_committed],0
    mov byte [cs:practice_form_request],0
    ret
.select_legacy:
%endif
    cmp byte [cs:practice_form_request],1
    jne .select_cancel
    ; Stay in MAIN's native wait loop until the new target has been accepted.
    ret
.select_cancel:
    mov byte [cs:practice_result_selecting],0
    mov byte [cs:practice_form_request],0
    call practice_result_save
    mov byte [cs:practice_result_visible],1
    mov byte [cs:practice_result_choice],2
    mov byte [cs:practice_result_confirm_armed],0
    ; The key that closed the form must not also reset the result selection.
    mov word [cs:practice_result_input],8
    mov word [cs:practice_result_keys],8
    call practice_result_draw
    ret

practice_result_draw:
    push ds
    push cs
    pop ds
    mov ax,0xa000
    mov es,ax
    mov bl,0xe1
    mov di,8*160+15*2
    mov si,practice_result_clear
    cmp byte [cs:practice_outcome],1
    je .title
    mov si,practice_result_failed
.title:
    call practice_text_write
    mov di,10*160+15*2
    mov si,practice_result_retry
    call practice_text_write
    mov di,12*160+15*2
    mov si,practice_result_save_option
    call practice_text_write
    mov di,14*160+15*2
    mov si,practice_result_select
    call practice_text_write
    xor bx,bx
    mov bl,[cs:practice_result_choice]
    shl bx,1
    mov di,[cs:practice_result_positions+bx]
    mov cx,26
.selected:
    mov word [es:di+0x2000],0xe5
    add di,2
    loop .selected
    pop ds
    ret
practice_result_clear: db '      PRACTICE CLEAR      ',0
practice_result_failed: db '     PRACTICE FAILED      ',0
practice_result_retry: db '          RETRY           ',0
practice_result_save_option: db '           SAVE           ',0
practice_result_select: db '          SELECT          ',0
practice_result_positions: dw 10*160+15*2,12*160+15*2,14*160+15*2

; Preserve the same four spaced rows used by the native pause menu.
practice_result_save:
    mov ax,0xa000
    mov es,ax
    mov di,8*160+15*2
    mov si,practice_text_saved
    mov dx,4
.row:
    mov cx,26
.cell:
    mov ax,[es:di]
    mov [cs:si],ax
    mov ax,[es:di+0x2000]
    mov [cs:si+2],ax
    add si,4
    add di,2
    loop .cell
    add di,268
    dec dx
    jnz .row
    ret
practice_result_restore:
    mov ax,0xa000
    mov es,ax
    mov di,8*160+15*2
    mov si,practice_text_saved
    mov dx,4
.row:
    mov cx,26
.cell:
    mov ax,[cs:si]
    mov [es:di],ax
    mov ax,[cs:si+2]
    mov [es:di+0x2000],ax
    add si,4
    add di,2
    loop .cell
    add di,268
    dec dx
    jnz .row
    ret
practice_result_visible: db 0
practice_result_choice: db 0
practice_result_request: db 0 ; 1 retry, 2 save, 3 select target
practice_result_ack: db 0     ; 1 success, 2 reject
practice_result_input: dw 0   ; held directions, confirm and cancel
practice_result_confirm_armed: db 0
practice_result_keys: dw 0
practice_result_selecting: db 0
