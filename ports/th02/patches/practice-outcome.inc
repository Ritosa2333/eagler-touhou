; Resolve a target attempt at the native post-collision boundary. A final
; death takes precedence over a simultaneous stage or attack completion.
practice_target_outcome:
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:target_preparing],0
    jne .done
    cmp byte [cs:practice_outcome],0
    jne .done
    cmp byte [0xaa7],0
    je .clear_fatal
    cmp byte [0xb2c],0
    je .death_window
.clear_fatal:
    mov byte [cs:practice_fatal],0
.death_window:
    cmp byte [0xaa7],0
    je .completion
    cmp byte [0xb2c],0
    jne .completion
    mov al,[0x2b99]
    test al,al
    jz .final_death
    cmp al,24
    jae .completion
    mov byte [cs:practice_fatal],1
    jmp .completion
.final_death:
    cmp byte [cs:practice_fatal],0
    je .completion
    mov byte [cs:practice_outcome],2
    mov byte [cs:hold],1
    ret
.completion:
    cmp byte [0xaa7],0
    jne .done
    cmp byte [0x2b94],0
    jne .done
    mov al,[0x8f5]
    cmp al,[cs:practice_initial_stage]
    jne .clear
    cmp byte [0x19f6],2
    je .clear
    cmp byte [cs:practice_initial_entry],1
    jne .boss
    cmp byte [0x2baa],0
    je .clear
    ret
.boss:
    cmp byte [0x19f6],1
    jne .phase
    mov eax,[0x1a0c]
    cmp eax,[0x1a10]
    jne .phase
    mov byte [cs:practice_seen_boss],1
.phase:
    cmp byte [cs:practice_initial_phase],0
    je .whole
    call practice_boss_phase
    cmp al,[cs:practice_selected_phase]
    jne .clear
    ret
.whole:
    cmp byte [cs:practice_seen_boss],0
    je .done
    cmp byte [0x2bfa],0
    je .done
.clear:
    mov byte [cs:practice_outcome],1
    mov byte [cs:hold],1
.done:
    ret
practice_initial_stage: db 0
practice_selected_phase: db 0
practice_seen_boss: db 0
practice_fatal: db 0
practice_outcome: db 0 ; 0 running, 1 clear, 2 failed
