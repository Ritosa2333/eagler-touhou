; Clear preparation entities after the Boss update and before rendering.
; Preserve Marisa's four native bits when requested by the selected target.
; Counts are published for diagnostics in this exact pool order.
practice_cleanup:
    push es
    mov byte [cs:practice_cleanup_pending],0
    mov bx,practice_cleanup_counts
    mov si,0x5a22
    mov cx,440
    mov dx,26
    call .pool
    mov si,0x42f0
    mov cx,2
    mov dx,24
    call .pool
    mov si,0x9292
    mov cx,16
    mov dx,42
    call .pool
    mov si,0xaf34
    mov cx,32
    mov dx,20
    call .pool
    mov si,0x9594
    mov cx,16
    mov dx,10
    call .pool
    mov si,0xb204
    mov cx,32
    cmp byte [cs:practice_cleanup_keep_bits],0
    je .custom
    add si,4*26
    sub cx,4
.custom:
    mov dx,26
    call .pool
    mov byte [0xbcb9],0
    mov byte [0xbcba],0
    mov byte [cs:practice_redraw_pending],2
    call .invalidate
    pop es
    ret
.invalidate:
    push ds
    pop es
    mov di,0x4700
    mov cx,0x640
    mov al,1
    cld
    rep stosb
    ret
.pool:
    mov word [cs:bx],0
.slot:
    cmp byte [si],0
    je .next
    inc word [cs:bx]
    mov byte [si],0
.next:
    add si,dx
    loop .slot
    add bx,2
    ret
practice_cleanup_keep_bits: db 0
practice_cleanup_pending: db 0
practice_redraw_pending: db 0
practice_cleanup_counts: times 6 dw 0

practice_redraw_frame:
    cmp byte [cs:practice_redraw_pending],0
    je .done
    dec byte [cs:practice_redraw_pending]
    push es
    call practice_cleanup.invalidate
    pop es
.done:
    ret
