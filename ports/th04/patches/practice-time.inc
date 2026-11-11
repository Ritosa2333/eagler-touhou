; TH04 native phase timeout lock. Called before the native player update.
; Stage and phase are the game's own state, not a predicted HUD timer.
practice_time_lock:
    test byte [cs:practice_assists], 16
    jz .done
    cmp word [0xbcd0], 0
    je .mid
    cmp word [0xbcd0], 0x11c0
    je .mid
    cmp byte [0x53d9], 254
    jae .mid
    xor bx,bx
    mov bl,[0x5394]
    cmp bl,6
    ja .done
    mov dl,[0x53d9]
    mov dh,bl
    cmp bl,3
    jne .extra
    cmp byte [0x5398],1
    jne .extra
    mov dh,7
.extra:
    cmp bl,6
    jne .lookup
    cmp word [0xbcd0],0xc7da
    jne .lookup
    mov dh,8
.lookup:
    mov si,practice_time_caps
.cap_next:
    cmp byte [cs:si],0xff
    je .loop_table
    cmp [cs:si],dh
    jne .cap_skip
    cmp [cs:si+1],dl
    jne .cap_skip
    mov ax,[cs:si+2]
    sub ax,2
    cmp [0x53da],ax
    jl .loop_table
    mov word [0x53da],0
    jmp .loop_table
.cap_skip:
    add si,4
    jmp .cap_next
.loop_table:
    mov si,practice_time_loops
.loop_next:
    cmp byte [cs:si],0xff
    je .done
    cmp [cs:si],dh
    jne .loop_skip
    cmp [cs:si+1],dl
    jne .loop_skip
    mov al,[cs:si+2]
    cmp dh,2
    jne .limit_ready
    cmp dl,3
    jne .limit_ready
    mov al,[0x46e4]
    cmp al,4
    jae .limit_40
    inc al
    shl al,3
    jmp .limit_ready
.limit_40:
    mov al,40
.limit_ready:
    sub al,2
    jnz .counter
    inc al
.counter:
    mov si,0x53df
    cmp bl,0
    jne .extra_counter
    mov si,0xbced
    jmp .compare
.extra_counter:
    cmp bl,6
    jne .compare
    cmp dh,8
    je .compare
    mov si,0x46ae
.compare:
    cmp [si],al
    jb .done
    mov byte [si],0
    ret
.loop_skip:
    add si,3
    jmp .loop_next
.mid:
    cmp word [0x46b6],0
    je .done
    cmp word [0x46b6],0x11c0
    je .done
    cmp byte [0x53c5],254
    jae .done
    mov al,[0x5394]
    test al,al
    jnz .mid_stage1
    cmp byte [0x53c5],3
    jne .done
    cmp byte [0x4277],2
    jbe .done
    mov byte [0x4277],2
    ret
.mid_stage1:
    cmp al,1
    jne .mid_stage2
    mov si,0x4274
    mov al,15
    jmp .compare
.mid_stage2:
    cmp al,2
    jne .mid_stage3
    mov si,0x425a
    mov al,10
    jmp .compare
.mid_stage3:
    cmp al,3
    jne .done
    mov si,0x4287
    mov al,7
    jmp .compare
.done:
    ret
; group, phase, native frame limit
practice_time_caps:
    db 0,3
    dw 1501
    db 0,4
    dw 601
    db 1,3
    dw 2001
    db 1,5
    dw 700
    db 4,4
    dw 500
    db 4,7
    dw 500
    db 4,10
    dw 500
    db 4,13
    dw 288
    db 4,16
    dw 288
    db 4,17
    dw 1000
    db 5,16
    dw 2500
    db 6,4
    dw 192
    db 6,6
    dw 4000
    db 7,11
    dw 1000
    db 8,7
    dw 1500
    db 8,8
    dw 5000
    db 0xff
; group, phase, native loop limit
practice_time_loops:
    db 0,2,16, 1,2,11, 1,4,11, 2,3,40, 3,2,52
    db 4,2,4, 4,5,4, 4,8,4, 4,11,4, 4,14,4
    db 5,2,10, 5,4,10, 5,8,10, 5,12,10, 5,14,18
    db 6,2,36, 7,2,9, 7,4,17, 7,6,11, 7,8,10, 7,9,12
    db 8,2,22, 8,3,22, 8,4,22, 8,5,22
    db 0xff
