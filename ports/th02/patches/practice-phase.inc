; Read TH02's actual Boss attack state. AL is zero for bosses that do not
; expose independent attack targets, otherwise its native phase number.
practice_boss_phase:
    xor ax,ax
    cmp byte [0x8f5],0
    jne .stage_two
    inc al
    cmp word [0x2bea],700
    jb .done
    inc al
    cmp word [0x2bea],1400
    jb .done
    inc al
    ret
.stage_two:
    cmp byte [0x8f5],1
    jne .stage_five
    mov al,[0x7886]
    inc al
    ret
.stage_five:
    cmp byte [0x8f5],4
    jne .stage_extra
    mov ax,[0x91f8]
    ret
.stage_extra:
    cmp byte [0x8f5],5
    jne .done
    mov al,[0x7b42]
.done:
    ret

; Called immediately before the native Mima/Genjii phase comparison.
practice_target_transition:
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [0x19f6],1
    jne .done
    mov eax,[0x1a0c]
    cmp eax,[0x1a10]
    jne .done
    call practice_boss_phase
    test al,1
    jz .done
    cmp al,[cs:practice_initial_phase]
    jae .done
    mov word [0x2be0],0
.done:
    ret

; Advance earlier attacks using the native Boss damage/transition state.
; Native initializers and resource loading still run for each phase.
practice_target_advance:
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [0x19f6],1
    jne .done
    mov eax,[0x1a0c]
    cmp eax,[0x1a10]
    jne .done
    ; Meira's three attacks use an explicit major phase and pattern index.
    ; Her normal Boss initializer has already run at this point; select the
    ; requested attack directly instead of waiting for the preceding pattern
    ; to complete its current cycle.
    cmp byte [0x8f5],1
    jne .read_phase
    mov al,[cs:practice_initial_phase]
    cmp al,2
    jb .read_phase
    cmp al,3
    ja .read_phase
    dec al
    cmp [0x7886],al
    jae .read_phase
    mov [0x7886],al
    mov byte [0x7887],0
    mov word [0x2be0],0
    mov ax,701
    cmp byte [cs:practice_initial_phase],2
    je .direct_meira
    mov ax,1501
.direct_meira:
    mov [0x2bea],ax
    ret
.read_phase:
    call practice_boss_phase
    mov bl,al
    cmp byte [0x8f5],4
    jne .extra
    cmp al,[cs:practice_initial_phase]
    jae .extra
    test al,1
    jnz .extra
    cmp al,8
    jae .extra
    mov word [0x2be0],100
.extra:
    cmp byte [0x8f5],5
    jne .damage
    test bl,bl
    jnz .damage
    mov word [0x2be0],50
.damage:
    cmp bl,[cs:practice_initial_phase]
    jae .done
    cmp byte [0x8f5],0
    jne .stage_two
    mov ax,1400
    cmp byte [cs:practice_initial_phase],2
    jne .write
    mov ax,700
    jmp .write
.stage_two:
    cmp byte [0x8f5],1
    jne .late
    mov ax,1501
    cmp bl,1
    jne .write
    mov ax,701
    jmp .write
.late:
    cmp byte [0x8f5],4
    jb .done
    test bl,1
    jz .done
    mov ax,1800
    cmp byte [0x8f5],4
    jne .write
    ; Advance each Mima attack at its native damage threshold. Her next
    ; attack initializer resets damage after the transition.
    mov ax,600
    cmp bl,1
    je .write
    mov ax,700
    cmp bl,3
    je .write
    mov ax,800
    cmp bl,5
    je .write
    mov ax,1500
.write:
    mov [0x2bea],ax
.done:
    ret

; Prepare the selected Boss or midboss through the game's own spawner and
; scroll state. The first frame is reserved for the stage-title cleanup.
practice_target_warp:
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [cs:practice_initial_entry],0
    je .done
    cmp byte [cs:practice_warp_state],0
    jne .second
    mov byte [cs:practice_warp_state],1
    ret
.second:
    cmp byte [cs:practice_warp_state],1
    jne .done
    mov byte [cs:practice_warp_state],2
    mov eax,[0x1a0c]
    mov [0x91d0],eax
    cmp byte [cs:practice_initial_entry],2
    jne .midboss
    mov byte [0x2baa],0
    mov byte [0xa91],1
    mov byte [0xa93],0
    ret
.midboss:
    mov byte [0x2baa],1
    mov ax,[0x2ba6]
    mov [0x28da],ax
.done:
    ret

; Determine arrival from MAIN's actual Boss/midboss state, not render timing.
practice_target_check:
    mov byte [cs:practice_target_reached],0
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [cs:practice_initial_entry],0
    je .reached
    cmp byte [0x19f6],1
    jne .mid
    mov eax,[0x1a0c]
    cmp eax,[0x1a10]
    jne .mid
    cmp byte [cs:practice_initial_entry],2
    jne .done
    cmp byte [0x2bfa],0
    jne .done
    call practice_boss_phase
    cmp byte [cs:practice_initial_phase],0
    je .whole_boss
    cmp al,[cs:practice_initial_phase]
    je .reached
    ret
.whole_boss:
    cmp byte [0x8f5],4
    jb .reached
    test al,1
    jnz .reached
    ret
.mid:
    cmp byte [cs:practice_initial_entry],1
    jne .done
    cmp byte [0x2baa],0
    je .done
    mov ax,2
    cmp byte [0x8f5],0
    jne .mid_two
    mov ax,18
    jmp .mid_check
.mid_two:
    cmp byte [0x8f5],1
    jne .mid_three
    mov ax,320
    jmp .mid_check
.mid_three:
    cmp byte [0x8f5],2
    jne .mid_check
    mov ax,128
.mid_check:
    cmp [0x2be0],ax
    jb .done
.reached:
    mov byte [cs:practice_target_reached],1
    mov byte [cs:target_preparing],0
    mov byte [cs:practice_arrival_pending],1
    mov byte [cs:entry_pause],1
.done:
    ret
practice_initial_phase: db 0
practice_initial_entry: db 0 ; stage=0, midboss=1, boss=2
practice_warp_state: db 0
practice_target_reached: db 0
