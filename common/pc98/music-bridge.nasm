; TH03 resident PMD bridge, adapted from TH04's local music launcher.
; Keep PMD measures and sound effects alive; mask all 15 music parts.
bits 16
cpu 386
org 0x100
    jmp start
; PMD still advances measures/fades so native timing queries remain valid.
; Its documented AH=1E masks silence music parts only; part 15 (FM effects)
; is deliberately left enabled. No MAIN.EXE patch space is used.
remove_local_bgm:
    mov ax,0x3560
    int 0x21
    cmp bx,local_pmd
    jne .done
    cmp dword [es:bgm_mailbox],0x33304854 ; TH03
    jne .done
    mov bx,es
    cmp [es:bgm_segment],bx
    jne .done
    push es
    push ds
    mov dx,[es:original_pmd]
    mov ax,[es:original_pmd+2]
    mov ds,ax
    mov ax,0x2560
    int 0x21
    pop ds
    pop es
    ; The resident launcher has no callbacks once INT 60h is restored.
    mov ah,0x49
    int 0x21
.done:
    ret

install_local_bgm:
    mov ax,0x3560
    int 0x21
    cmp word [es:bx+2],0x4d50 ; PM
    jne .bad
    cmp byte [es:bx+4],'D'
    jne .bad
    mov [cs:original_pmd],bx
    mov [cs:original_pmd+2],es
    mov ax,[es:bx+5]
    mov [cs:pmd_version],ax
    mov [cs:bgm_segment],cs
    push ds
    push cs
    pop ds
    mov dx,local_pmd
    mov ax,0x2560
    int 0x21
    pop ds
    clc
    ret
.bad:
    stc
    ret

local_pmd:
    jmp short pmd_dispatch
    db 'PMD'
pmd_version: dw 0
pmd_dispatch:
%ifdef LOCAL_SFX
    ; Network variant also copies the game's already-prioritized FM effect
    ; command. Forward it unchanged so PMD queries/state remain original.
    cmp ah,0x0c
    je .event
%endif
    cmp ah,2
    jbe .event
    cmp ah,0x19
    je .event
    cmp ah,0x1a
    je .event
    cmp ah,0x1b
    je .event
    jmp far [cs:original_pmd]
.event:
    pushad
    push ds
    push es
    mov [cs:bgm_command],ax
    test ah,ah
    jnz .record
    mov ah,6
    pushf
    call far [cs:original_pmd]
    mov si,dx
    ; Immutable PMD header: flag byte + first 12 part offsets. Song data beyond
    ; this header may be modified by the driver's MML loop counters.
    mov eax,0x811c9dc5
    mov cx,25
.hash:
    movzx edx,byte [ds:si]
    xor eax,edx
    imul eax,eax,0x01000193
    inc si
    loop .hash
    mov [cs:bgm_track],eax
.record:
    mov bx,[cs:bgm_sequence]
    mov di,bx
    and di,63
    shl di,3
    inc bx
    mov [cs:bgm_events+di],bx
    mov ax,[cs:bgm_command]
    mov [cs:bgm_events+di+2],ax
    mov eax,[cs:bgm_track]
    mov [cs:bgm_events+di+4],eax
    mov [cs:bgm_sequence],bx
    pop es
    pop ds
    popad
    test ah,ah
    jz .play
    jmp far [cs:original_pmd]
.play:
    pushf
    call far [cs:original_pmd]
    pushad
    push ds
    push es
    xor bx,bx
.mask:
    mov ax,bx
    mov ah,0x1e
    pushf
    call far [cs:original_pmd]
    inc bx
    cmp bx,15
    jb .mask
    pop es
    pop ds
    popad
    iret
original_pmd: dd 0
bgm_command: dw 0
bgm_track: dd 0
bgm_mailbox: db 'TH03LOCALBGMv01!'
bgm_segment: dw 0
bgm_sequence: dw 0
bgm_events: times 64*8 db 0

resident_end:
start:
    cmp byte [0x80],0
    je .install
    call remove_local_bgm
    mov ax,0x4c00
    int 0x21
.install:
    call install_local_bgm
    jc .fail
    ; Release our DOS environment before retaining the interrupt wrapper.
    mov es,[0x2c]
    mov ah,0x49
    int 0x21
    mov dx,(resident_end-$$+0x100+15)/16
    mov ax,0x3100
    int 0x21
.fail:
    mov ax,0x4c01
    int 0x21
