; Resolve the selected target at native Boss/world update boundaries.
practice_target_outcome:
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:target_preparing],0
    jne .done
    cmp byte [cs:practice_outcome],0
    jne .done
    les bx,[0xba86]
    mov al,[es:bx+11]
    cmp al,1
    jbe .death_window
    mov byte [cs:practice_fatal],0
.death_window:
    cmp al,1
    ja .completion
    mov al,[0x466a]
    test al,al
    jz .final_death
    cmp al,32
    ja .completion
    mov byte [cs:practice_fatal],1
    jmp .completion
.final_death:
    cmp byte [cs:practice_fatal],0
    je .completion
    mov byte [cs:practice_outcome],2
    mov byte [cs:hold],1
    ret
.completion:
    cmp byte [0x466a],0
    jne .done
    cmp byte [0x4669],0
    jne .done
    mov al,[0x5394]
    cmp al,[cs:practice_initial_stage]
    jne .clear
    cmp byte [0x5392],2
    je .clear
    cmp byte [cs:practice_initial_entry],1
    jne .boss
    mov al,[0x53c5]
    cmp byte [cs:practice_initial_phase],0
    je .mid_whole
    cmp al,[cs:practice_selected_phase]
    jne .clear
    ret
.mid_whole:
    cmp al,254
    jae .clear
    ret
.boss:
    mov ax,[0xbcd0]
    test ax,ax
    jz .phase
    cmp ax,0x11c0
    je .phase
    cmp byte [0x53d9],254
    jae .phase
    cmp byte [0x53d9],2
    jb .gengetsu
    mov byte [cs:practice_seen_boss],1
.gengetsu:
    cmp ax,0xc7da
    jne .phase
    mov byte [cs:practice_seen_gengetsu],1
.phase:
    mov al,[0x53d9]
    cmp byte [cs:practice_initial_phase],0
    je .whole
    cmp al,[cs:practice_selected_phase]
    jne .clear
    ret
.whole:
    cmp byte [cs:practice_seen_boss],0
    je .done
    cmp byte [cs:practice_initial_stage],6
    jne .terminal
    cmp byte [cs:practice_initial_gengetsu],0
    jne .terminal
    cmp byte [cs:practice_seen_gengetsu],0
    je .done
.terminal:
    cmp al,254
    jb .done
.clear:
    mov byte [cs:practice_outcome],1
    mov byte [cs:hold],1
.done:
    ret
practice_initial_stage: db 0
practice_selected_phase: db 0
practice_seen_boss: db 0
practice_seen_gengetsu: db 0
practice_fatal: db 0
practice_outcome: db 0 ; 0 running, 1 clear, 2 failed
