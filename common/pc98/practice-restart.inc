; MAIN applies the selected resident settings and chooses the next executable.
; Values: stage, difficulty, character, lives, bombs, apply-settings.
practice_restart_tick:
    cmp byte [cs:practice_restart_pending],0
    je .done
    mov byte [cs:practice_restart_pending],0
%ifdef PRACTICE_RESTART_TH02
    les bx,[0x27fc]
    cmp byte [cs:practice_restart_values+5],0
    je .stage02
    mov al,[cs:practice_restart_values+1]
    mov [es:bx+22],al
    mov al,[cs:practice_restart_values+2]
    mov [es:bx+38],al
    mov al,[cs:practice_restart_values+3]
    mov [es:bx+26],al
    mov al,[cs:practice_restart_values+4]
    mov [es:bx+25],al
.stage02:
    mov al,[cs:practice_restart_values]
    mov [es:bx+11],al
    mov [0x8f5],al
    mov dword [es:bx+14],0
    mov word [es:bx+18],0
    mov al,[es:bx+25]
    mov [es:bx+20],al
    mov al,[es:bx+26]
    mov [es:bx+21],al
    mov byte [cs:0x10e],0xe9
    mov word [cs:0x10f],9
    mov dword [cs:0x111],0x6e69616d
    mov byte [cs:0x115],0
    mov byte [cs:0x13b],0x0e
    mov word [cs:0x13d],0x111
    mov byte [0x2b97],1
%else
    les bx,[0xba86]
    cmp byte [cs:practice_restart_values+5],0
    je .stage04
    mov al,[cs:practice_restart_values+1]
    mov [es:bx+15],al
    mov al,[cs:practice_restart_values+2]
    mov ah,al
    shr al,1
    add al,'0'
    mov [es:bx+18],al
    and ah,1
    mov [es:bx+25],ah
    mov al,[cs:practice_restart_values+3]
    inc al
    mov [es:bx+11],al
    mov [es:bx+12],al
    mov [es:bx+58],al
    mov al,[cs:practice_restart_values+4]
    mov [es:bx+13],al
    mov [es:bx+14],al
    mov [es:bx+59],al
.stage04:
    mov al,[cs:practice_restart_values]
    mov [es:bx+17],al
    add al,'0'
    mov [es:bx+19],al
    mov dword [cs:0x88],0x6e69616d
    mov byte [cs:0x8c],0
    mov byte [cs:0x8d],0x0e
    mov word [cs:0x8f],0x88
    mov byte [0x5392],1
%endif
    cmp byte [cs:practice_result_selecting],0
    je .done
    mov byte [cs:practice_result_selecting],0
    mov byte [cs:hold],0
.done:
    ret
practice_restart_pending: db 0
practice_restart_values: times 6 db 0
practice_restart_committed: db 0

; Bit 6 is a replayable quick-retry command, separate from form navigation.
practice_quick_retry_tick:
    test word [cs:practice_form_input],64
    jz .done
    and word [cs:practice_form_input],0xffbf
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:practice_form_options+5],0
    je .done
    cmp byte [cs:practice_form_open],0
    jne .done
    cmp byte [cs:practice_pause_open],0
    jne .done
    cmp byte [cs:practice_outcome],0
    jne .done
    mov byte [cs:practice_form_request],1
    call practice_form_native_start
    mov byte [cs:practice_form_request],0
    mov byte [cs:practice_restart_committed],0
.done:
    ret

; The form remains authoritative: select settings and start MAIN in the same
; guest. The signal lets the host mirror the already-selected setup for its
; clock, audio, diagnostics and replay bookkeeping.
practice_form_native_start:
    push dx
%ifdef PRACTICE_RESTART_TH02
    mov dx,0x7fd1
%else
    mov dx,0x7fe1
%endif
    call signal
    pop dx
    mov ax,0xa000
    mov es,ax
    mov di,11*160+13*2
    mov si,practice_loading_text
    mov bl,0xe1
    call practice_text_write
    ; Keep a versioned copy in conventional DOS RAM, which survives EXEC
    ; of MAIN but is replaced on emulator boot. Invalidate magic first.
    xor ax,ax
    mov es,ax
    mov dword [es:0x4e0],0
    xor bx,bx
.save_values:
    mov al,[cs:practice_form_values+bx]
    mov [es:bx+0x4e4],al
    inc bx
    cmp bx,10
    jb .save_values
    mov ax,[cs:practice_form_hp]
    mov [es:0x4ee],ax
    xor bx,bx
