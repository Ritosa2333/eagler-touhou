; Wrap only player_move's normal-input call, not knockback position updates.
; Duplicate the near Pascal argument: the original and wrapper each pop one.
move_hook:
    push bp
    mov bp,sp
    push word [bp+4]
    call ORIGINAL(0xd71e)
    pushf
    pushad
    movzx ebx,byte [0x6590]
    imul bx,bx,6
%ifdef SOLO_ASSIST
    cmp byte [0x6590],0
    jne .keyboard
%endif
    cmp byte [cs:phase],1
    jne .keyboard
    test word [cs:touch_state+bx],1
    jz .keyboard
    mov si,[0x6588]
    cmp byte [si+0x1f],0
    jne .blocked
    cmp byte [si+0x11],0
    jne .blocked
    cmp byte [si+0x15],0
    jne .blocked
    call touch_move
    popad
    popf
    mov al,2                   ; position applied; skip native key velocity
    pop bp
    ret 2
.blocked:
    mov word [cs:touch_state+bx],0
    mov dword [cs:touch_state+bx+2],0
.keyboard:
    cmp al,1                   ; preserve MOVE_INVALID / MOVE_NOINPUT
    jne .done
    mov cl,[0x6590]            ; pid.current, 0 or 1 at this call site
    mov al,1
    shl al,cl
    test [cs:focus_active],al
    jz .done
    sar byte [0x6586],1        ; signed subpixel velocity, both axes
    sar byte [0x6587],1
.done:
    popad
    popf
    pop bp
    ret 2

; Bound the drag to the native field, then limit its vector to the current
; character's speed. Keep the remainder until the next native movement tick.
; Same-size native touch replacement. Pending deltas stay in unfocused units;
; focus scales requested distance AND maximum speed, as in TH06 Player.cpp.
; Bounds use 16-bit arithmetic: positions + bounded signed wire deltas fit.
touch_move:
    
    
    mov si,[cs:touch_state+bx+2]
    mov di,[cs:touch_state+bx+4]
    xor cx,cx
    test word [cs:touch_state+bx],2
    jnz .ratio
    movzx ax,byte [0x6590]
    bt word [cs:focus_active],ax
    jnc .ratio
    inc cx
.ratio:
    sar si,cl
    sar di,cl
    mov bp,[0x6588]
    mov ax,[ds:bp]
    add si,ax
    cmp si,0x80
    jge .xmax
    mov si,0x80
.xmax:
    cmp si,0x1180
    jle .xready
    mov si,0x1180
.xready:
    sub si,ax
    mov ax,[ds:bp+2]
    add di,ax
    cmp di,0x180
    jge .ymax
    mov di,0x180
.ymax:
    cmp di,0x1600
    jle .yready
    mov di,0x1600
.yready:
    sub di,ax
    ; Keep the bounded request in raw units; guest precision is 1/16 pixel.
    mov dx,si
    shl dx,cl
    mov [cs:touch_state+bx+2],dx
    mov dx,di
    shl dx,cl
    mov [cs:touch_state+bx+4],dx
    push cx
    test word [cs:touch_state+bx],2
    jnz .apply
    movsx esi,si
    movsx edi,di
    mov eax,esi
    imul eax,eax
    mov edx,edi
    imul edx,edx
    add eax,edx
    xchg eax,ecx
    xor eax,eax
    mov ebp,0x4000000
.sqrt:
    lea edx,[eax+ebp]
    shr eax,1
    cmp ecx,edx
    jb .next
    sub ecx,edx
    add eax,ebp
.next:
    shr ebp,2
    jnz .sqrt
    neg ecx
    adc eax,0
    movzx ebp,byte [0x6582]
    pop cx
    push cx
    shr ebp,cl
    xchg eax,ecx
    cmp ecx,ebp
    jbe .apply
    mov eax,esi
    imul eax,ebp
    cdq
    idiv ecx
    mov esi,eax
    mov eax,edi
    imul eax,ebp
    cdq
    idiv ecx
    mov edi,eax
.apply:
    pop cx
    mov bp,[0x6588]
    add [ds:bp],si
    add [ds:bp+2],di
    shl si,cl
    shl di,cl
    sub [cs:touch_state+bx+2],si
    sub [cs:touch_state+bx+4],di
.done:
    ret
    times 268-($-touch_move) nop
