; TH02-only single-player bridge, derived from the released EMS HUD MAIN.
bits 16
cpu 386
%include 'practice-config.inc'
org PRACTICE_ORG
%define ORIGINAL(a) (a-0xb160)
db 'T02PRA01'
dw load_hook,frame_hook,miss_hook,world_hook,entry_pause,hold
dw target_practice,lock_lives,unlimited_bombs,enemy_lock
dw death_gate,life_consume,bomb_gate,bomb_consume,lives_zero,bombs_zero
dw shots_return,dialog_box,dialog_pre,dialog_post
dw power_dirty,lock_time,time_mid1,time_mid2,time_mid3,time_mid4,time_mima
dw transition_hook
dw entities_hook
dw practice_pause_call
dw practice_assists
dw target_preparing
dw practice_initial_lives,practice_initial_bombs,practice_initial_power,practice_initial_rank
dw practice_target_resources
dw practice_initial_phase,practice_boss_phase
dw practice_target_advance
dw practice_initial_entry,practice_warp_state,practice_target_warp
dw practice_arrival_pending,practice_target_arrival
dw practice_target_reached,practice_target_check
dw practice_initial_hp,practice_target_hp
dw practice_initial_stage,practice_outcome,practice_target_outcome
dw practice_result_visible,practice_result_request,practice_result_ack,practice_result_input
dw wait_hold
dw practice_pause_open,practice_pause_request,practice_pause_ack,practice_pause_input
dw practice_form_open,practice_form_input,practice_form_values,practice_form_request,practice_form_hp,practice_form_options,practice_info_enabled,practice_info_values
dw practice_restart_pending,practice_restart_values,practice_result_selecting
dw practice_focus_hook
dw practice_focus_test
entry_pause: db 0
hold: db 0
target_practice: db 0
lock_lives: db 0
unlimited_bombs: db 0
enemy_lock: db 0
power_dirty: db 0
lock_time: db 0
target_preparing: db 0
redraw_pending: db 0
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
    call ORIGINAL(0xb2ab)
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
    mov dx,0x7fd0
    call signal
    pop dx
    pushf
    pushad
    push es
    call practice_loading_clear_if_present
    call practice_native_setup
    call practice_target_resources
    pop es
    popad
    popf
    ret
%macro PRACTICE_CAPTURE_SCROLL 0
    movzx si,byte [0x2ba9]
    shl si,1
    mov ax,[ss:bp+si-8]
%endmacro
frame_hook:
    pushf
    pushad
    push es
    call practice_info_overlay_restore
    PRACTICE_CAPTURE_SCROLL
    mov [cs:practice_form_scroll],ax
    call practice_target_resources
    call practice_loading_clear
    call practice_preparation_guard
    call practice_target_check
    pop es
    popad
    popf
    push dx
    mov dx,0x7fd1
    call signal
    pop dx
    pushf
    pushad
    push es
    call practice_target_warp
    call practice_target_advance
    call practice_target_arrival
    cmp byte [cs:power_dirty],0
    je .power_ready
    mov byte [cs:power_dirty],0
    call ORIGINAL(0xdead)
.power_ready:
    cmp byte [cs:entry_pause],0
    je .resume
    cmp byte [cs:practice_assists_drawn],0
    je .pause_menu_ready
    mov byte [cs:practice_assists_open],0
    call practice_assists_draw
.pause_menu_ready:
    mov byte [cs:entry_pause],0
    ; Same native pause adapter used by Esc. No game update until it returns.
.pause_call:
    call SOLO_PAUSE
    test ax,ax
    jz .resume
    mov byte [0x2b97],1
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
    cmp byte [cs:practice_restart_committed],0
    je .form_returned
    mov byte [cs:practice_restart_committed],0
    mov byte [cs:practice_form_request],0
.form_returned:
    pop gs
    pop fs
.form_closed:
    call practice_quick_retry_tick
    call practice_restart_tick
    call practice_native_restrictions
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
    call practice_info_tick
    call practice_info_draw
    pop es
    call practice_power_lock
    pop es
    popad
    popf
    jmp SOLO_UPDATE
wait_hold:
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
    call practice_restart_tick
    pop es
    popad
    popf
    ; The original frame delay reaches the emulator's sleep boundary, allowing
    ; replay sealing, rendering and menu key delivery while gameplay is frozen.
    push word 1
    call PRACTICE_DELAY_SEGMENT:0x1c8
    jmp .wait
