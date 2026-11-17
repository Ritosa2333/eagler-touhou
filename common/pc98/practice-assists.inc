; State and edge-triggered controls for the compact guest trainer.
; AL = current PC-98 key group 1 (Backspace = 0x40),
; AH = current key group 12 (F1..F5 = 0x04..0x40),
; DL = current key group 2 (U = 0x40).
; The caller must poll once each game frame. Returns AL = 1 if the displayed
; state changed, 0 otherwise. All other registers are preserved.
; practice_assists bits: F1..F5 = 0..4, U = 5.
practice_assists_poll:
    push bx
    push cx
    push dx
    mov bx, ax
    mov cl, al
    not byte [cs:practice_keys_backspace]
    and cl, [cs:practice_keys_backspace]
    mov [cs:practice_keys_backspace], al
    mov [cs:practice_backspace_edge], cl
    mov cl, ah
    not byte [cs:practice_keys_functions]
    and cl, [cs:practice_keys_functions]
    mov [cs:practice_keys_functions], ah
    mov ch, dl
    not byte [cs:practice_keys_u]
    and ch, [cs:practice_keys_u]
    mov [cs:practice_keys_u], dl
    xor al, al
    test bl, 0x40
    jz .toggle_assists
    ; Toggle only on the rising edge, not while Backspace stays held.
    test byte [cs:practice_backspace_edge], 0x40
    jz .toggle_assists
    xor byte [cs:practice_assists_open], 1
    mov al, 1
.toggle_assists:
    cmp byte [cs:practice_assists_open], 0
    je .done
    shr cl, 2
    and cl, 0x1f
    test ch, 0x40
    jz .apply
    or cl, 0x20
.apply:
    test cl, cl
    jz .done
    xor [cs:practice_assists], cl
    mov al, 1
.done:
    pop dx
    pop cx
    pop bx
    ret
db 'P98AST01'
practice_keys_backspace: db 0
practice_keys_functions: db 0
practice_keys_u: db 0
practice_backspace_edge: db 0
practice_assists_open: db 0
practice_assists: db 0
