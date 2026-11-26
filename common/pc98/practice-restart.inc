; MAIN applies the selected resident settings and chooses the next executable.
; Values: stage, difficulty, character, lives, bombs, apply-settings.
practice_restart_tick:
    cmp byte [cs:practice_restart_pending],0
    je .done
    mov byte [cs:practice_restart_pending],0
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
%ifdef PRACTICE_CHECKPOINT_BYTES
    mov byte [cs:practice_checkpoint_requested],1
%endif
    call practice_form_native_start
    mov byte [cs:practice_form_request],0
    mov byte [cs:practice_restart_committed],0
.done:
    ret

; The form remains authoritative: select settings and start MAIN in the same
; guest. The signal lets the host mirror the already-selected setup for its
; clock, audio, diagnostics and replay bookkeeping.
practice_form_native_start:
%ifdef PRACTICE_CHECKPOINT_BYTES
    mov byte [cs:practice_checkpoint_retarget],0
    mov byte [cs:practice_checkpoint_use_baseline],0
    cmp byte [cs:practice_checkpoint_requested],0
    jne .checkpoint_requested
    call practice_checkpoint_can_seek
    cmp byte [cs:practice_checkpoint_retarget],0
    je .reload
.checkpoint_requested:
    mov byte [cs:practice_checkpoint_requested],0
    cmp byte [cs:practice_checkpoint_retarget],0
    jne .retarget_ready
    cmp byte [cs:practice_checkpoint_valid],0
    jne .checkpoint_ready
    cmp byte [cs:practice_checkpoint_baseline_valid],0
    je .reload
    mov byte [cs:practice_checkpoint_use_baseline],1
    jmp .checkpoint_ready
.retarget_ready:
    mov byte [cs:practice_checkpoint_use_baseline],1
.checkpoint_ready:
    ; Notify the replay/clock bridge that the following transition stays in
    ; this MAIN, then restore the native data and resident configuration.
    mov byte [cs:practice_restart_pending],2
    push dx
    mov dx,0x7fe1
    call signal
    cmp byte [cs:practice_checkpoint_use_baseline],0
    je .restore_selected
    cmp byte [cs:practice_checkpoint_retarget],0
    je .restore_baseline
    call practice_checkpoint_promote
.restore_baseline:
    mov ax,[cs:practice_checkpoint_buffer]
    push ax
    mov ax,[cs:practice_checkpoint_baseline_buffer]
    mov [cs:practice_checkpoint_buffer],ax
    call practice_checkpoint_restore
    pop ax
    mov [cs:practice_checkpoint_buffer],ax
    cmp byte [cs:practice_checkpoint_retarget],0
    je .restore_finished
    ; The new selected attack will save over its own checkpoint at arrival.
    mov byte [cs:practice_checkpoint_valid],0
    jmp .restore_finished
.restore_selected:
    call practice_checkpoint_restore
.restore_finished:
    cmp byte [cs:practice_checkpoint_retarget],0
    je .restored_setup
    mov al,[cs:practice_form_values+4]
    mov [cs:practice_initial_phase],al
    mov ax,[cs:practice_form_hp]
    mov [cs:practice_initial_hp],ax
.restored_setup:
    mov byte [cs:practice_restart_pending],0
    mov byte [cs:practice_restart_committed],1
    mov byte [cs:practice_result_selecting],0
    mov byte [cs:practice_outcome],0
    mov byte [cs:hold],0
    mov byte [cs:target_preparing],1
    mov byte [cs:practice_target_reached],0
    mov byte [cs:practice_arrival_pending],0
    mov byte [cs:practice_redraw_pending],2
    mov dx,0x7fe0
    call signal
    pop dx
    ret
.reload:
%endif
    push dx
    mov dx,0x7fe1
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
    mov al,[cs:practice_assists]
    mov [es:0x4fb],al
    mov dword [es:0x4e0],0x34525054 ; TPR4
    mov al,[cs:practice_form_values]
    xor al,al
    cmp byte [cs:practice_form_values+2],4
    jne .stage_ready
    mov al,6
.stage_ready:
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
    cmp word [cs:practice_prepare_frames],40000
    jb .prepare
    mov byte [cs:target_preparing],0
    mov byte [cs:practice_outcome],2
    mov byte [cs:hold],1
    ret
.prepare:
    mov byte [0x4662],2
    mov byte [0x4669],0
    mov byte [0x466a],0
.done:
    ret
practice_prepare_frames: dw 0

; Apply restrictions and rank through the game's own input/state variables.
practice_native_restrictions:
    cmp byte [cs:target_practice],0
    je .done
    ; A long Boss attempt must not trigger the unrelated stage midboss.
    cmp byte [cs:practice_initial_entry],2
    jne .mid04
    mov ax,[0x538a]
    dec ax
    mov [0x53c0],ax
    jmp .input04
.mid04:
    ; Stage 1's midboss immediately retreats once the map scroll accelerates.
    ; Keep its actual combat available for a dedicated practice attempt.
    cmp byte [cs:practice_initial_entry],1
    jne .input04
    cmp byte [0x5394],0
    jne .input04
    cmp byte [0x4277],2
    jbe .input04
    mov byte [0x4277],2
.input04:
    mov bx,0x3974
    mov cx,0x20
    mov dx,0x10
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
    cmp byte [cs:practice_form_options+2],0
    je .force_focus
    mov byte [0x3976],0
    jmp .rank
.force_focus:
    cmp byte [cs:practice_form_options+4],0
    je .rank
    mov byte [0x3976],1
.rank:
    cmp byte [cs:practice_form_options],0
    je .done
    mov ax,[cs:practice_initial_rank]
    mov [0x5395],al
.done:
    ret


; Consume only this game's one-shot resident configuration at MAIN load.
practice_native_setup:
    push es
    push bx
    xor ax,ax
    mov es,ax
    cmp dword [es:0x4e0],0x34525054
    jne .done
    mov dword [es:0x4e0],0
%ifdef PRACTICE_CHECKPOINT_BYTES
    mov byte [cs:practice_checkpoint_seekable],1
%endif
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
    mov al,[es:0x4fb]
    mov [cs:practice_assists],al
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
.done:
    pop bx
    pop es
    ret
practice_native_ranks: db 4,16,11,24,20,32,22,34,16,20

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
