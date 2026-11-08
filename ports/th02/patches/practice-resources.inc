; Apply the selected entry's initial resources in the guest. Caller preserves
; registers and flags and must have the current MAIN data segment in DS.
practice_target_resources:
    cmp byte [cs:target_preparing],0
    je .done
    mov al,[cs:practice_initial_lives]
    mov [0xb2c],al
    mov al,[cs:practice_initial_bombs]
    mov [0xb2d],al
    mov ax,[cs:practice_initial_rank]
    mov [0x2802],ax
    mov al,[cs:practice_initial_power]
    cmp [0xaa6],al
    je .done
    mov [0xaa6],al
    call ORIGINAL(0xdead)
.done:
    ret
practice_initial_lives: db 0
practice_initial_bombs: db 0
practice_initial_power: db 1
practice_initial_rank: dw 0

; Reset transient damage/input and target statistics at the native arrival
; boundary, immediately before MAIN enters its own pause procedure.
practice_target_arrival:
    cmp byte [cs:practice_arrival_pending],0
    je .done
    mov byte [cs:practice_arrival_pending],0
    call practice_target_hp
    call practice_boss_phase
    mov [cs:practice_selected_phase],al
    mov byte [cs:practice_seen_boss],0
    mov byte [cs:practice_fatal],0
    mov byte [cs:practice_outcome],0
    call practice_info_reset
    mov dword [0xb28],0
    mov dword [0x3e3c],0
    mov word [0x3e40],0
    mov word [0xb46],0
    mov byte [0x2b94],0
    mov byte [0x2b95],0
    mov byte [0x2b96],0
    mov byte [0x2b99],0
    mov byte [0xaa7],0
    mov byte [0x2b97],0
    mov word [0x2538],0
    mov byte [0x109d],255
    mov byte [0x109e],255
    call practice_checkpoint_save
.done:
    ret
practice_arrival_pending: db 0

; TH02 stores damage rather than current HP. Convert the requested HP using
; the actual target phase's native cumulative damage threshold.
practice_target_hp:
    cmp word [cs:practice_initial_hp],0
    je .done
    xor bx,bx
    mov bl,[0x8f5]
    cmp byte [cs:practice_initial_entry],1
    jne .boss
    shl bx,1
    mov ax,[cs:practice_mid_limits+bx]
    jmp .write
.boss:
    cmp bl,0
    jne .stage_two
    xor bx,bx
    mov bl,[cs:practice_initial_phase]
    shl bx,1
    mov ax,[cs:practice_rika_limits+bx]
    jmp .write
.stage_two:
    cmp bl,1
    jne .stage_four
    xor bx,bx
    mov bl,[cs:practice_initial_phase]
    shl bx,1
    mov ax,[cs:practice_meira_limits+bx]
    jmp .write
.stage_four:
    cmp bl,3
    jne .late
    mov ax,900
    jmp .write
.late:
    xor bx,bx
    mov bl,[cs:practice_initial_phase]
    shr bx,1
    shl bx,1
    cmp byte [0x8f5],4
    jne .extra
    mov ax,[cs:practice_mima_limits+bx]
    jmp .write
.extra:
    mov ax,[cs:practice_genji_limits+bx]
.write:
    sub ax,[cs:practice_initial_hp]
    jns .store
    xor ax,ax
.store:
    mov [0x2bea],ax
.done:
    ret
practice_initial_hp: dw 0
practice_mid_limits: dw 301,381,0,400,0,0
practice_rika_limits: dw 2261,700,1400,2261
practice_meira_limits: dw 2400,701,1501,2400
practice_mima_limits: dw 600,700,800,1500,1100
practice_genji_limits: dw 1800,1800,1800,1800,1300
