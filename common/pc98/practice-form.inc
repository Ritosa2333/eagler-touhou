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
    ; Held horizontal keys repeat after a short delay. A fresh press still
    ; moves by one, allowing exact values as well as practical large edits.
    mov dx,ax
    and dx,12
    jz .repeat_reset
    cmp dx,12
    je .repeat_reset
    cmp dx,[cs:practice_form_repeat_key]
    jne .repeat_new
    cmp byte [cs:practice_form_repeat_ticks],24
    jb .repeat_count
    inc byte [cs:practice_form_repeat_ticks]
    test byte [cs:practice_form_repeat_ticks],3
    jnz .repeat_ready
    not dx
    and bx,dx
    jmp .repeat_ready
.repeat_count:
    inc byte [cs:practice_form_repeat_ticks]
    jmp .repeat_ready
.repeat_new:
    mov [cs:practice_form_repeat_key],dx
    mov byte [cs:practice_form_repeat_ticks],0
    jmp .repeat_ready
.repeat_reset:
    mov word [cs:practice_form_repeat_key],0
    mov byte [cs:practice_form_repeat_ticks],0
.repeat_ready:
    mov cl,[cs:practice_form_choice]
    mov ch,13
    cmp byte [cs:practice_form_options_page],0
    je .input_ready
    mov ch,12
.input_ready:
    call practice_menu_input
    mov [cs:practice_form_keys],bx
    mov [cs:practice_form_choice],cl
    cmp byte [cs:practice_form_options_page],0
    jne .options_page
    cmp ax,3
    je .cancel
    cmp ax,2
    jne .edit
    cmp cl,10
    jne .confirm_main
    ; Left/right still edits HP; confirmation opens the options page only
    ; through the dedicated OPTIONS row below.
.confirm_main:
    cmp cl,11
    je .start
    cmp cl,12
    je .options_open
    jmp .changed
.edit:
    cmp cl,11
    jae .changed
    cmp ax,4
    jb .changed
    cmp ax,5
    ja .changed
    cmp cl,10
    je .edit_hp
    cmp cl,4
    je .edit_phase
    push cx
    xor bx,bx
    mov bl,cl
    mov si,practice_form_values
    add si,bx
    mov dh,[cs:practice_form_max+bx]
    mov dl,[cs:practice_form_min+bx]
    call practice_menu_value
    pop cx
    cmp cl,0
    je .stage_changed
    cmp cl,2
    jne .dependencies
    cmp byte [cs:practice_form_values+2],4
    jne .normal_difficulty
    mov byte [cs:practice_form_values],FORM_STAGE_MAX
    jmp .dependencies
.normal_difficulty:
    cmp byte [cs:practice_form_values],FORM_STAGE_MAX
    jne .dependencies
    mov byte [cs:practice_form_values],0
    jmp .dependencies
.stage_changed:
    cmp byte [cs:practice_form_values],FORM_STAGE_MAX
    jne .normal_stage
    mov byte [cs:practice_form_values+2],4
    jmp .dependencies
.normal_stage:
    cmp byte [cs:practice_form_values+2],4
    jne .dependencies
    mov byte [cs:practice_form_values+2],1
.dependencies:
    cmp byte [cs:practice_form_values+3],1
    jne .entry_valid
%ifdef FORM_TH02
    cmp byte [cs:practice_form_values],4
    jne .entry_valid
%else
    cmp byte [cs:practice_form_values],3
    jbe .entry_valid
%endif
    mov byte [cs:practice_form_values+3],2
.entry_valid:
    cmp cl,0
    je .reset_dependent
    cmp cl,1
    je .reset_dependent
    cmp cl,3
    je .reset_dependent
    cmp cl,8
    jne .edited
.reset_dependent:
    mov byte [cs:practice_form_values+4],0
    mov word [cs:practice_form_hp],0
.edited:
    mov ax,1
    jmp .done
.edit_phase:
    push ax
    call practice_form_phase_mask
    pop ax
    mov bl,[cs:practice_form_values+4]
.next_phase:
    cmp ax,4
    jne .phase_up
    test bl,bl
    jnz .phase_dec
    mov bl,FORM_PHASE_MAX+1
.phase_dec:
    dec bl
    jmp .phase_test
.phase_up:
    inc bl
    cmp bl,FORM_PHASE_MAX+1
    jb .phase_test
    xor bl,bl
.phase_test:
    movzx ecx,bl
    bt edx,ecx
    jnc .next_phase
    mov [cs:practice_form_values+4],bl
    mov word [cs:practice_form_hp],0
    mov ax,1
    jmp .done
