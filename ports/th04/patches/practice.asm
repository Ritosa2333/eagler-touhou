; Isolated TH04 single-player practice bridge. All OUT callbacks are synchronous.
bits 16
cpu 386
%include 'practice-config.inc'
org PRACTICE_ORG
db 'T04PRA01'
dw load_hook,frame_hook,player_hook,world_hook,entry_pause,hold
dw target_practice,lock_lives,unlimited_bombs,enemy_lock
dw respawn_lives,bomb_gate,bomb_consume,lives_zero,bombs_zero,shots_return
dw dialog_run_fast
dw resource_dirty
dw boss_hook
entry_pause: db 0
hold: db 0
target_practice: db 0
lock_lives: db 0
unlimited_bombs: db 0
enemy_lock: db 0
resource_dirty: db 0 ; bit 0 power/shot function/HUD, bit 1 Dream HUD
; Run native resource updates after JS writes, before pause/player/render.
; Both originals return far; push CS to preserve their calling convention.
refresh_resources:
    pushf
    pushad
    push es
    test byte [cs:resource_dirty],1
    jz .dream
    push cs
    call POWER_UPDATE
.dream:
    test byte [cs:resource_dirty],2
    jz .done
    push cs
    call DREAM_PUT
.done:
    mov byte [cs:resource_dirty],0
    pop es
    popad
    popf
    ret
signal:
    pushf
    pushad
    mov ax,ds
    movzx eax,ax
    shl eax,16
    mov ax,cs
    out dx,eax
    popad
    popf
    ret
load_hook:
    call (0xad03-0xaaf0)
    push dx
    mov dx,0x7fe0
    call signal
    pop dx
    ret
frame_hook:
    push dx
    mov dx,0x7fe1
    call signal
    pop dx
    call refresh_resources
    pushf
    pushad
    push es
    cmp byte [cs:entry_pause],0
    je .resume
    mov byte [cs:entry_pause],0
    call (0x10d4b-0xaaf0) ; original overlay_wipe
    call SOLO_PAUSE
    test ax,ax
    jz .resume
    mov byte [0x5392],1
.resume:
    pop es
    popad
    popf
    jmp word [0x428a]
player_hook:
    push dx
    mov dx,0x7fe2
    call signal
    pop dx
    call refresh_resources
    call SOLO_UPDATE
    push dx
    mov dx,0x7fe3
    call signal
    pop dx
    call refresh_resources
    ret
world_hook:
    call SOLO_FRAME
    push dx
    mov dx,0x7fe4
    call signal
    pop dx
    call refresh_resources
    pushf
    pushad
    push es
.wait:
    cmp byte [cs:hold],0
    je .done
    call (0xaaf2-0xaaf0) ; wait for native vsync, no extra game update
    jmp .wait
.done:
    pop es
    popad
    popf
    ret
boss_hook:
    call far [0xbcd0]
    pushf
    pushad
    push es
    cmp byte [cs:target_practice],0
    je .done
    mov dx,0x7fe5
    call signal
    call refresh_resources
    cmp byte [cs:entry_pause],0
    je .done
    ; Redraw this VRAM page before any foreground entity is rendered.
    ; The other page is marked dirty by the arrival callback too.
    call word [0x4268]
.done:
    pop es
    popad
    popf
    ret
respawn_lives:
    cmp byte [cs:lock_lives],0
    jne .locked
    cmp byte [es:bx+11],1
    jbe .gameover
    jmp (0x10a96-0xaaf0)
.gameover:
    jmp (0x10ab7-0xaaf0)
.locked:
    jmp (0x10a9a-0xaaf0)
bomb_gate:
    cmp byte [cs:unlimited_bombs],0
    jne .allowed
    cmp byte [es:bx+13],0
    je .empty
.allowed:
    jmp (0xffc9-0xaaf0)
.empty:
    jmp (0x10028-0xaaf0)
bomb_consume:
    cmp byte [cs:unlimited_bombs],0
    jne .done
    dec byte [es:bx+13]
.done:
    ret
; Inventory clamped before *every* native redraw, including pickups/respawns.
lives_zero:
    pushf
    push bx
    push es
    cmp byte [cs:target_practice],0
    je .native
    les bx,[0xba86]
    mov byte [es:bx+11],1
.native:
    pop es
    pop bx
    popf
    push bp
    mov bp,sp
    jmp (0xeeeb-0xaaf0)
bombs_zero:
    pushf
    push bx
    push es
    cmp byte [cs:target_practice],0
    je .native
    les bx,[0xba86]
    mov byte [es:bx+13],0
.native:
    pop es
    pop bx
    popf
    push bp
    mov bp,sp
    jmp (0xefa4-0xaaf0)
shots_return:
    mov ax,di
    pushf
    cmp byte [cs:enemy_lock],0
    je .native
    xor ax,ax
.native:
    popf
    pop di
    pop si
    leave
    retf
; Keep the original script opcode interpreter for resource/music/palette ops.
; Text and waits are omitted only during an explicitly committed target run.
dialog_run_fast:
    cmp byte [cs:target_practice],0
    jne .fast
    enter 6,0
    jmp DIALOG_BODY
.fast:
    push si
    push di
    push es
    xor di,di ; 0 outer script, 1 inside a two-byte text box
.next:
    les bx,[0x428c]
    mov al,[es:bx]
    inc word [0x428c]
    test al,al
    jz .done
    cmp al,0x20
    jbe .next
    cmp al,0x5c
    je .op
    test di,di
    jnz .text_pair
    cmp al,'0'
    je .player_box
    cmp al,'1'
    jne .next
    mov word [0x4290],0x30
    mov word [0x4292],0xc0
    mov word [0x4294],1
    jmp .box
.player_box:
    mov word [0x4290],0xa0
    mov word [0x4292],0x140
    mov word [0x4294],0
.box:
    mov di,1
    jmp .next
.text_pair:
    inc word [0x428c] ; an SJIS trail byte can itself be a backslash
    jmp .next
.op:
    les bx,[0x428c]
    mov al,[es:bx]
    inc word [0x428c]
    cmp al,'#'
    je .segment_end
    cmp al,'$'
    je .segment_end
    cmp al,'k'
    je .next
    xor ah,ah
    push ax
    call DIALOG_OP
    jmp .next
.segment_end:
    ; STOP leaves a text box first; only an outer STOP leaves dialog_run.
    test di,di
    jz .done
    xor di,di
    jmp .next
.done:
    call (0x10d4b-0xaaf0)
    pop es
    pop di
    pop si
    ret
