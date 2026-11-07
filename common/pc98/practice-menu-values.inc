; Guest-side unsigned byte value editing. DS:SI points at the value, while
; DL and DH contain its inclusive minimum and maximum. AX is the action from
; practice_menu_input (4 = decrease, 5 = increase). Invalid existing values
; are clamped before applying the action. Returns AL = resulting value.
; Other actions leave a valid value unchanged. Flags are scratch.
practice_menu_value:
    mov bl, [si]
    cmp bl, dl
    jae .check_max
    mov bl, dl
.check_max:
    cmp bl, dh
    jbe .edit
    mov bl, dh
.edit:
    cmp ax, 4
    je .decrease
    cmp ax, 5
    je .increase
    jmp .store
.decrease:
    cmp bl, dl
    jne .dec
    mov bl, dh
    jmp .store
.dec:
    dec bl
    jmp .store
.increase:
    cmp bl, dh
    jne .inc
    mov bl, dl
    jmp .store
.inc:
    inc bl
.store:
    mov [si], bl
    mov al, bl
    ret
