bits 16
cpu 386
org 0x100
    jmp start
mailbox: db 'PC98LIVESFX0001!'
resident_segment: dw 0
effect: db 0
state: db 0
filename: db 'SOUND.EFC',0
start:
    mov ax,0x3d00
    mov dx,filename
    int 0x21
    jc fail
    mov bx,ax
    push ds
    mov ax,0x0b00
    int 0x60
    mov cx,0xffff
    mov ah,0x3f
    int 0x21
    pop ds
    jc fail
    mov ah,0x3e
    int 0x21
    mov [resident_segment],cs
    mov byte [state],2
.wait:
    cmp byte [state],1
    jne .wait
    mov al,[effect]
    mov ah,0x0c
    int 0x60
    mov byte [state],2
    jmp .wait
fail:
    mov ax,0x4c01
    int 0x21
