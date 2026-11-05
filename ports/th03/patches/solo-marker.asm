bits 16
cpu 386
org 0x5218
%define BASE 0x96e0
%define ORIGINAL(a) (a-BASE)
%define PLAYERS 0x65a6
%define SOLO_ASSIST 1

dw frame_hook, round_hook, mailbox, move_hook, pause_hook, render_hook
mailbox:
    db 'TH03SOLOASSIST1!'
ticks: dd 0
guest_ds: dw 0
phase: db 0
    times 3 db 0              ; compatible unused command/seat/ack fields
focus_input: db 0
point_options: db 1
generation: dw 0
focus_active: db 0
always_point: db 0
touch_state: times 12 db 0

round_hook:
    inc word [cs:generation]
    mov byte [cs:phase],0
    mov byte [cs:focus_active],0
    mov dword [cs:touch_state],0
    mov dword [cs:touch_state+4],0
    mov dword [cs:touch_state+8],0
    call ORIGINAL(0x9778)
    mov byte [cs:phase],0
    ret

frame_hook:
    call ORIGINAL(0xa438)
    pushf
    pushad
    push es
    inc dword [cs:ticks]
    mov [cs:guest_ds],ds
    mov byte [cs:phase],0
    mov byte [cs:focus_active],0
    les bx,[0x1d90]
    cmp byte [es:bx+0x39],0   ; never modify attract demos
    jne .done
    cmp byte [es:bx+0x0e],0  ; only the human left-hand player
    jne .done
    cmp byte [es:bx+0x0f],0  ; never touch local two-player matches
    je .done
    mov al,[es:bx+0x28]
    cmp al,1                 ; Story
    je .mode_ok
    cmp al,0x80              ; 1P vs CPU
    jne .done
.mode_ok:
    cmp byte [0x65a0],0      ; quit
    jne .done
    cmp byte [0x38dc],0      ; round decided
    jne .done
    cmp byte [PLAYERS+0x1f],0
    jne .done
    mov byte [cs:phase],1
    mov al,[cs:focus_input]
    and al,1                 ; CPU always retains its original movement
    mov [cs:focus_active],al
.done:
    pop es
    popad
    popf
    ret

; Preserve the original Escape / Q pause routine and its return registers.
pause_hook:
    mov byte [cs:phase],0
    mov byte [cs:focus_active],0
    mov dword [cs:touch_state],0
    mov dword [cs:touch_state+4],0
    mov dword [cs:touch_state+8],0
    call ORIGINAL(0xc7a5)
    ret

%include "patches/movement.inc"

; Called after 2P's final glow/charge rendering, with EGC already disabled.
render_hook:
    push bp
    mov bp,sp
    push word [bp+4]
    call ORIGINAL(0xdf18)
    pushf
    pushad
    push es
    cmp byte [0x38dc],0
    jne .done
    xor bp,bp
.player:
    mov cx,bp
    mov al,1
    shl al,cl
    test [cs:always_point],al
    jnz .draw
    test [cs:focus_active],al
    jz .next
    test [cs:point_options],al
    jz .next
.draw:
    mov si,bp
    shl si,7
    add si,PLAYERS
    cmp byte [si+0x1f],0
    jne .next
    ; Match the collision origin in screen space, including field shake.
    mov ax,[si]
    sar ax,4
    mov bx,bp
    shl bx,1
    add ax,[bx+0x659c]
    add ax,16-2
    cmp bp,0
    je .x_ready
    add ax,320
.x_ready:
    ; TH03 uses 200 VRAM rows. Include PLAYFIELD_TOP=16 screen pixels.
    ; sprite16_put divides screen Y by two (native player top is y-16,
    ; sprite height is 64 screen pixels, so its center is y+16).
    ; Two VRAM rows display as four screen pixels; use a 4x4 square.
    mov dx,[si+2]
    sar dx,5
    add dx,8-1
    cmp ax,0
    jl .next
    cmp ax,635
    ja .next
    cmp dx,197
    ja .next
    imul di,dx,80
    mov cx,ax
    and cx,7
    shr ax,3
    add di,ax
    mov bx,0x00f0
    ror bx,cl                  ; spans two bytes at every X alignment
    mov ax,0xa800
    mov es,ax
    mov al,0xc0
    out 0x7c,al
    xor al,al
    out 0x7e,al
    out 0x7e,al
    out 0x7e,al
    out 0x7e,al
    mov [es:di],bx
    mov [es:di+80],bx
    mov bx,0x0060
    ror bx,cl
    mov al,0xff
    out 0x7e,al
    out 0x7e,al
    out 0x7e,al
    out 0x7e,al
    mov [es:di+80],bx
    xor al,al
    out 0x7c,al
.next:
    inc bp
    cmp bp,1
    jb .player
.done:
    pop es
    popad
    popf
    pop bp
    ret 2