.save_options:
    mov al,[cs:practice_form_options+bx]
    mov [es:bx+0x4f0],al
    inc bx
    cmp bx,11
    jb .save_options
%ifdef PRACTICE_RESTART_TH02
    mov dword [es:0x4e0],0x32525054 ; TPR2
%else
    mov dword [es:0x4e0],0x34525054 ; TPR4
%endif
    mov al,[cs:practice_form_values]
%ifndef PRACTICE_RESTART_TH02
    xor al,al
    cmp byte [cs:practice_form_values+2],4
    jne .stage_ready
    mov al,6
.stage_ready:
%endif
    mov [cs:practice_restart_values],al
    mov al,[cs:practice_form_values+2]
    mov [cs:practice_restart_values+1],al
    mov al,[cs:practice_form_values+1]
    mov [cs:practice_restart_values+2],al
    mov al,[cs:practice_form_values+5]
    mov [cs:practice_restart_values+3],al
    mov al,[cs:practice_form_values+6]
    mov [cs:practice_restart_values+4],al
    mov byte [cs:practice_restart_values+5],1
    mov byte [cs:practice_restart_pending],1
    mov byte [cs:practice_restart_committed],1
    call practice_restart_tick
    ret
practice_loading_text: db '     LOADING PRACTICE     ',0

practice_loading_clear:
    cmp byte [cs:target_preparing],0
    je .done
    push ax
    push cx
    push di
    push es
    mov ax,0xa000
    mov es,ax
    mov di,11*160+13*2
    mov cx,26
.cell:
    mov word [es:di],0x20
    mov word [es:di+0x2000],0
    add di,2
    loop .cell
    pop es
    pop di
    pop cx
    pop ax
.done:
    ret

practice_preparation_guard:
    cmp byte [cs:target_preparing],0
    je .done
    inc word [cs:practice_prepare_frames]
%ifdef PRACTICE_RESTART_TH02
    mov byte [0x2b94],0
    mov byte [0x2b95],2
    cmp word [cs:practice_prepare_frames],1
    jne .done
    cmp byte [cs:practice_initial_entry],0
    je .done
    mov dword [0x2ba2],159
%else
    mov byte [0x4662],2
    mov byte [0x4669],0
    mov byte [0x466a],0
%endif
.done:
    ret
practice_prepare_frames: dw 0

; Apply restrictions and rank through the game's own input/state variables.
practice_native_restrictions:
    cmp byte [cs:target_practice],0
    je .done
%ifdef PRACTICE_RESTART_TH02
    mov bx,0x2538
    mov cx,0x10
    mov dx,0x20
%else
    ; A long Boss attempt must not trigger the unrelated stage midboss.
    cmp byte [cs:practice_initial_entry],2
    jne .input04
    mov ax,[0x538a]
    dec ax
    mov [0x53c0],ax
.input04:
    mov bx,0x3974
    mov cx,0x20
    mov dx,0x10
%endif
    cmp byte [cs:target_preparing],0
    jne .clear_keys
    cmp byte [cs:practice_outcome],0
    je .options
.clear_keys:
    mov word [bx],0
.options:
    cmp byte [cs:practice_form_options+3],0
    je .bomb
    not cx
    and [bx],cx
.bomb:
    cmp byte [cs:practice_form_options+1],0
    je .focus
    not dx
    and [bx],dx
.focus:
%ifndef PRACTICE_RESTART_TH02
    cmp byte [cs:practice_form_options+2],0
    je .force_focus
    mov byte [0x3976],0
    jmp .rank
.force_focus:
    cmp byte [cs:practice_form_options+4],0
    je .rank
    mov byte [0x3976],1
%endif
.rank:
    cmp byte [cs:practice_form_options],0
    je .done
    mov ax,[cs:practice_initial_rank]
%ifdef PRACTICE_RESTART_TH02
    cmp byte [0x8f4],0
    jne .rank_normal
    cmp ax,4
    jle .rank_normal
    mov ax,4
.rank_normal:
    mov [0x2802],ax
%else
    mov [0x5395],al
%endif
.done:
    ret

%ifdef PRACTICE_RESTART_TH02
; Preserve AX and return the flags expected by the movement branch.
practice_focus_test:
    push ax
    call practice_focus_hook
    test ax,0x100
    pop ax
    ret
