bits 16
cpu 386
org CH_ORG
counts: times 3 dd 0
start:
    call CH_FLUSH
    pushad
    push es
    mov ax,0xa000
    mov es,ax
    xor bp,bp
.player:
    imul di,bp,CH_STEP*160
    add di,CH_ROW*160+CH_COL*2
    mov cx,CH_WIDTH
    mov ax,0x0520
.clear:
    push cx
    call CH_CHAR
    pop cx
    loop .clear
    sub di,CH_WIDTH*2
    call .number
    inc bp
%if CH_GAME = 2
    cmp bp,[cs:CH_COUNT]
%else
    movzx ax,byte [cs:CH_COUNT]
    cmp bp,ax
%endif
    jb .player
    pop es
    popad
    ret
.number:
    push eax
    push ebx
    push ecx
    push edx
    mov bx,bp
    shl bx,2
    mov eax,[cs:counts+bx]
    test eax,eax
    jns .digits
    neg eax
    push eax
    mov ax,0xe12d
    call CH_CHAR
    pop eax
.digits:
    xor cx,cx
    mov ebx,10
.next:
    xor edx,edx
    div ebx
    add dl,'0'
    push dx
    inc cx
    test eax,eax
    jnz .next
.print:
    pop ax
    mov ah,0xe1
    push cx
    call CH_CHAR
    pop cx
    loop .print
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret
