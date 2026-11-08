; Apply the configured target's resources from MAIN's own frame boundary.
; The game's native HUD and shot updater run through resource_dirty.
practice_target_resources:
    cmp byte [cs:target_preparing],0
    je .done
    call practice_target_stocks
    mov al,[cs:practice_initial_power]
    cmp [0x4664],al
    je .dream
    mov [0x4664],al
    or byte [cs:resource_dirty],1
.dream:
    cmp byte [0x4676],0
    jne .clear_dream
    cmp word [0xbccc],0
    je .rank
.clear_dream:
    mov byte [0x4676],0
    mov word [0xbccc],0
    or byte [cs:resource_dirty],2
.rank:
    mov al,[cs:practice_initial_rank]
    mov [0x5395],al
.done:
    ret
practice_target_stocks:
    cmp byte [cs:target_preparing],0
    je .done
    push es
    les bx,[0xba86]
    mov al,[cs:practice_initial_lives]
    inc al
    mov [es:bx+11],al
    mov al,[cs:practice_initial_bombs]
    mov [es:bx+13],al
    pop es
.done:
    ret

; Use MAIN's own enemy VM, midboss and stage-scroll state to seek the target.
practice_target_warp:
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [cs:practice_warp_state],0
    jne .done
    mov byte [cs:practice_warp_state],1
    cmp byte [cs:practice_initial_entry],0
    je .done
    mov eax,[0xbcd0]
    mov [0x4646],eax
    cmp byte [cs:practice_initial_entry],2
    jne .midboss
    mov byte [0x4277],0
    mov [0x46b6],eax
    mov byte [0x53c5],255
    mov word [0x53c2],0
    ret
.midboss:
    mov ax,[0x53c0]
    mov [0x538a],ax
.done:
    ret
target_preparing: db 0
practice_initial_lives: db 0
practice_initial_bombs: db 0
practice_initial_power: db 1
practice_initial_rank: db 16
practice_initial_entry: db 0 ; stage=0, midboss=1, boss=2
practice_warp_state: db 0

; MAIN's own Boss and midboss state determines the requested entry boundary.
practice_target_check:
    mov byte [cs:practice_target_reached],0
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [cs:practice_initial_entry],0
    je .reached
    mov ax,[0xbcd0]
    test ax,ax
    jz .mid
    cmp ax,0x11c0
    je .mid
    cmp byte [0x53d9],254
    jae .mid
    cmp byte [cs:practice_initial_entry],2
    jne .done
    cmp byte [cs:practice_initial_gengetsu],0
    je .boss_phase
    cmp byte [0x5394],6
    jne .boss_phase
    cmp ax,0xc7da
    jne .done
.boss_phase:
    mov al,[0x53d9]
    cmp byte [cs:practice_initial_phase],0
    je .boss_minimum
    cmp al,[cs:practice_initial_phase]
    je .reached
    ret
.boss_minimum:
    mov ah,2
    cmp byte [0x5394],2
    jne .minimum
    mov ah,3
    jmp .minimum
.mid:
    cmp byte [cs:practice_initial_entry],1
    jne .done
    mov ax,[0x46b6]
    test ax,ax
    jz .done
    cmp ax,0x11c0
    je .done
    cmp byte [0x53c5],254
    jae .done
    mov al,[0x53c5]
    cmp byte [cs:practice_initial_phase],0
    je .mid_minimum
    cmp al,[cs:practice_initial_phase]
    je .reached
    ret
.mid_minimum:
    mov ah,1
    cmp byte [0x5394],0
    jne .minimum
    mov ah,3
.minimum:
    cmp al,ah
    jb .done
.reached:
    mov byte [cs:practice_target_reached],1
    mov byte [cs:target_preparing],0
    mov byte [cs:practice_arrival_pending],1
    mov byte [cs:entry_pause],1
    cmp byte [cs:practice_initial_entry],0
    je .done
    mov byte [cs:practice_cleanup_pending],1
    mov byte [cs:practice_cleanup_keep_bits],0
    cmp byte [cs:practice_initial_stage],3
    jne .done
    cmp byte [cs:practice_initial_entry],2
    jne .done
    cmp byte [cs:practice_initial_reimu],0
    jne .done
    mov byte [cs:practice_cleanup_keep_bits],1
.done:
    ret
practice_initial_phase: db 0
practice_initial_gengetsu: db 0
practice_target_reached: db 0

; Clear preparation damage and native attempt statistics before pause.
practice_target_arrival:
    cmp byte [cs:practice_arrival_pending],0
    je .done
    mov byte [cs:practice_arrival_pending],0
    cmp byte [cs:practice_initial_entry],2
    jne .target_hp
    cmp byte [cs:practice_initial_phase],0
    je .target_hp
    cmp word [0x53d6],0
    jg .target_hp
    mov ax,[cs:practice_seek_final_hp]
    test ax,ax
    jle .target_hp
    mov [0x53d6],ax
.target_hp:
    call practice_target_hp
    mov al,[0x53d9]
    cmp byte [cs:practice_initial_entry],1
    jne .selected
    mov al,[0x53c5]
.selected:
    mov [cs:practice_selected_phase],al
    mov byte [cs:practice_seen_boss],0
    mov byte [cs:practice_seen_gengetsu],0
    mov byte [cs:practice_fatal],0
    mov byte [cs:practice_outcome],0
    call practice_info_reset
    mov dword [0x4349],0
    mov dword [0x434d],0
    mov dword [0x435a],0
    mov dword [0x435e],0
    mov byte [0x1a66],0
    mov byte [0x4662],0
    mov byte [0x4663],0
    mov byte [0x4669],0
    mov byte [0x466a],0
    mov byte [0xbcca],0
    mov byte [0xbcba],0
    mov word [0x4656],0
    mov word [0x4658],0
    mov word [0x3974],0
    call practice_checkpoint_save
.done:
    ret
practice_arrival_pending: db 0

practice_target_hp:
    cmp byte [cs:practice_initial_entry],0
    je .done
    mov ax,[cs:practice_initial_hp]
    test ax,ax
    jz .done
    mov si,0x53d6
    mov dx,[0x53e0]
    cmp byte [cs:practice_initial_entry],1
    jne .bounds
    mov si,0x53c2
    xor dx,dx
.bounds:
    ; A selected attack ends at phase_end_hp, not at zero. The form's HP
    ; denotes remaining HP in that attack, so convert it to total Boss HP.
    cmp byte [cs:practice_initial_entry],2
    jne .limit
    cmp byte [cs:practice_initial_phase],0
    je .limit
    add ax,dx
    jno .limit
    mov ax,0x7fff
.limit:
    cmp ax,[si]
    jle .minimum
    mov ax,[si]
.minimum:
    inc dx
    cmp ax,dx
    jge .write
    mov ax,dx
.write:
    mov [si],ax
.done:
    ret
practice_initial_hp: dw 0