.done:
    pop es
    popad
    popf
    ret
miss_hook:
    cmp byte [cs:target_practice],0
    je .counted
    cmp byte [cs:target_preparing],0
    jne .counted
    cmp byte [cs:practice_outcome],0
    jne .counted
    cmp byte [0x2b94],0
    je .counted
    cmp byte [0x2b99],0
    jne .counted
    inc word [cs:practice_info_misses]
.counted:
    push dx
    mov dx,0x7fd2
    call signal
    pop dx
    call wait_hold
    test byte [cs:practice_assists],1
    jz .check_hit
    mov byte [0x2b94],0
.check_hit:
    ; The caller has already negated Bomb/respawn invincibility. Bullets and
    ; enemies have already consumed their collision. F1 clears this flag only.
    cmp byte [0x2b94],0
    je .done
    call ORIGINAL(0xeff2)
.done:
    ret
world_hook:
    call 0:0x41aa ; original sparks_update_and_render; segment is MZ relocated
    pushf
    pushad
    call practice_target_outcome
    cmp byte [cs:target_practice],0
    je .native_time_done
    cmp byte [cs:target_preparing],0
    jne .native_time_done
    test byte [cs:practice_assists],0x10
    jz .native_time_done
    cmp byte [0x8f5],5
    jne .native_time_done
    cmp byte [0x2baa],0
    je .native_time_done
    cmp word [0x2be0],420
    jb .native_time_done
    mov word [0x2be0],0
.native_time_done:
    popad
    popf
    pushf
    cmp byte [cs:target_preparing],0
    je .redraw
    mov byte [cs:redraw_pending],2
.redraw:
    cmp byte [cs:redraw_pending],0
    je .redraw_done
    mov byte [0x52d8],1
    dec byte [cs:redraw_pending]
.redraw_done:
    popf
    push dx
    mov dx,0x7fd4
    call signal
    pop dx
    call practice_info_overlay_mask
    call wait_hold
    retf
entities_hook:
    ; After Boss/midboss generation, before items, lasers and bullets update
    ; or render. Suppress only preparation entities, retaining target shots.
    push dx
    mov dx,0x7fd5
    call signal
    pop dx
    pushf
    push ax
    push cx
    push si
    cmp byte [cs:target_preparing],0
    je .clean_done
    mov si,0x2c18
    mov cx,150
    mov ax,20
    call .clear_pool
    mov si,0x3ce6
    mov cx,20
    mov ax,16
    call .clear_pool
    mov si,0x5f6c
    mov cx,12
    mov ax,12
    call .clear_pool
.clean_done:
    pop si
    pop cx
    pop ax
    popf
    jmp ORIGINAL(0xdaf0) ; original items_update_and_render, near CALL ABI
.clear_pool:
    cmp byte [si],1
    jne .next
    mov byte [si],2
.next:
    add si,ax
    loop .clear_pool
    ret
death_gate:
    test byte [cs:practice_assists],2
    jnz .alive
    cmp byte [cs:lock_lives],0
    jne .alive
    cmp byte [0xb2c],0
    je .gameover
.alive:
    jmp ORIGINAL(0xf107)
.gameover:
    jmp ORIGINAL(0xf0e1)
life_consume:
    test byte [cs:practice_assists],2
    jnz .done
    cmp byte [cs:lock_lives],0
    jne .done
    dec byte [0xb2c]
.done:
    ret
bomb_gate:
    test byte [cs:practice_assists],4
    jnz .allowed
    cmp byte [cs:unlimited_bombs],0
    jne .allowed
    cmp byte [0xb2d],0
    je .empty
.allowed:
    jmp ORIGINAL(0xe291)
.empty:
    jmp ORIGINAL(0xe2d7)
bomb_consume:
    test byte [cs:practice_assists],4
    jnz .done
    cmp byte [cs:unlimited_bombs],0
    jne .done
    dec byte [0xb2d]
.done:
    ret
%macro STOCK 3
%1:
    pushf
    cmp byte [cs:target_practice],0
    je %%native
    cmp byte [%2],5
    jbe %%native
    mov byte [%2],5
%%native:
    popf
    push bp
    mov bp,sp
    jmp ORIGINAL(%3)
