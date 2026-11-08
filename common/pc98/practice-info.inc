; Native replacement information panel on the right HUD, in text VRAM.
; Practice counters are maintained in MAIN. Visibility is a frontend input.
%ifdef PRACTICE_RESTART_TH02
%define INFO_ROWS 10
%else
%define INFO_ROWS 13
%endif
practice_info_reset:
    mov dword [cs:practice_info_frames],0
    mov word [cs:practice_info_misses],0
%ifdef PRACTICE_RESTART_TH02
    mov al,[0xbde]
%else
    les bx,[0xba86]
    mov al,[es:bx+50]
%endif
    mov [cs:practice_info_initial_bombs],al
    ret

practice_info_tick:
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:target_preparing],0
    jne .done
    cmp byte [cs:practice_outcome],0
    jne .done
    inc dword [cs:practice_info_frames]
%ifdef PRACTICE_RESTART_TH02
    movzx ax,byte [0xb2c]
    mov [cs:practice_info_values],ax
    movzx ax,byte [0xb2d]
    mov [cs:practice_info_values+2],ax
    mov al,[0xbde]
    movzx bx,byte [0xaa6]
    mov [cs:practice_info_values+8],bx
    mov bx,[0x2802]
    test bx,bx
    jns .rank_ready
    xor bx,bx
.rank_ready:
    mov [cs:practice_info_values+10],bx
    mov bx,[0x2538]
%else
    les bx,[0xba86]
    xor ax,ax
    mov al,[es:bx+11]
    test al,al
    jz .lives_ready
    dec ax
.lives_ready:
    mov [cs:practice_info_values],ax
    movzx ax,byte [es:bx+13]
    mov [cs:practice_info_values+2],ax
    mov al,[es:bx+50]
    movzx bx,byte [0x4664]
    mov [cs:practice_info_values+8],bx
    movzx bx,byte [0x5395]
    mov [cs:practice_info_values+10],bx
    mov bx,[0x3974]
%endif
    sub al,[cs:practice_info_initial_bombs]
    xor ah,ah
    mov [cs:practice_info_values+6],ax
    mov ax,[cs:practice_info_misses]
    mov [cs:practice_info_values+4],ax
    cmp byte [cs:practice_form_options+7],0
    jne .keys_ready
    xor bx,bx
.keys_ready:
    mov [cs:practice_info_values+14],bx
    mov eax,[cs:practice_info_frames]
    xor edx,edx
    mov ecx,56
    div ecx
    cmp eax,65535
    jbe .time_ready
    mov ax,65535
.time_ready:
    mov [cs:practice_info_values+12],ax
.done:
    ret
practice_info_frames: dd 0
practice_info_misses: dw 0
practice_info_initial_bombs: db 0

; The right-hand VRAM also stores background tiles. Save the current page
; immediately before masking and restore it before the next game update.
practice_info_overlay_restore:
    cmp byte [cs:practice_info_overlay_active],0
    je .done
    pushf
    pushad
    push ds
    push es
    cld
    mov al,[PRACTICE_PAGE_BACK]
    mov [cs:practice_info_return_page],al
    mov al,[cs:practice_info_overlay_page]
    out 0xa6,al
    xor al,al
    out 0x7c,al
    mov ds,[cs:practice_info_overlay_buffer]
    xor si,si
    xor bx,bx
.plane:
    mov es,[cs:practice_info_planes+bx]
    mov di,[cs:practice_info_overlay_offset]
    mov bp,INFO_ROWS*16
.row:
    mov cx,28
    rep movsb
    add di,52
    cmp di,32000
    jb .next
    sub di,32000
.next:
    dec bp
    jnz .row
    add bx,2
    cmp bx,8
    jb .plane
    mov al,[cs:practice_info_return_page]
    out 0xa6,al
    mov byte [cs:practice_info_overlay_active],0
    pop es
    pop ds
    popad
    popf
.done:
    ret

practice_info_overlay_mask:
    cmp byte [cs:practice_info_enabled],0
    je .done
    cmp byte [cs:practice_info_drawn],0
    je .done
    cmp byte [cs:practice_info_overlay_active],0
    jne .done
    pushf
    pushad
    push ds
    push es
    cld
    cmp word [cs:practice_info_overlay_buffer],0
    jne .allocated
    mov bx,INFO_ROWS*112
    mov ah,0x48
    int 0x21
    jc .exit
    mov [cs:practice_info_overlay_buffer],ax
