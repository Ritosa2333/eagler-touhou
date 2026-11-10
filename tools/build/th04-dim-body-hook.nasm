bits 16
%ifndef HOOK_ORIGIN
%define HOOK_ORIGIN 0xb920
%endif
org HOOK_ORIGIN

pushad
mov ax, [0x464e]
sar ax, 4
add ax, 16
mov [cs:0x9a32], ax
mov ax, [0x4650]
sub ax, 128
push ax
call 0x1120
mov [cs:0x9a34], ax
mov ax, 1
cmp word [0x4656], 0
je .frame
jl .left
mov ax, 3
jmp .frame
.left:
inc ax
.frame:
mov [cs:0x9a2e], ax
movzx ax, byte [cs:0x9398]
mov [cs:0x9a30], ax
popad
call 0x610d
mov dword [cs:0x9a2e], 0
ret

times 80-($-$$) db 0
; Preserve the game's rank selection and give the original-color player's
; projectile writes a seat-specific native palette tag.
push bx
call 0x9878
pop bx
test al, al
jnz .done
push ax
mov word [cs:0x9a2e], 13
mov [cs:0x9a30], bx
pop ax
.done:
ret

times 112-($-$$) db 0
; This branch is reached only after a successful life transfer.
call 0x8bb5
push word 7
call far [cs:0x8cf1]
ret

times 128-($-$$) db 0
; Score Extend adds a reserve to each eligible slot, including ghosts.
pushad
call 0x8b79
mov si, 0x95a5
xor dx, dx
movzx cx, byte [cs:0x95a2]
.seat:
cmp byte [cs:si+6], 2
je .next
cmp byte [cs:si+4], 100
jae .next
inc byte [cs:si+4]
.next:
inc dx
add si, 16
loop .seat
call 0x8bb5
popad
ret
