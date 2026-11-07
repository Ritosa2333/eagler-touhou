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
dw practice_pause_call
dw practice_assists
dw practice_cleanup_keep_bits,practice_cleanup_counts,practice_cleanup_pending
dw practice_redraw_frame
dw target_preparing,practice_initial_lives,practice_initial_bombs,practice_initial_power,practice_initial_rank,practice_target_resources
dw practice_initial_entry,practice_warp_state,practice_target_warp
dw practice_arrival_pending,practice_target_arrival
dw practice_initial_phase,practice_initial_gengetsu,practice_target_reached,practice_target_check
dw practice_initial_hp,practice_target_hp
dw practice_initial_reimu,practice_target_advance
dw practice_initial_stage,practice_outcome,practice_target_outcome
dw practice_result_visible,practice_result_request,practice_result_ack,practice_result_input
dw practice_pause_open,practice_pause_request,practice_pause_ack,practice_pause_input
dw practice_form_open,practice_form_input,practice_form_values,practice_form_request,practice_form_hp
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
    pushf
    pushad
    mov al,0x41
    out 0x6a,al
    mov al,1
    out 0x6a,al
    mov ah,0x40
    int 0x18
    popad
    popf
    push dx
    mov dx,0x7fe0
    call signal
    pop dx
    pushf
    pushad
    call practice_target_stocks
    popad
    popf
    ret
frame_hook:
    pushf
    pushad
    push es
    call practice_target_resources
    call practice_target_check
    pop es
    popad
    popf
    push dx
    mov dx,0x7fe1
    call signal
    pop dx
    call refresh_resources
    pushf
    pushad
    push es
    call practice_target_warp
    call practice_target_advance
    call practice_redraw_frame
    call practice_target_arrival
    cmp byte [cs:practice_cleanup_pending],0
    je .cleanup_ready
    call practice_cleanup
.cleanup_ready:
    cmp byte [cs:entry_pause],0
    je .resume
    cmp byte [cs:practice_assists_drawn],0
    je .pause_menu_ready
    mov byte [cs:practice_assists_open],0
    call practice_assists_draw
.pause_menu_ready:
    mov byte [cs:entry_pause],0
    call (0x10d4b-0xaaf0) ; original overlay_wipe
.pause_call:
    call SOLO_PAUSE
    test ax,ax
    jz .resume
    mov byte [0x5392],1
.resume:
    cmp byte [cs:practice_pause_open],0
    je .pause_closed
    push fs
    push gs
    call practice_pause
    pop gs
    pop fs
.pause_closed:
    cmp byte [cs:practice_form_open],0
    je .form_closed
    cmp byte [cs:practice_assists_drawn],0
    je .form_ready
    mov byte [cs:practice_assists_open],0
    call practice_assists_draw
.form_ready:
    push fs
    push gs
    call practice_form
    pop gs
    pop fs
.form_closed:
    ; Poll the PC-98 BIOS keyboard bitmap in the game process. The native
    ; trainer state is independent of browser keyboard focus or DOM controls.
    push es
    xor ax,ax
    mov es,ax
    mov al,[es:0x52b]
    mov ah,[es:0x536]
    mov dl,[es:0x52c]
    call practice_assists_poll
    or al,[cs:practice_assists_open]
    test al,al
    jz .assist_done
    call practice_assists_draw
.assist_done:
    pop es
    call practice_power_lock
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
    pushf
    test byte [cs:practice_assists],1
    jz .hit_ready
    cmp byte [0x4662],1
    ja .hit_ready
    cmp byte [0x466a],0
    jne .hit_ready
    mov byte [0x4669],0
.hit_ready:
    popf
    pushf
    pushad
    call practice_time_lock
    popad
    popf
    call SOLO_UPDATE
    push dx
    mov dx,0x7fe3
    call signal
    pop dx
    call refresh_resources
    ret
world_hook:
    call SOLO_FRAME
    pushf
    pushad
    push es
    call practice_target_outcome
    pop es
    popad
    popf
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
    inc dword [cs:SOLO_MAILBOX+16]
    pushf
    pushad
    push es
    call practice_result_tick
    pop es
    popad
    popf
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
    call practice_target_outcome
    call practice_target_check
    mov dx,0x7fe5
    call signal
    call refresh_resources
    call practice_target_arrival
    cmp byte [cs:entry_pause],0
    je .done
    cmp byte [cs:practice_cleanup_pending],0
    je .cleanup_ready
    call practice_cleanup
.cleanup_ready:
    ; Redraw this VRAM page before any foreground entity is rendered.
    ; The other page is marked dirty by the arrival callback too.
    call word [0x4268]
.done:
    pop es
    popad
    popf
    ret
respawn_lives:
    test byte [cs:practice_assists],2
    jnz .locked
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
    test byte [cs:practice_assists],4
    jnz .allowed
    cmp byte [cs:unlimited_bombs],0
    jne .allowed
    cmp byte [es:bx+13],0
    je .empty
.allowed:
    jmp (0xffc9-0xaaf0)
.empty:
    jmp (0x10028-0xaaf0)
bomb_consume:
    test byte [cs:practice_assists],4
    jnz .done
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
    test byte [cs:practice_assists],32
    jnz .zero
    cmp byte [cs:enemy_lock],0
    je .native
.zero:
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

%include 'practice-assists.inc'
%include 'practice-assists-draw.inc'
%include 'practice-text.inc'
%define POWER_BYTE 0x4664
%macro POWER_REFRESH 0
    push cs
    call POWER_UPDATE
%endmacro
%include 'practice-power.inc'
%include 'practice-time.inc'
%include 'practice-cleanup.inc'
%include 'practice-resources.inc'
%include 'practice-advance.inc'
%include 'practice-outcome.inc'
%include 'practice-result.inc'
%macro PRACTICE_PAUSE_WAIT 0
    call (0xaaf2-0xaaf0)
%endmacro
%define PRACTICE_PAGE_FRONT 0x46fd
%define PRACTICE_PAGE_BACK 0x46fc

%macro PRACTICE_CAPTURE_SCROLL 0
    movzx si,byte [0x46fd]
    shl si,1
    mov ax,[si+0x4260]
%endmacro
%include 'practice-pause.inc'
%define FORM_STAGE_MAX 6
%define FORM_CHARACTER_MAX 3
%define FORM_PHASE_MAX 17
%define FORM_STOCK_MAX 8
%define FORM_POWER_MAX 128
%define FORM_BRANCH_MAX 1
%define FORM_RANK_DEFAULT 12
%define FORM_RANK_MAX 30
%define FORM_RANK_BASE 4
%define FORM_HP_MAX 30000
%include 'practice-form.inc'
practice_pause_call equ frame_hook.pause_call