.allocated:
    mov al,[PRACTICE_PAGE_BACK]
    mov [cs:practice_info_overlay_page],al
    mov ax,[cs:practice_form_scroll]
    add ax,176
    xor dx,dx
    mov cx,400
    div cx
    mov ax,dx
    mov cx,80
    mul cx
    add ax,52
    mov [cs:practice_info_overlay_offset],ax
    mov es,[cs:practice_info_overlay_buffer]
    xor di,di
    xor al,al
    out 0x7c,al
    xor bx,bx
.plane:
    mov ds,[cs:practice_info_planes+bx]
    mov si,[cs:practice_info_overlay_offset]
    mov bp,INFO_ROWS*16
.row:
    mov cx,28
.cell:
    lodsb
    stosb
    mov byte [si-1],0
    loop .cell
    add si,52
    cmp si,32000
    jb .next
    sub si,32000
.next:
    dec bp
    jnz .row
    add bx,2
    cmp bx,8
    jb .plane
    mov byte [cs:practice_info_overlay_active],1
.exit:
    pop es
    pop ds
    popad
    popf
.done:
    ret
practice_info_planes: dw 0xa800,0xb000,0xb800,0xe000
practice_info_overlay_buffer: dw 0
practice_info_overlay_offset: dw 0
practice_info_overlay_active: db 0
practice_info_overlay_page: db 0
practice_info_return_page: db 0

practice_info_draw:
    push ds
    push es
    push cs
    pop ds
    mov ax,0xa000
    mov es,ax
    cmp byte [cs:practice_info_enabled],0
    jne .show
    cmp byte [cs:practice_info_drawn],0
    je .done
    mov di,11*160+52*2
    mov si,practice_info_saved
    mov dx,INFO_ROWS
.restore_row:
    mov cx,28
.restore_cell:
    mov ax,[cs:si]
    mov [es:di],ax
    mov ax,[cs:si+2]
    mov [es:di+0x2000],ax
    add si,4
    add di,2
    loop .restore_cell
    add di,104
    dec dx
    jnz .restore_row
    mov byte [cs:practice_info_drawn],0
    jmp .done
.show:
    cmp byte [cs:practice_info_drawn],0
    jne .paint
    mov di,11*160+52*2
    mov si,practice_info_saved
    mov dx,INFO_ROWS
.save_row:
    mov cx,28
.save_cell:
    mov ax,[es:di]
    mov [cs:si],ax
    mov ax,[es:di+0x2000]
    mov [cs:si+2],ax
    add si,4
    add di,2
    loop .save_cell
    add di,104
    dec dx
    jnz .save_row
    mov byte [cs:practice_info_drawn],1
.paint:
    mov di,11*160+52*2
    mov bp,practice_info_labels
    xor bx,bx
.row:
    push bx
    push di
    mov cx,28
.background:
    mov word [es:di],0x20
    mov word [es:di+0x2000],0x05
    add di,2
    loop .background
    pop di
%ifdef PRACTICE_RESTART_TH02
    add di,2*2
%else
    add di,4*2
%endif
    mov si,[cs:bp]
    mov bl,0xe1
    call practice_text_write
    pop bx
    cmp bx,16
    jae .next_row
    mov ax,[cs:practice_info_values+bx]
    push bx
    push di
    add di,15*2
    mov cx,5
.digit:
    xor dx,dx
    push cx
    mov cx,10
    div cx
    pop cx
    add dl,'0'
    mov [es:di],dl
    mov byte [es:di+1],0
    mov word [es:di+0x2000],0xe1
    sub di,2
    loop .digit
    pop di
    pop bx
.next_row:
    add bx,2
    add bp,2
%ifdef PRACTICE_RESTART_TH02
    sub di,2*2
%else
    sub di,4*2
%endif
    add di,160
    cmp bx,INFO_ROWS*2
    jb .row
.done:
    pop es
    pop ds
    ret
practice_info_enabled: db 0
practice_info_drawn: db 0
practice_info_values: times 12 dw 0
practice_info_labels: dw .lives,.stock,.miss,.bomb,.power,.rank,.time,.keys,.blank,.blank,.blank,.blank,.blank
.lives: db 'LIVES',0
.stock: db 'BOMBS',0
.miss: db 'MISS',0
.bomb: db 'BOMB',0
.power: db 'POWER',0
.rank: db 'RANK',0
.time: db 'TIME',0
.keys: db 'KEYS',0
.blank: db ' ',0
practice_info_saved: times 28*INFO_ROWS*4 db 0