.edit_hp:
    call practice_form_hp_limit
    cmp ax,4
    jne .hp_increase
    cmp word [cs:practice_form_hp],0
    jne .hp_decrease
    mov [cs:practice_form_hp],dx
    jmp .hp_changed
.hp_decrease:
    dec word [cs:practice_form_hp]
    jmp .hp_changed
.hp_increase:
    cmp [cs:practice_form_hp],dx
    jb .hp_add
    mov word [cs:practice_form_hp],0
    jmp .hp_changed
.hp_add:
    inc word [cs:practice_form_hp]
.hp_changed:
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
.options_open:
    mov byte [cs:practice_form_options_page],1
    mov byte [cs:practice_form_choice],0
    mov ax,1
    jmp .done
.options_page:
    cmp ax,3
    je .options_close
    cmp cl,11
    jne .options_edit
    cmp ax,2
    je .options_close
    jmp .changed
.options_edit:
    cmp ax,2
    je .option_toggle
    cmp ax,4
    je .option_toggle
    cmp ax,5
    jne .changed
.option_toggle:
    xor bx,bx
    mov bl,cl
    cmp bl,10
    jne .option_bool
    inc byte [cs:practice_form_options+bx]
    cmp byte [cs:practice_form_options+bx],6
    jb .option_done
    mov byte [cs:practice_form_options+bx],0
    jmp .option_done
.option_bool:
    xor byte [cs:practice_form_options+bx],1
.option_done:
    mov ax,1
    jmp .done
.options_close:
    mov byte [cs:practice_form_options_page],0
    mov byte [cs:practice_form_choice],12
    mov ax,1
    jmp .done

; Called inside MAIN. The regular frame wait keeps the host input bridge alive.
practice_form:
    mov byte [cs:practice_form_options_page],0
    mov byte [cs:practice_form_choice],0
    mov word [cs:practice_form_keys],0xffff
    mov word [cs:practice_form_repeat_key],0
    mov byte [cs:practice_form_repeat_ticks],0
    call practice_form_graphics_save
    call practice_form_save
    call practice_form_draw_screen
    mov byte [cs:practice_form_open],2
.loop:
    inc dword [cs:SOLO_MAILBOX+16]
    PRACTICE_PAUSE_WAIT
    mov ax,[cs:practice_form_input]
    call practice_form_step
    cmp ax,2
    je .start
    cmp ax,3
    je .close_cancel
    test ax,ax
    jz .loop
    call practice_form_draw_screen
    jmp .loop
.start:
    mov byte [cs:practice_form_request],1
    jmp .close
.close_cancel:
    mov byte [cs:practice_form_request],2
.close:
    call practice_form_restore
    call practice_form_graphics_restore
    mov byte [cs:practice_form_open],0
    ret

practice_form_draw_screen:
    mov ax,0xa000
    mov es,ax
    call practice_form_draw
    ret

; The trainer and pause cannot be active at once; use the shared text buffer.
practice_form_save:
    mov ax,0xa000
    mov es,ax
    mov di,5*160+20*2
    mov si,practice_text_saved
    mov dx,13
.row:
    mov cx,25
.cell:
    mov ax,[es:di]
    mov [cs:si],ax
    mov ax,[es:di+0x2000]
    mov [cs:si+2],ax
    add si,4
    add di,2
    loop .cell
    add di,110
    dec dx
    jnz .row
    ret
practice_form_restore:
    mov ax,0xa000
    mov es,ax
    mov di,5*160+20*2
    mov si,practice_text_saved
    mov dx,13
.row:
    mov cx,25
.cell:
    mov ax,[cs:si]
    mov [es:di],ax
    mov ax,[cs:si+2]
    mov [es:di+0x2000],ax
    add si,4
    add di,2
    loop .cell
    add di,110
    dec dx
    jnz .row
    ret

; Preserve both planar pages in DOS memory, then clear only the form rectangle.
; The game is suspended until the pages are restored, so the black panel never
; becomes part of its background or damages either page on resume.
practice_form_graphics_save:
    pushf
    cld
    push ds
    mov al,[PRACTICE_PAGE_BACK]
    mov [cs:practice_form_page],al
    mov bx,2600                 ; 25 bytes * 208 lines * 4 planes * 2 pages
    mov ah,0x48
    int 0x21
    jc .failed
    mov [cs:practice_form_buffer],ax
    mov es,ax
    xor di,di
    mov byte [cs:practice_form_page_index],0
    xor al,al
    out 0x7c,al                ; ordinary planar VRAM access
.page:
    mov al,[cs:practice_form_page_index]
    out 0xa6,al
    xor bx,bx
