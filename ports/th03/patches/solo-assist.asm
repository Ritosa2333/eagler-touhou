bits 16
cpu 386
org 0x5218
%define BASE 0x96e0
%define ORIGINAL(a) (a-BASE)
%define PLAYERS 0x65a6
%define SOLO_ASSIST 1

dw frame_hook, round_hook, mailbox, move_hook, pause_hook
mailbox:
    db 'TH03SOLOASSIST1!'
ticks: dd 0
guest_ds: dw 0
phase: db 0
    times 3 db 0              ; compatible unused command/seat/ack fields
focus_input: db 0
    db 0                      ; native marker rendering is never installed
generation: dw 0
focus_active: db 0
    db 0
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
