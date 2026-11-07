; Replace the retired original pause routine in place. No DOS heap allocation
; and no executable growth: the two overwritten text rows live on the stack.
bits 16
cpu 386
%include 'net-pause-config.inc'
org PATCH_ORG
    pushad
    push es
    push fs
    sub sp,144
    mov bp,sp
    push ss
    pop fs
    mov ax,0xa000
    mov es,ax
    mov di,8*160+22*2
    mov bx,bp
    mov dx,3
.save_row:
    mov cx,12
.save_cell:
    mov ax,[es:di]
    mov [fs:bx],ax
    mov ax,[es:di+0x2000]
    mov [fs:bx+2],ax
    add bx,4
    add di,2
    loop .save_cell
    add di,320-24
    dec dx
    jnz .save_row
.draw:
    mov di,8*160+22*2
    mov si,labels
    xor dx,dx
.draw_row:
    mov cx,12
    mov ah,0xe1
    cmp dl,[cs:PAUSE_STATE+2]
    jne .draw_cell
    mov ah,0xe5
.draw_cell:
    mov al,[cs:si]
    mov [es:di],al
    mov byte [es:di+1],0
    mov [es:di+0x2000],ah
    mov byte [es:di+0x2001],0
    inc si
    add di,2
    loop .draw_cell
    add di,320-24
    inc dx
    cmp dl,3
    jb .draw_row
    mov bl,[cs:PAUSE_STATE+2]
.wait:
    push word 1
    call far [cs:PAUSE_STATE+4]
    cmp byte [cs:PAUSE_STATE+1],0
    je .restore
    cmp bl,[cs:PAUSE_STATE+2]
    je .wait
    jmp .draw
.restore:
    mov ax,0xa000
    mov es,ax
    mov di,8*160+22*2
    mov bx,bp
    mov si,labels
    mov dx,3
.restore_row:
    mov cx,12
.restore_cell:
    mov al,[cs:si]
    cmp [es:di],al
    jne .next_cell
    mov ax,[fs:bx]
    mov [es:di],ax
    mov ax,[fs:bx+2]
    mov [es:di+0x2000],ax
.next_cell:
    inc si
    add bx,4
    add di,2
    loop .restore_cell
    add di,320-24
    dec dx
    jnz .restore_row
    add sp,144
    pop fs
    pop es
    popad
    xor ax,ax
    ret
labels: db '   RESUME   ','    QUIT    ','  RESTART   '