.plane:
    mov ax,[cs:practice_form_planes+bx]
    mov ds,ax
    mov si,6420                ; x=160, y=80 at 80 bytes per raster line
    mov bp,208
.row:
    mov cx,25
.cell:
    lodsb
    stosb
    mov byte [si-1],0
    loop .cell
    add si,55
    dec bp
    jnz .row
    add bx,2
    cmp bx,8
    jb .plane
    inc byte [cs:practice_form_page_index]
    cmp byte [cs:practice_form_page_index],2
    jb .page
    mov al,[cs:practice_form_page]
    out 0xa6,al
    pop ds
    popf
    ret
.failed:
    mov word [cs:practice_form_buffer],0
    pop ds
    popf
    ret

practice_form_graphics_restore:
    cmp word [cs:practice_form_buffer],0
    je .done
    pushf
    cld
    push ds
    mov ax,[cs:practice_form_buffer]
    mov ds,ax
    xor si,si
    mov byte [cs:practice_form_page_index],0
    xor al,al
    out 0x7c,al
.page:
    mov al,[cs:practice_form_page_index]
    out 0xa6,al
    xor bx,bx
.plane:
    mov ax,[cs:practice_form_planes+bx]
    mov es,ax
    mov di,6420
    mov bp,208
.row:
    mov cx,25
    rep movsb
    add di,55
    dec bp
    jnz .row
    add bx,2
    cmp bx,8
    jb .plane
    inc byte [cs:practice_form_page_index]
    cmp byte [cs:practice_form_page_index],2
    jb .page
    mov al,[cs:practice_form_page]
    out 0xa6,al
    mov es,[cs:practice_form_buffer]
    mov ah,0x49
    int 0x21
    mov word [cs:practice_form_buffer],0
    pop ds
    popf
.done:
    ret
practice_form_planes: dw 0xa800,0xb000,0xb800,0xe000
practice_form_buffer: dw 0
practice_form_page: db 0
practice_form_page_index: db 0

; ES = A000h. Draw ten rows in the native PC-98 text plane.
practice_form_draw:
    push ds
    push cs
    pop ds
    mov di,5*160+20*2
    mov bp,practice_form_labels
    cmp byte [cs:practice_form_options_page],0
    je .labels_ready
    mov bp,practice_form_option_labels
.labels_ready:
    xor dx,dx
.row:
    mov si,[cs:bp]
    mov bl,0xe5
    cmp dl,[cs:practice_form_choice]
    jne .background
    mov bl,0xa5
.background:
    mov [cs:practice_form_row_attr],bl
    push di
    mov cx,25
.background_cell:
    mov word [es:di],' '
    xor bh,bh
    mov [es:di+0x2000],bx
    add di,2
    loop .background_cell
    pop di
.label:
    call practice_text_write
    cmp byte [cs:practice_form_options_page],0
    jne .draw_option
    cmp dl,11
    jae .next
    cmp dl,10
    je .draw_hp
    cmp dl,9
    je .draw_rank
    cmp dl,1
    je .character
    cmp dl,2
    je .difficulty
    cmp dl,3
    je .entry
    cmp dl,8
    je .branch
    xor bx,bx
    mov bl,dl
    xor ax,ax
    mov al,[cs:practice_form_values+bx]
    test dl,dl
    jnz .number
    inc ax                       ; internal stage 0 is the first stage
.number:
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
    mov al,[cs:practice_form_row_attr]
    mov [es:bx+0x2000],al
    mov [es:bx+0x2002],al
    mov [es:bx+0x2004],al
    jmp .next
.draw_option:
    cmp dl,11
    jae .next
    xor bx,bx
    mov bl,dl
    mov al,[cs:practice_form_options+bx]
    xor ah,ah
    shl ax,1
    mov bx,ax
    mov si,[cs:practice_form_option_values+bx]
    cmp dl,10
    jne .option_text
    mov si,[cs:practice_form_speed_values+bx]
.option_text:
    push di
    add di,19*2
    mov bl,[cs:practice_form_row_attr]
    call practice_text_write
    pop di
    jmp .next
.character:
    mov bx,practice_form_characters
    jmp .enum
.difficulty:
    mov bx,practice_form_difficulties
    jmp .enum
.entry:
    mov bx,practice_form_entries
    jmp .enum
.branch:
    mov bx,practice_form_branches
.enum:
    xor ax,ax
    mov al,dl
    mov si,ax
    mov al,[cs:practice_form_values+si]
    xor ah,ah
    shl ax,1
    add bx,ax
    mov si,[cs:bx]
    push di
    add di,17*2
    mov bl,[cs:practice_form_row_attr]
    call practice_text_write
    pop di
    jmp .next
