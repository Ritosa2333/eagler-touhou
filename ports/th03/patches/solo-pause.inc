; TH05-style solo pause, drawn by MAIN itself into the PC-98 graphics planes.
; The menu owns the guest frame until it resumes or the host acknowledges an
; exit/restart request. The host only handles the browser replay file.
%define PAUSE_LEFT_BYTES 20
%define PAUSE_TOP 48
%define PAUSE_WIDTH_BYTES 40
%define PAUSE_HEIGHT 104
%define PAUSE_PLANE_BYTES (PAUSE_WIDTH_BYTES * PAUSE_HEIGHT)

solo_pause:
    pushf
    pushad
    push es
    mov byte [cs:phase],2
    mov byte [cs:mailbox+23],0
    mov byte [cs:mailbox+24],0
    mov byte [cs:mailbox+25],0
    mov byte [cs:pause_choice],0
    xor al,al
    out 0x7c,al             ; disable the GRCG before accessing individual planes
    mov al,[0x658f]         ; MAIN's currently displayed front page
    out 0xa6,al
    call pause_hide_text_layer
    call pause_save_background
    call pause_draw_buttons
.release:
    call pause_poll
    cmp word [0x1a9a],0
    jne .release
.input:
    call pause_poll
    mov al,[cs:mailbox+24]
    test al,0x80
    jz .keyboard
    mov byte [cs:mailbox+24],0
    and al,0x7f
    cmp al,3
    ja .input
    mov [cs:pause_choice],al
    call pause_draw_buttons
    jmp .confirm
.keyboard:
    mov ax,[0x1a9a]
    test ax,0x1000          ; Escape
    jnz .resume
    test ax,0x4000          ; Q = quit without saving
    jnz .discard
    test ax,1
    jnz .up
    test ax,2
    jnz .down
    test ax,0x2020          ; Z or Enter
    jz .input
.confirm:
    mov al,[cs:pause_choice]
    cmp al,0
    je .resume
    cmp al,1
    je .save
    cmp al,2
    je .discard
    mov al,3                ; restart
    jmp .request
.save:
    mov al,1                ; save replay and exit
    jmp .request
.discard:
    mov al,2                ; exit without replay
.request:
    mov [cs:mailbox+23],al
.await_host:
    call pause_poll
    cmp byte [cs:mailbox+25],0
    je .await_host
    cmp byte [cs:mailbox+25],1
    je .finish
    mov byte [cs:mailbox+23],0
    mov byte [cs:mailbox+25],0
    jmp .release
.up:
    dec byte [cs:pause_choice]
    and byte [cs:pause_choice],3
    call pause_draw_buttons
    jmp .release
.down:
    inc byte [cs:pause_choice]
    and byte [cs:pause_choice],3
    call pause_draw_buttons
    jmp .release
.resume:
    call pause_wait_release
.finish:
    call pause_restore_background
    call pause_restore_text_layer
    mov al,[0x658e]         ; restore MAIN's current back-page drawing target
    out 0xa6,al
    mov byte [cs:mailbox+23],0
    mov byte [cs:mailbox+25],0
    mov byte [cs:phase],0
    pop es
    popad
    popf
    ret

; Reuse MAIN's relocated input and frame-wait functions, not fixed segments.
pause_poll:
    inc dword [cs:ticks]
    call far [cs:0x30fc]
    push word 1
    call far [cs:0x3103]
    ret
pause_wait_release:
    call pause_poll
    cmp word [0x1a9a],0
    jne pause_wait_release
    ret

; The original center separator is drawn on PC-98 text VRAM above the
; graphics planes. Preserve its attributes while the centered panel is open.
pause_hide_text_layer:
    mov ax,0xa200
    mov es,ax
    mov di,6*160+PAUSE_LEFT_BYTES*2
    xor bx,bx
    mov dx,14
.row:
    mov cx,PAUSE_WIDTH_BYTES
.cell:
    mov ax,[es:di]
    mov [cs:pause_text_attributes+bx],ax
    mov word [es:di],0
    add bx,2
    add di,2
    loop .cell
    add di,160-PAUSE_WIDTH_BYTES*2
    dec dx
    jnz .row
    ret

pause_restore_text_layer:
    mov ax,0xa200
    mov es,ax
    mov di,6*160+PAUSE_LEFT_BYTES*2
    xor bx,bx
    mov dx,14
.row:
    mov cx,PAUSE_WIDTH_BYTES
.cell:
    mov ax,[cs:pause_text_attributes+bx]
    mov [es:di],ax
    add bx,2
    add di,2
    loop .cell
    add di,160-PAUSE_WIDTH_BYTES*2
    dec dx
    jnz .row
    ret

; Save all four planes before painting an opaque pause panel. The panel lies
; within the 640x400 game screen; each graphics VRAM row represents two rows.
pause_save_background:
    xor bx,bx
    xor si,si
.plane:
    mov ax,[cs:pause_planes+si]
    mov es,ax
    mov di,PAUSE_TOP*80+PAUSE_LEFT_BYTES
    mov dx,PAUSE_HEIGHT
.row:
    mov cx,PAUSE_WIDTH_BYTES
.pixel:
    mov al,[es:di]
    mov [cs:pause_background+bx],al
    mov byte [es:di],0
    inc bx
    inc di
    loop .pixel
    add di,80-PAUSE_WIDTH_BYTES
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    ret

pause_restore_background:
    xor bx,bx
    xor si,si
.plane:
    mov ax,[cs:pause_planes+si]
    mov es,ax
    mov di,PAUSE_TOP*80+PAUSE_LEFT_BYTES
    mov dx,PAUSE_HEIGHT
.row:
    mov cx,PAUSE_WIDTH_BYTES
.pixel:
    mov al,[cs:pause_background+bx]
    mov [es:di],al
    inc bx
    inc di
    loop .pixel
    add di,80-PAUSE_WIDTH_BYTES
    dec dx
    jnz .row
    add si,2
    cmp si,8
    jb .plane
    ret

pause_draw_buttons:
    xor si,si
.plane:
    mov ax,[cs:pause_planes+si]
    mov es,ax
    mov byte [cs:pause_row_choice],0
    mov di,56*80+22
.button:
    mov bp,di
    xor ax,ax
    mov al,[cs:pause_row_choice]
    imul bx,ax,720
    add bx,pause_glyphs
    mov dx,20
.row:
    mov cx,36
.pixel:
    ; A white selected button with dark glyphs keeps the selection legible
    ; under the game's current PC-98 palette.
    mov al,[cs:pause_row_choice]
    cmp al,[cs:pause_choice]
    jne .unselected
    mov al,[cs:bx]
    not al
    jmp .write
.unselected:
    xor al,al
    cmp si,0
    je .blue
    cmp si,6
    je .blue
    jmp .glyph
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
    inc byte [cs:pause_row_choice]
    cmp byte [cs:pause_row_choice],4
    jb .button
    add si,2
    cmp si,8
    jb .plane
    ret

pause_planes: dw 0xa800,0xb000,0xb800,0xe000
pause_choice: db 0
pause_row_choice: db 0
%include "patches/solo-pause-font.inc"
pause_background: times (PAUSE_PLANE_BYTES * 4) db 0
pause_text_attributes: times (PAUSE_WIDTH_BYTES * 14 * 2) db 0
