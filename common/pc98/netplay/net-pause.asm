bits 16
cpu 386
%include 'net-pause-config.inc'
org PATCH_ORG
%define PRACTICE_NETWORK_PAUSE 1
%define SOLO_MAILBOX 0
%if GAME = 2
%define PRACTICE_PAGE_FRONT 0x2ba9
%define PRACTICE_PAGE_BACK 0x2ba8
%macro PRACTICE_CAPTURE_SCROLL 0
    movzx si,byte [0x2ba9]
    shl si,1
    mov ax,[ss:bp+si-8]
%endmacro
%else
%define PRACTICE_PAGE_FRONT 0x46fd
%define PRACTICE_PAGE_BACK 0x46fc
%macro PRACTICE_CAPTURE_SCROLL 0
    movzx si,byte [0x46fd]
    shl si,1
    mov ax,[si+0x4260]
%endmacro
%endif
%macro PRACTICE_PAUSE_WAIT 0
    nop
%endmacro
jmp network_pause
network_pause:
    pushf
    pushad
    push ds
    push es
    push fs
    push gs
    call practice_pause_screen_save
    mov byte [cs:practice_pause_choice],0
    call practice_pause_draw
%if GAME = 2
    mov word [cs:network_previous],0xffff
.loop:
    push word 1
    call DELAY_SEGMENT:0x1c8
    call DELAY_SEGMENT:0x1de
    mov dx,[0x2538]
    mov bx,[cs:network_previous]
    mov [cs:network_previous],dx
    not bx
    and bx,dx
    test bl,0x40
    jnz .resume
    test bl,1
    jz .down
    dec byte [cs:practice_pause_choice]
    jns .redraw
    mov byte [cs:practice_pause_choice],2
.redraw:
    call practice_pause_draw
    jmp .loop
.down:
    test bl,2
    jz .confirm
    inc byte [cs:practice_pause_choice]
    cmp byte [cs:practice_pause_choice],3
    jb .redraw
    mov byte [cs:practice_pause_choice],0
    jmp .redraw
.confirm:
    test bl,0x90
    jz .loop
    cmp byte [cs:practice_pause_choice],0
    je .resume
    cmp byte [cs:practice_pause_choice],2
    je .restart
    jmp .quit
.restart:
    les di,[0x27fc]
    xor al,al
    cmp byte [es:di+22],4
    jne .stage_ready
    mov al,5
.stage_ready:
    mov [es:di+11],al
    mov [0x8f5],al
    mov dword [es:di+14],0
    mov word [es:di+18],0
    mov al,[es:di+25]
    mov [es:di+20],al
    mov al,[es:di+26]
    mov [es:di+21],al
    mov dword [cs:0x10e],0x6d0009e9
    mov dword [cs:0x112],0x006e6961
    mov byte [cs:0x13b],0x0e
    mov word [cs:0x13d],0x111
    mov byte [cs:network_result],2
    jmp .finish
.resume:
    mov byte [cs:network_result],0
    jmp .finish
.quit:
    mov byte [cs:network_result],1
%else
.loop:
    push word 1
    call far [cs:PAUSE_STATE+4]
    cmp byte [cs:PAUSE_STATE+1],0
    je .resume
    mov al,[cs:PAUSE_STATE+2]
    cmp al,[cs:practice_pause_choice]
    je .loop
    mov [cs:practice_pause_choice],al
    call practice_pause_draw
    jmp .loop
.resume:
    mov byte [cs:network_result],0
%endif
.finish:
    call practice_pause_screen_restore
    pop gs
    pop fs
    pop es
    pop ds
    popad
    popf
    xor ax,ax
    mov al,[cs:network_result]
    cmp al,2
    jne .return
    mov al,1
.return:
    ret
network_previous: dw 0
network_result_tag: db 'NPRST02!'
network_result: db 0
%include 'practice-pause.inc'
%include 'practice-text.inc'