; The solo movement adapter reads focus through this accessor. Keep the raw
; key available for replay, but let native practice options override it.
practice_focus_hook:
    mov ax,[cs:SOLO_CURRENT_INPUT]
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:practice_form_options+2],0
    je .force
    and ax,0xfeff
    ret
.force:
    cmp byte [cs:practice_form_options+4],0
    je .done
    or ax,0x100
.done:
    ret
%endif

; Consume only this game's one-shot resident configuration at MAIN load.
practice_native_setup:
    push es
    push bx
    xor ax,ax
    mov es,ax
%ifdef PRACTICE_RESTART_TH02
    cmp dword [es:0x4e0],0x32525054
%else
    cmp dword [es:0x4e0],0x34525054
%endif
    jne .done
    mov dword [es:0x4e0],0
    xor bx,bx
.copy:
    mov al,[es:bx+0x4e4]
    mov [cs:practice_form_values+bx],al
    inc bx
    cmp bx,10
    jb .copy
    mov ax,[es:0x4ee]
    mov [cs:practice_form_hp],ax
    mov [cs:practice_initial_hp],ax
    xor bx,bx
.copy_options:
    mov al,[es:bx+0x4f0]
    mov [cs:practice_form_options+bx],al
    inc bx
    cmp bx,11
    jb .copy_options
    mov byte [cs:target_practice],1
    mov byte [cs:target_preparing],1
    mov al,[cs:practice_form_values]
    mov [cs:practice_initial_stage],al
    mov al,[cs:practice_form_values+3]
    mov [cs:practice_initial_entry],al
    mov al,[cs:practice_form_values+4]
    mov [cs:practice_initial_phase],al
    mov al,[cs:practice_form_values+5]
    mov [cs:practice_initial_lives],al
    mov al,[cs:practice_form_values+6]
    mov [cs:practice_initial_bombs],al
    mov al,[cs:practice_form_values+7]
    mov [cs:practice_initial_power],al
%ifdef PRACTICE_RESTART_TH02
    mov al,[cs:practice_form_values+9]
    sub al,6
    cbw
    cmp byte [cs:practice_form_values+2],0
    jne .rank_limit
    cmp ax,4
    jle .rank_ready
    mov ax,4
.rank_limit:
    cmp ax,16
    jle .rank_ready
    mov ax,16
.rank_ready:
    mov [cs:practice_initial_rank],ax
%else
    mov al,[cs:practice_form_values+8]
    mov [cs:practice_initial_gengetsu],al
    xor al,al
    cmp byte [cs:practice_form_values+1],2
    jb .character_ready
    inc al
.character_ready:
    mov [cs:practice_initial_reimu],al
    xor bx,bx
    mov bl,[cs:practice_form_values+2]
    shl bx,1
    mov al,[cs:practice_form_values+9]
    add al,4
    cmp al,[cs:practice_native_ranks+bx]
    jae .min_ready
    mov al,[cs:practice_native_ranks+bx]
.min_ready:
    cmp al,[cs:practice_native_ranks+bx+1]
    jbe .rank_ready
    mov al,[cs:practice_native_ranks+bx+1]
.rank_ready:
    mov [cs:practice_initial_rank],al
    les bx,[0xba86]
    mov al,[cs:practice_initial_stage]
    mov [es:bx+17],al
    mov [0x5394],al
    add al,'0'
    mov [es:bx+19],al
%endif
.done:
    pop bx
    pop es
    ret
%ifndef PRACTICE_RESTART_TH02
practice_native_ranks: db 4,16,11,24,20,32,22,34,16,20
%endif

; Empty native loading text from a prior MAIN on the first load of a normal
; game too; never erase unrelated HUD cells or title text.
practice_loading_clear_if_present:
    push ax
    push bx
    push cx
    push di
    push si
    push es
    mov ax,0xa000
    mov es,ax
    mov di,11*160+13*2
    mov si,practice_loading_text
    mov cx,26
.check:
    mov al,[cs:si]
    cmp [es:di],al
    jne .done
    inc si
    add di,2
    loop .check
    mov di,11*160+13*2
    mov cx,26
.clear:
    mov word [es:di],0x20
    mov word [es:di+0x2000],0
    add di,2
    loop .clear
.done:
    pop es
    pop si
    pop di
    pop cx
    pop bx
    pop ax
    ret
