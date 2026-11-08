; Advance earlier Boss attacks by applying their native end-HP threshold.
; The original phase transition code still runs and creates its own assets.
practice_target_advance:
    cmp byte [cs:target_preparing],0
    je .done
    cmp byte [cs:practice_initial_entry],0
    je .done
    mov ax,[0xbcd0]
    test ax,ax
    jz .done
    cmp ax,0x11c0
    je .done
    cmp byte [0x53d9],254
    jae .done
    mov dl,[0x53d9]
    mov dh,[0x5394]
    ; Enter the game's own Mugetsu-to-Gengetsu loader as soon as the Extra
    ; Boss has finished its initial resource setup. PHASE_NONE/frame zero is
    ; the native dialog boundary which frees Mugetsu's assets, loads Gengetsu
    ; and initializes its update/render functions. Do not synthesize pointers
    ; or run previous attacks or the defeat animation.
    cmp dh,6
    jne .entrance
    cmp byte [cs:practice_initial_gengetsu],0
    je .entrance
    cmp ax,0xc7da
    je .entrance
    cmp dl,2
    jb .entrance
    mov byte [0x53d9],255
    mov word [0x53da],0
    ret
.entrance:
    ; Preserve the handful of frame-triggered initialization events while
    ; skipping the long waits in Orange's and Kurumi's introductions.
    cmp dh,0
    jne .kurumi
    cmp dl,0
    jne .orange_one
    mov bx,[0x53da]
    test bx,bx
    jz .elly
    cmp bx,191
    jb .orange_192
    cmp bx,335
    jb .orange_336
    cmp bx,351
    jae .elly
    mov word [0x53da],351
    jmp .elly
.orange_192:
    mov word [0x53da],191
    jmp .elly
.orange_336:
    mov word [0x53da],335
    jmp .elly
.orange_one:
    cmp dl,1
    jne .elly
    mov bx,31
    jmp .intro_deadline
.kurumi:
    cmp dh,1
    jne .stage_four_intro
    cmp dl,0
    jne .kurumi_one
    mov bx,[0x53da]
    test bx,bx
    jz .elly
    cmp bx,128
    jb .kurumi_128
    cmp bx,296
    jb .kurumi_296
    cmp bx,320
    jae .elly
    mov word [0x53da],320
    jmp .elly
.kurumi_128:
    mov word [0x53da],128
    jmp .elly
.kurumi_296:
    mov word [0x53da],296
    jmp .elly
.kurumi_one:
    cmp dl,1
    jne .elly
    mov bx,32
    jmp .intro_deadline
.stage_four_intro:
    cmp dh,3
    jne .later_intro
    cmp dl,0
    jne .stage_four_one
    mov bx,97
    jmp .intro_deadline
.stage_four_one:
    cmp dl,1
    jne .elly
    mov bx,127
.intro_deadline:
    mov cx,[0x53da]
    test cx,cx
    jz .elly
    cmp cx,bx
    jae .elly
    mov [0x53da],bx
    jmp .elly
.later_intro:
    ; Entrance/fill states retain their frame-zero setup, then execute their
    ; original terminal frame without simulating the entire waiting period.
    cmp dh,5
    jb .elly
    xor bx,bx
    cmp dl,0
    jne .entrance_one
    mov bx,129
    jmp .deadline
.entrance_one:
    cmp dl,1
    jne .entrance_extra
    mov bx,64
    jmp .deadline
.entrance_extra:
    cmp dh,6
    jne .deadline
    cmp ax,0xc7da
    je .deadline
    cmp dl,3
    jne .entrance_five
    mov bx,64
    jmp .deadline
.entrance_five:
    cmp dl,5
    jne .deadline
    mov bx,128
.deadline:
    test bx,bx
    jz .gengetsu
    dec bx
    mov cx,[0x53da]
    test cx,cx
    jz .gengetsu
    cmp cx,bx
    jae .gengetsu
    mov [0x53da],bx
