; TH02 direct browser launch. Runs after the original ZUN resident and PMD.
; Layout: ReC98 th02/th02.inc mikoconfig_t; HUUMA.CFG resident segment at +5.
bits 16
cpu 386
org 0x100
    jmp start
    db 'TH02WEBSTARTv1!'
config: db 1,0,1,2,3,0 ; schema, stage (0..5), rank (0..3), spare lives, bombs, P1 shot
start:
    mov dx,cfg_name
    mov ax,0x3d00
    int 0x21
    jc failure
    mov bx,ax
    mov dx,cfg
    mov cx,8
    mov ah,0x3f
    int 0x21
    pushf
    push ax
    mov ah,0x3e
    int 0x21
    pop ax
    popf
    jc failure
    cmp ax,8
    jne failure
    mov ax,[cfg+5]
    test ax,ax
    jz failure
    mov es,ax
    xor di,di
    mov si,resident_id
    mov cx,11
    cld
    repe cmpsb
    jne failure
    mov di,11
    mov cx,37
    xor ax,ax
    rep stosb
    mov al,[config+1]
    mov [es:11],al
    mov al,[config+2]
    cmp byte [config+1],5
    jne .rank
    mov al,4 ; Native RANK_EXTRA
.rank:
    mov [es:22],al
    mov al,[config+3]
    mov [es:21],al
    mov [es:26],al
    mov al,[config+4]
    mov [es:20],al
    mov [es:25],al
    mov byte [es:24],1 ; Original FM driver, including effects
    mov al,[config+5]
    mov [es:38],al
    mov dword [es:28],0x4a3d ; Fixed native RNG seed, independent of load time
    ; MAIN inherits visible graphics / function key state from OP normally.
    mov al,0x41
    out 0x6a,al
    mov al,1
    out 0x6a,al
    mov ah,0x40
    int 0x18
    mov dx,screen_setup
    mov ah,9
    int 0x21
    mov ax,0x4c00
    int 0x21
failure:
    mov dx,error
    mov ah,9
    int 0x21
    mov ax,0x4c01
    int 0x21
cfg_name: db 'HUUMA.CFG',0
resident_id: db 'MIKOConfig',0
cfg: times 8 db 0
screen_setup: db 27,'[>1h',27,'[>5h','$'
error: db 'TH02: resident initialization failed. Reload the web page.',13,10,'$'
