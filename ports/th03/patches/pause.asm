bits 16
cpu 386
; The next segment shares its first 8 bytes with this code group's tail.
; The builder preserves those bytes before appending this payload.
org 0x5218

%define BASE 0x96e0
%define ORIGINAL(a) (a-BASE)
%define PLAYERS 0x65a6
%define ROUND_FRAME 0x6596
%define ROUND_RESULT 0x38dc
%define ROUND_LOSER 0x38dd
%define QUIT 0x65a0
%define RESIDENT 0x1d90
%define VSYNC_COUNT 0x11ea

dw frame_hook, round_hook, mailbox, move_hook, render_hook
mailbox:
    db 'TH03LOCALPAUSE1!'
ticks: dd 0
guest_ds: dw 0
phase: db 0                     ; 0 menus, 1 human VS, 2 VS result, 3 other round
command: db 0                   ; 1 surrender round, 2 surrender match
seat: db 0
ack: db 0                       ; 0 idle, 1 applied, 2 rejected, 3 read by browser
focus_input: db 0              ; enabled, held focus bits: 1P=1, 2P=2
point_options: db 3            ; show center while focused, per player
generation: dw 0
focus_active: db 0             ; sampled once per native frame
always_point: db 0             ; touch mode persistent point display
touch_state: times 12 db 0     ; per-player mode + signed x/y deltas

round_hook:
    inc word [cs:generation]
    mov byte [cs:command],0
    mov byte [cs:ack],0
    mov byte [cs:focus_active],0
    mov dword [cs:touch_state],0
    mov dword [cs:touch_state+4],0
    mov dword [cs:touch_state+8],0
    call ORIGINAL(0x9778)
    mov byte [cs:phase],0
    mov byte [cs:command],0
    ; MAINL reuses MAIN's memory. Retain the receipt until the browser reads
    ; it, with a native VSYNC timeout so a missing browser cannot block exit.
    pushf
    push ax
    push dx
    mov dx,[VSYNC_COUNT]
.wait_receipt:
    cmp byte [cs:ack],1
    je .receipt_timeout
    cmp byte [cs:ack],2
    jne .receipt_done
.receipt_timeout:
    mov ax,[VSYNC_COUNT]
    sub ax,dx
    cmp ax,180
    jb .wait_receipt
.receipt_done:
    pop dx
    pop ax
    popf
    ret

frame_hook:
    call ORIGINAL(0xa438)       ; original collision-map clear, once per frame
    pushf
    pushad
    push es
    inc dword [cs:ticks]
    mov [cs:guest_ds],ds
    mov byte [cs:phase],3
    mov byte [cs:focus_active],0
    les bx,[RESIDENT]
    cmp byte [es:bx+0x28],0x81 ; GM_VS_1P_2P
    jne .reject
    cmp word [es:bx+0x0e],0    ; both players human
    jne .reject
    cmp byte [es:bx+0x39],0    ; no attract demo
    jne .reject
    cmp byte [QUIT],0
    jne .reject
    mov byte [cs:phase],2
    cmp byte [ROUND_RESULT],0
    jne .reject
    cmp byte [PLAYERS+0x1f],0
    jne .reject
    cmp byte [PLAYERS+0x80+0x1f],0
    jne .reject
    mov byte [cs:phase],1
    mov al,[cs:focus_input]
    and al,3
    mov [cs:focus_active],al
    mov al,[cs:command]
    or al,al
    jz .done
    cmp al,2
    ja .reject
    cmp byte [cs:seat],1
    ja .reject
    mov byte [cs:command],0
    mov byte [cs:ack],1
    xor dx,dx
    mov dl,[cs:seat]           ; loser
    mov [ROUND_LOSER],dl
    mov si,dx
    shl si,7
    add si,PLAYERS
    mov byte [si+7],0          ; losing player's half-hearts
    mov byte [si+0x1f],0xff    ; original completed-loss animation sentinel
    xor dl,1                  ; winner
    mov [es:bx+0x17],dl        ; resident->pid_winner
    mov di,dx
    shl di,7
    add di,PLAYERS
    cmp al,2
    je .match
    inc byte [di+0x6c]        ; original rounds_won field
    jmp .settle
.match:
    mov byte [di+0x6c],2      ; native deciding-round tally, then MAINL win
.settle:
    mov byte [ROUND_RESULT],2
    mov word [ROUND_FRAME],0
    push dx
    xor dx,1
    push dx
    push cs
    call ORIGINAL(0xb92f)     ; far pascal hearts HUD update for loser
    pop dx
    push dx
    call ORIGINAL(0xba12)     ; native rounds-won HUD update (pascal)
    mov byte [cs:phase],2
    jmp .done
.reject:
    cmp byte [cs:command],0
    je .done
    mov byte [cs:command],0
    mov byte [cs:ack],2
.done:
    pop es
    popad
    popf
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
    cmp byte [ROUND_RESULT],0
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
    ; A 5x3 VRAM marker (black border, white middle) follows that same grid.
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
    mov bx,0x00f8
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
    mov [es:di+160],bx
    mov bx,0x0070
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
    cmp bp,2
    jb .player
.done:
    pop es
    popad
    popf
    pop bp
    ret 2