.gengetsu:
    cmp ax,0xc7da
    jne .elly
    ; Gengetsu's phase-two initializer has loaded its assets and common
    ; state. Later attacks use the same resident Boss and have explicit native
    ; HP boundaries, so initialize the requested attack directly here.
    cmp dl,2
    jne .gen_seek
    mov bl,[cs:practice_initial_phase]
    cmp bl,3
    jb .gen_seek
    cmp bl,8
    ja .gen_seek
    cmp bl,6
    je .gen_seek
    xor bh,bh
    shl bx,1
    mov ax,[cs:practice_gengetsu_start_hp+bx]
    mov [0x53d6],ax
    mov ax,[cs:practice_gengetsu_end_hp+bx]
    mov [0x53e0],ax
    mov al,[cs:practice_initial_phase]
    mov [0x53d9],al
    mov word [0x53da],0
    mov byte [0x53dc],0
    mov byte [0x53dd],0
    mov byte [0x53de],0
    mov byte [0x53df],0
    cmp al,7
    jne .gen_eight
    mov word [0x53da],32
.gen_eight:
    cmp al,8
    jne .done
    mov byte [0xbced],16
    ret
.gen_seek:
    cmp dl,[cs:practice_initial_phase]
    jae .elly
    cmp dl,2
    jb .gen_six
    cmp dl,5
    ja .gen_six
    cmp byte [0x53dd],255
    jne .gen_six
    mov bx,[0x53da]
    cmp bx,2
    jb .gen_six
    cmp bx,64
    jae .gen_six
    mov cx,64
    sub cx,bx
    imul cx,[0x53d2]
    add [0x53ca],cx
    mov word [0x53da],64
.gen_six:
    cmp dl,6
    jne .elly
    mov bx,[0x53da]
    test bx,bx
    jz .elly
    cmp bx,127
    jae .elly
    mov word [0x53da],127
.elly:
    cmp dh,2
    jne .group
    cmp dl,1
    jne .group
    mov word [0x538a],9240
.group:
    cmp dh,3
    jne .extra
    cmp byte [cs:practice_initial_reimu],0
    je .extra
    mov dh,7
.extra:
    cmp dh,6
    jne .find
    cmp ax,0xc7da
    jne .find
    mov dh,8
.find:
    mov si,practice_attack_states
.next:
    cmp byte [cs:si],255
    je .done
    cmp [cs:si],dh
    jne .skip
    cmp [cs:si+1],dl
    je .attack
.skip:
    add si,2
    jmp .next
.attack:
    cmp byte [cs:practice_initial_gengetsu],0
    je .phase
    cmp dh,6
    je .end_attack
.phase:
    cmp dl,[cs:practice_initial_phase]
    jae .done
.end_attack:
    cmp word [0x53e0],0
    jne .end_attack_threshold
    mov ax,[0x53d6]
    test ax,ax
    jle .end_attack_threshold
    mov [cs:practice_seek_final_hp],ax
.end_attack_threshold:
    mov ax,[0x53e0]
    dec ax
    jns .store
    xor ax,ax
.store:
    mov [0x53d6],ax
.done:
    ret
practice_initial_reimu: db 0
practice_seek_final_hp: dw 0
practice_gengetsu_start_hp: dw 0,0,18700,14700,12700,8900,0,5500,3000
practice_gengetsu_end_hp: dw 0,0,14700,12700,8900,5500,0,3000,0
practice_attack_states:
    db 0,2, 0,3, 0,4, 1,2, 1,3, 1,4, 1,5, 2,3, 3,2
    db 4,2, 4,4, 4,5, 4,7, 4,8, 4,10, 4,11, 4,13, 4,14, 4,16, 4,17
    db 5,2, 5,4, 5,8, 5,12, 5,14, 5,16, 6,2, 6,4, 6,6
    db 7,2, 7,4, 7,6, 7,8, 7,9, 7,11
    db 8,2, 8,3, 8,4, 8,5, 8,7, 8,8, 255