%endmacro
STOCK lives_zero,0xb2c,0xdf79
STOCK bombs_zero,0xb2d,0xdfc7
; Shared hit test still consumes player shots and runs collision geometry.
; Returning zero damage covers ordinary enemies, Bosses, stones and Bombs.
shots_return:
    add [0x3e3c],ax
    mov ax,[bp-8]
    pushf
    test byte [cs:practice_assists],32
    jnz .zero
    cmp byte [cs:enemy_lock],0
    je .native
.zero:
    xor ax,ax
.native:
    popf
    retf
%macro BACK 2
%1:
    pushf
    push ax
    mov ax,ds
    sub ax,DIALOG_DELTA
    mov [cs:%%pointer+2],ax
    pop ax
    popf
    jmp far [cs:%%pointer]
%%pointer: dw %2,0
%endmacro
; Only the timeout comparison is replaced. RETF retains native CS, stack,
; registers and comparison flags; the original conditional branch still runs.
%macro TIME_COMPARE 4
%1:
    test byte [cs:practice_assists],16
    jnz %%locked
    cmp byte [cs:lock_time],0
    je %%native
%%locked:
    cmp byte [cs:lock_time],2 ; CF=1, SF!=OF, ZF=0: JB/JL/JLE all true
    retf
%%native:
    cmp %2 [%3],%4
    retf
%endmacro
TIME_COMPARE time_mid1,word,0x28da,0xb8
TIME_COMPARE time_mid2,word,0x2be0,1600
TIME_COMPARE time_mid3,byte,0x5538,4
TIME_COMPARE time_mid4,byte,0x930d,12
time_mima:
    mov ax,[0x924e]
    test byte [cs:practice_assists],16
    jnz .locked
    cmp byte [cs:lock_time],0
    je .native
.locked:
    cmp ax,ax
    retf
.native:
    cmp ax,[0x9282]
    retf
transition_hook:
    push dx
    mov dx,0x7fd3
    call signal
    pop dx
    pushf
    pushad
    call practice_target_transition
    popad
    popf
    cmp word [0x2be0],0
    retf
; TH02 dialogs are hardcoded, unlike TH04/05's bytecode. Skip only each text
; box and its slide animation; the caller's music/palette/resource code runs.
dialog_box:
    pushf
    cmp byte [cs:target_practice],0
    je .native
    popf
    inc byte [0x740a]
    jmp box_skip
.native:
    popf
    enter 4,0
    push si
    jmp box_body
dialog_pre:
    pushf
    cmp byte [cs:target_practice],0
    jne .skip
    popf
    enter 4,0
    push si
    jmp pre_body
.skip:
    popf
    jmp dialog_return
dialog_post:
    pushf
    cmp byte [cs:target_practice],0
    jne .skip
    popf
    enter 4,0
    push si
    jmp post_body
.skip:
    popf
    jmp dialog_return
BACK box_skip,(0x13149-0xfac0)
BACK box_body,(0x13055-0xfac0+5)
BACK pre_body,(0x12de0-0xfac0+5)
BACK post_body,(0x12e95-0xfac0+5)
BACK dialog_return,(0x13c90-0xfac0)

%include 'practice-assists.inc'
%include 'practice-assists-draw.inc'
%include 'practice-text.inc'
%define POWER_BYTE 0xaa6
%define POWER_REFRESH call ORIGINAL(0xdead)
%include 'practice-power.inc'
%include 'practice-resources.inc'
%include 'practice-phase.inc'
%include 'practice-outcome.inc'
%define PRACTICE_NATIVE_FORM_START 1
%include 'practice-result.inc'
%macro PRACTICE_PAUSE_WAIT 0
    push word 1
    call PRACTICE_DELAY_SEGMENT:0x1c8
%endmacro
%define PRACTICE_PAGE_FRONT 0x2ba9
%define PRACTICE_PAGE_BACK 0x2ba8

%include 'practice-pause.inc'
%define FORM_STAGE_MAX 5
%define FORM_TH02 1
%define FORM_CHARACTER_MAX 2
%define FORM_PHASE_MAX 9
%define FORM_STOCK_MAX 5
%define FORM_POWER_MAX 80
%define FORM_BRANCH_MAX 0
%define FORM_RANK_DEFAULT 6
%define FORM_RANK_MAX 22
%define FORM_RANK_BASE -6
%define FORM_HP_MAX 8500
%include 'practice-form.inc'
%define PRACTICE_RESTART_TH02 1
%include 'practice-info.inc'
%include 'practice-restart.inc'
practice_pause_call equ frame_hook.pause_call
