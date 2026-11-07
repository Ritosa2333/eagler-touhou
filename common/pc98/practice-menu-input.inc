; Guest-side menu navigation for TH02 and TH04. The caller supplies the
; original game's input masks before including this file.
;
; AX = current native input, BX = previous native input, CL = selected row,
; CH = number of rows (1..255). Return BX = current input, CL = selected row,
; AX = action: 0 none, 1 selection changed, 2 confirm, 3 cancel,
; 4 decrease value, 5 increase value. DX and flags are scratch.
; An opening caller must seed BX with the currently held input so the key
; that opened the menu cannot also activate an entry.
practice_menu_input:
    mov dx, ax
    not bx
    and bx, ax                  ; newly pressed keys only
    test bx, PRACTICE_CANCEL
    jnz .cancel
    test bx, PRACTICE_CONFIRM
    jnz .confirm
    test bx, PRACTICE_UP
    jnz .up
    test bx, PRACTICE_DOWN
    jnz .down
    test bx, PRACTICE_LEFT
    jnz .left
    test bx, PRACTICE_RIGHT
    jnz .right
    xor ax, ax
    jmp .done
.up:
    test cl, cl
    jnz .dec
    mov cl, ch
.dec:
    dec cl
    mov ax, 1
    jmp .done
.down:
    inc cl
    cmp cl, ch
    jb .selected
    xor cl, cl
.selected:
    mov ax, 1
    jmp .done
.confirm:
    mov ax, 2
    jmp .done
.cancel:
    mov ax, 3
    jmp .done
.left:
    mov ax, 4
    jmp .done
.right:
    mov ax, 5
.done:
    mov bx, dx
    ret