.draw_rank:
    xor ax,ax
    mov al,[cs:practice_form_values+9]
    add ax,FORM_RANK_BASE
    mov bx,di
    add bx,22*2
    mov byte [es:bx],' '
    test ax,ax
    jns .rank_magnitude
    neg ax
    mov byte [es:bx],'-'
.rank_magnitude:
    mov cl,10
    div cl
    add ax,0x3030
    mov [es:bx+2],al
    mov [es:bx+4],ah
    mov byte [es:bx+1],0
    mov byte [es:bx+3],0
    mov byte [es:bx+5],0
    mov al,[cs:practice_form_row_attr]
    mov [es:bx+0x2000],al
    mov [es:bx+0x2002],al
    mov [es:bx+0x2004],al
    jmp .next
.draw_hp:
    mov ax,[cs:practice_form_hp]
    mov bx,di
    add bx,24*2
    mov cx,5
.hp_digit:
    xor dx,dx
    push cx
    mov cx,10
    div cx
    pop cx
    add dl,'0'
    mov [es:bx],dl
    mov byte [es:bx+1],0
    push ax
    mov al,[cs:practice_form_row_attr]
    mov [es:bx+0x2000],al
    pop ax
    sub bx,2
    loop .hp_digit
    mov dl,10
.next:
    add bp,2
    add di,160
    inc dl
    cmp dl,13
    jb .row
    pop ds
    ret
practice_form_choice: db 0
practice_form_options_page: db 0
practice_form_row_attr: db 0
practice_form_keys: dw 0
practice_form_repeat_key: dw 0
practice_form_repeat_ticks: db 0
practice_form_open: db 0
practice_form_input: dw 0
practice_form_request: db 0
practice_form_values: db 0,0,1,0,0,3,2,FORM_POWER_MAX,0,FORM_RANK_DEFAULT
practice_form_hp: dw 0
; lock rank, disable bomb/focus/shot, force focus, quick retry, always point,
; show keys, show info, prepared retry, speed index.
practice_form_options: times 11 db 0
practice_form_min: db 0,0,0,0,0,0,0,1,0,0
practice_form_max: db FORM_STAGE_MAX,FORM_CHARACTER_MAX,4,2,FORM_PHASE_MAX,FORM_STOCK_MAX,FORM_STOCK_MAX,FORM_POWER_MAX,FORM_BRANCH_MAX,FORM_RANK_MAX

; Phase bit zero is the normal entry. Other bits denote actual attack phases.
practice_form_phase_mask:
    mov edx,1
    cmp byte [cs:practice_form_values+3],0
    je .done
    cmp byte [cs:practice_form_values+3],1
    jne .boss
%ifndef FORM_TH02
    mov edx,3
    cmp byte [cs:practice_form_values],0
    jne .done
    mov edx,9
%endif
    ret
.boss:
    xor bx,bx
    mov bl,[cs:practice_form_values]
    shl bx,2
    mov edx,[cs:practice_form_phase_masks+bx]
%ifndef FORM_TH02
    cmp byte [cs:practice_form_values],3
    jne .extra
    cmp byte [cs:practice_form_values+1],2
    jb .done
    mov edx,0x00000b55
.extra:
    cmp byte [cs:practice_form_values],6
    jne .done
    cmp byte [cs:practice_form_values+8],1
    jne .done
    mov edx,0x000001bd
%endif
.done:
    ret
; Match the supported per-entry HP range, keeping zero as original HP.
practice_form_hp_limit:
    push bx
    push cx
    xor dx,dx
    mov bl,[cs:practice_form_values+3]
    test bl,bl
    jz .done
%ifdef FORM_TH02
    xor bx,bx
    mov bl,[cs:practice_form_values]
    cmp bl,2
    je .done
    cmp byte [cs:practice_form_values+3],1
    jne .boss
    shl bx,1
    mov dx,[cs:practice_form_mid_hp+bx]
    jmp .done
.boss:
    cmp bl,0
    je .first
    cmp bl,1
    je .second
    cmp bl,3
    jne .later
    mov dx,900
    jmp .done
.first:
    mov dx,2261
    cmp byte [cs:practice_form_values+4],0
    je .done
    mov dx,700
    cmp byte [cs:practice_form_values+4],3
    jne .done
    mov dx,861
    jmp .done
.second:
    mov dx,2400
    cmp byte [cs:practice_form_values+4],0
    je .done
    mov dx,701
    cmp byte [cs:practice_form_values+4],1
    je .done
    mov dx,800
    cmp byte [cs:practice_form_values+4],2
    je .done
    mov dx,899
    jmp .done
