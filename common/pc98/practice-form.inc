; Shared native practice form. The owner saves/restores its occupied text cells
; and supplies the game-specific field maxima before opening the form.
; Eight byte fields: stage, character, difficulty, entry, phase, lives, bombs,
; power. Rows 8 and 9 start or cancel. Input uses the shared edge detector.
%define PRACTICE_UP 1
%define PRACTICE_DOWN 2
%define PRACTICE_LEFT 4
%define PRACTICE_RIGHT 8
%define PRACTICE_CONFIRM 16
%define PRACTICE_CANCEL 32
practice_form_step:
    push ds
    push cs
    pop ds
    mov bx,[cs:practice_form_keys]
    mov cl,[cs:practice_form_choice]
    mov ch,10
    call practice_menu_input
    mov [cs:practice_form_keys],bx
    mov [cs:practice_form_choice],cl
    cmp ax,3
    je .cancel
    cmp ax,2
    jne .edit
    cmp cl,8
    je .start
    cmp cl,9
    je .cancel
    jmp .changed
.edit:
    cmp cl,8
    jae .changed
    cmp ax,4
    jb .changed
    cmp ax,5
    ja .changed
    xor bx,bx
    mov bl,cl
    mov si,practice_form_values
    add si,bx
    mov dh,[cs:practice_form_max+bx]
    xor dl,dl
    call practice_menu_value
    mov ax,1
    jmp .done
.start:
    mov ax,2
    jmp .done
.cancel:
    mov ax,3
    jmp .done
.changed:
.done:
    pop ds
    ret

; ES = A000h. Draw ten rows in the native PC-98 text plane.
practice_form_draw:
    push ds
    push cs
    pop ds
    mov di,5*160+20*2
    mov bp,practice_form_labels
    xor dx,dx
.row:
    mov si,[cs:bp]
    mov bl,0xe1
    cmp dl,[cs:practice_form_choice]
    jne .label
    mov bl,0xe5
.label:
    call practice_text_write
    cmp dl,8
    jae .next
    xor bx,bx
    mov bl,dl
    xor ax,ax
    mov al,[cs:practice_form_values+bx]
    mov cl,100
    div cl
    mov bx,di
    add bx,22*2
    add al,'0'
    mov [es:bx],al
    mov al,ah
    xor ah,ah
    mov cl,10
    div cl
    add ax,0x3030
    mov [es:bx+2],al
    mov [es:bx+4],ah
    mov byte [es:bx+1],0
    mov byte [es:bx+3],0
    mov byte [es:bx+5],0
    mov byte [es:bx+0x2000],0xe1
    mov byte [es:bx+0x2002],0xe1
    mov byte [es:bx+0x2004],0xe1
.next:
    add bp,2
    add di,160
    inc dl
    cmp dl,10
    jb .row
    pop ds
    ret
practice_form_choice: db 0
practice_form_keys: dw 0
practice_form_values: db 0,0,1,0,0,3,2,80
practice_form_max: db 5,2,4,2,9,5,5,80
practice_form_labels:
    dw .stage,.character,.difficulty,.entry,.phase,.lives,.bombs,.power,.start,.cancel
.stage: db 'STAGE                  ',0
.character: db 'CHARACTER              ',0
.difficulty: db 'DIFFICULTY             ',0
.entry: db 'ENTRY                  ',0
.phase: db 'PHASE                  ',0
.lives: db 'LIVES                  ',0
.bombs: db 'BOMBS                  ',0
.power: db 'POWER                  ',0
.start: db 'START PRACTICE         ',0
.cancel: db 'CANCEL                 ',0
%include 'practice-menu-input.inc'
%include 'practice-menu-values.inc'
