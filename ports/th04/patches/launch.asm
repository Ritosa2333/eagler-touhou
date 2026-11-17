; Runs after ZUN -S and the PMD driver. No OP.EXE/menu automation involved.
bits 16
cpu 386
org 0x100
    jmp start
    db 'TH04WEBSTARTv1!'
config: db 2,1,3,2,0,0,1,0,0,0,2 ; schema, rank, lives, bombs, P1/P2/P3 char/shot, count
start:
%ifdef LOCAL_BGM
    cmp byte [0x80],3
    jb .normal_launch
    cmp word [0x82],0x522f ; /R, used only by the private LAN GAME.BAT
    jne .normal_launch
    call remove_local_bgm
    mov ax,0x4c00
    int 0x21
.normal_launch:
%endif
    mov dx,cfg_name
    mov ax,0x3d00
    int 0x21
    jc failure
    mov bx,ax
    mov dx,cfg
    mov cx,10
    mov ah,0x3f
    int 0x21
    pushf
    push ax
    mov ah,0x3e
    int 0x21
    pop ax
    popf
    jc failure
    cmp ax,10
    jne failure
    mov ax,[cfg+6]
    test ax,ax
    jz failure
    mov es,ax
    xor di,di
    mov si,resident_id
    mov cx,11
    cld
    repe cmpsb
    jne failure
    ; Start a fresh run; preserve the resident identifier.
    mov di,11
    mov cx,63
    xor ax,ax
    rep stosb
    mov al,[config+1]
    mov [es:15],al
    mov al,[config+2]
    mov [es:11],al
    mov [es:12],al
    mov [es:58],al
    mov al,[config+3]
    mov [es:13],al
    mov [es:14],al
    mov [es:59],al
    mov al,[cfg+3]
    mov [es:16],al
    mov al,[cfg+4]
    mov [es:24],al
    mov al,[config+4]
    add al,'0'
    mov [es:18],al
    xor al,al
    cmp byte [config+1],4 ; RANK_EXTRA uses the original stage 6 entry.
    jne .stage
    mov al,6
.stage:
    mov [es:17],al
    add al,'0'
    mov [es:19],al
    mov al,[config+5]
    mov [es:25],al
    mov dword [es:20],0x4a3d
    mov byte [es:72],1
    ; MAIN normally inherits visible graphics and hidden function-key labels
    ; from OP. Initialize that hardware state when bypassing OP entirely.
    mov al,0x41
    out 0x6a,al
    mov al,1
    out 0x6a,al
    mov ah,0x40
    int 0x18
    mov dx,screen_setup
    mov ah,9
    int 0x21
%ifdef LOCAL_BGM
    call install_local_bgm
    jc failure
    mov dx,(resident_end-$$+0x100+15)/16
    mov ax,0x3100
    int 0x21
%endif
    mov ax,0x4c00
    int 0x21
failure:
    mov dx,error
    mov ah,9
    int 0x21
    mov ax,0x4c01
    int 0x21
cfg_name: db 'MIKO.CFG',0
resident_id: db 'HUMAConfig',0
cfg: times 10 db 0
screen_setup: db 27,'[>1h',27,'[>5h','$'
error: db 'TH04: resident initialization failed. Reload the web page.',13,10,'$'
%ifdef LOCAL_BGM
%include "patches/local-bgm.asm"
resident_end:
%endif