.later:
    xor bx,bx
    mov bl,[cs:practice_form_values+4]
    test bl,bl
    jnz .phase_index
    inc bl
.phase_index:
    shr bx,1
    shl bx,1
    cmp byte [cs:practice_form_values],4
    jne .extra_hp
    mov dx,[cs:practice_form_late_hp+bx]
    jmp .done
.extra_hp:
    mov dx,[cs:practice_form_extra_hp+bx]
%else
    mov dx,FORM_HP_MAX
%endif
.done:
    pop cx
    pop bx
    ret
%ifdef FORM_TH02
practice_form_mid_hp: dw 301,381,0,400,0,0
practice_form_late_hp: dw 600,700,800,1500,1100
practice_form_extra_hp: dw 1800,1800,1800,1800,1300
%endif
practice_form_phase_masks:
%ifdef FORM_TH02
    dd 0x0f,0x0f,1,1,0x2ab,0x2ab
%else
    dd 0x1d,0x3d,0x09,0x05,0x36db5,0x15115,0x55
%endif
practice_form_labels:
    dw .stage,.character,.difficulty,.entry,.phase,.lives,.bombs,.power,.branch,.rank,.hp,.start,.options
.stage: db 'STAGE                  ',0
.character: db 'CHARACTER              ',0
.difficulty: db 'DIFFICULTY             ',0
.entry: db 'ENTRY                  ',0
.phase: db 'PHASE                  ',0
.lives: db 'LIVES                  ',0
.bombs: db 'BOMBS                  ',0
.power: db 'POWER                  ',0
.branch: db 'EXTRA BRANCH           ',0
.rank: db 'RANK                   ',0
.hp: db 'BOSS HP                ',0
.start: db 'START PRACTICE         ',0
.options: db 'OPTIONS                ',0
practice_form_option_labels:
    dw .rank,.bomb,.focus,.shot,.force,.retry,.point,.keys,.info,.prepared,.speed,.back,.blank
.rank: db 'LOCK RANK              ',0
.bomb: db 'DISABLE BOMB           ',0
.focus: db 'DISABLE FOCUS          ',0
.shot: db 'DISABLE SHOT           ',0
.force: db 'FORCE FOCUS            ',0
.retry: db 'QUICK RETRY            ',0
.point: db 'ALWAYS HITBOX          ',0
.keys: db 'SHOW KEYS              ',0
.info: db 'SHOW INFO              ',0
.prepared: db 'PREPARED RETRY         ',0
.speed: db 'GAME SPEED             ',0
.back: db 'BACK                   ',0
.blank: db '                       ',0
practice_form_option_values: dw practice_form_off,practice_form_on
practice_form_speed_values: dw practice_form_speed25,practice_form_speed50,practice_form_speed75,practice_form_speed100,practice_form_speed150,practice_form_speed200
practice_form_off: db 'OFF',0
practice_form_on: db 'ON',0
practice_form_speed25: db '25%',0
practice_form_speed50: db '50%',0
practice_form_speed75: db '75%',0
practice_form_speed100: db '100%',0
practice_form_speed150: db '150%',0
practice_form_speed200: db '200%',0
practice_form_characters: dw practice_form_reimua,practice_form_reimub,practice_form_character3,practice_form_character4
practice_form_difficulties: dw practice_form_easy,practice_form_normal,practice_form_hard,practice_form_lunatic,practice_form_extra
practice_form_entries: dw practice_form_stage_value,practice_form_mid,practice_form_boss
practice_form_branches: dw practice_form_auto,practice_form_gengetsu
practice_form_reimua: db 'REIMU A',0
practice_form_reimub: db 'REIMU B',0
%ifdef FORM_TH02
practice_form_character3: db 'REIMU C',0
practice_form_character4: db 'REIMU C',0
%else
practice_form_character3: db 'MARISA A',0
practice_form_character4: db 'MARISA B',0
%endif
practice_form_easy: db 'EASY',0
practice_form_normal: db 'NORMAL',0
practice_form_hard: db 'HARD',0
practice_form_lunatic: db 'LUNATIC',0
practice_form_extra: db 'EXTRA',0
practice_form_stage_value: db 'STAGE',0
practice_form_mid: db 'MID',0
practice_form_boss: db 'BOSS',0
practice_form_auto: db 'AUTO',0
practice_form_gengetsu: db 'GENGETSU',0
%include 'practice-menu-input.inc'
%include 'practice-menu-values.inc'
