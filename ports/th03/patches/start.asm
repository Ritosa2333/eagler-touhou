; Runs after the original GAME.BAT installs the resident and sound driver.
; OP's native return-from-VS branch goes straight to two-player selection.
bits 16
cpu 386
org 0x100
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
    cmp byte [cfg+2],3
    ja failure
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
    ; Initialize only the verified fields through demo_num, retaining the ID.
    mov di,11
    mov cx,47
    xor ax,ax
    rep stosb
    mov al,[cfg+2]
    mov [es:0x0b],al           ; rank
    mov byte [es:0x0c],1      ; optional paletted Reimu
    mov byte [es:0x0d],5      ; optional paletted Marisa
    mov dword [es:0x10],0x4a3d
    mov al,[cfg]
    mov [es:0x15],al           ; original sound setting
    mov byte [es:0x16],0      ; keyboard vs keyboard
    mov byte [es:0x28],0x81   ; GM_VS_1P_2P; OP skips title and VS mode menu
    mov ax,0x4c00
    int 0x21
failure:
    mov dx,error
    mov ah,9
    int 0x21
    mov ax,0x4c01
    int 0x21
cfg_name: db 'YUME.CFG',0
resident_id: db 'YUMEConfig',0
cfg: times 8 db 0
error: db 'TH03: local VS initialization failed. Reload the web page.',13,10,'$'
