; A same-MAIN checkpoint for practice targets. The stack is above the
; saved data range. DOS owns the buffer and frees it when MAIN exits.
%ifdef PRACTICE_RESTART_TH02
%define PRACTICE_RESIDENT_PTR 0x27fc
%define PRACTICE_RESIDENT_SIZE 48
%else
%define PRACTICE_RESIDENT_PTR 0xba86
%define PRACTICE_RESIDENT_SIZE 74
%endif
%if PRACTICE_CHECKPOINT_BYTES <= 0 || PRACTICE_CHECKPOINT_BYTES + PRACTICE_RESIDENT_SIZE >= 65536
%error Invalid MAIN data checkpoint range
%endif

practice_checkpoint_save:
    ; A restored attempt reaches this hook again. Keep the original entry,
    ; rather than drifting forward by a frame on every subsequent retry.
    cmp byte [cs:practice_checkpoint_valid],0
    jne .done
    cmp byte [cs:target_practice],0
    je .done
    pushf
    pushad
    push ds
    push es
    cld
    mov byte [cs:practice_checkpoint_valid],0
    cmp word [cs:practice_checkpoint_buffer],0
    jne .allocated
    mov bx,(PRACTICE_CHECKPOINT_BYTES+PRACTICE_RESIDENT_SIZE+15)/16
    mov ah,0x48
    int 0x21
    jnc .store_buffer
%ifdef PRACTICE_NATIVE_FORM_START
    ; Prioritize immediate Retry when DOS cannot hold both snapshots. Reuse
    ; the early encounter buffer for the selected attack; an earlier phase
    ; can still be selected through the normal native reload path.
    cmp byte [cs:practice_checkpoint_baseline_valid],0
    je .exit
    mov ax,[cs:practice_checkpoint_baseline_buffer]
    mov word [cs:practice_checkpoint_baseline_buffer],0
    mov byte [cs:practice_checkpoint_baseline_valid],0
    jmp .store_buffer
%else
    jmp .exit
%endif
.store_buffer:
    mov [cs:practice_checkpoint_buffer],ax
.allocated:
    mov es,[cs:practice_checkpoint_buffer]
    xor si,si
    xor di,di
    mov cx,PRACTICE_CHECKPOINT_BYTES
    rep movsb
    les bx,[PRACTICE_RESIDENT_PTR]
    mov ax,es
    mov ds,ax
    mov si,bx
    mov es,[cs:practice_checkpoint_buffer]
    mov di,PRACTICE_CHECKPOINT_BYTES
    mov cx,PRACTICE_RESIDENT_SIZE
    rep movsb
%ifdef PRACTICE_NATIVE_FORM_START
    cmp byte [cs:practice_checkpoint_seekable],0
    je .saved_setup
    xor bx,bx
.copy_setup:
    mov al,[cs:practice_form_values+bx]
    mov [cs:practice_checkpoint_values+bx],al
    inc bx
    cmp bx,10
    jb .copy_setup
.saved_setup:
%endif
    mov byte [cs:practice_checkpoint_valid],1
.exit:
    pop es
    pop ds
    popad
    popf
.done:
    ret

practice_checkpoint_restore:
    pushf
    pushad
    push ds
    push es
    cld
    push ds
    pop es
    mov ds,[cs:practice_checkpoint_buffer]
    xor si,si
    xor di,di
    mov cx,PRACTICE_CHECKPOINT_BYTES
    rep movsb
    push es
    pop ds
    les bx,[PRACTICE_RESIDENT_PTR]
    mov di,bx
    mov ds,[cs:practice_checkpoint_buffer]
    mov si,PRACTICE_CHECKPOINT_BYTES
    mov cx,PRACTICE_RESIDENT_SIZE
    rep movsb
    pop es
    pop ds
    popad
    popf
    ret

practice_checkpoint_buffer: dw 0
practice_checkpoint_valid: db 0
practice_checkpoint_requested: db 0
%ifdef PRACTICE_NATIVE_FORM_START
; Keep a separate early-encounter snapshot for selecting other attacks. The
; ordinary checkpoint remains at the chosen attack, so Retry stays instant.
practice_checkpoint_capture_baseline:
    pushf
    pushad
    cmp byte [cs:practice_checkpoint_baseline_valid],0
    jne .done
    cmp byte [cs:practice_checkpoint_valid],0
    jne .done
    mov dl,bl
    call practice_checkpoint_save
    cmp byte [cs:practice_checkpoint_valid],0
    je .done
    mov ax,[cs:practice_checkpoint_buffer]
    mov [cs:practice_checkpoint_baseline_buffer],ax
    mov word [cs:practice_checkpoint_buffer],0
    mov byte [cs:practice_checkpoint_baseline_valid],1
    mov byte [cs:practice_checkpoint_valid],0
    xor bx,bx
.copy:
    mov al,[cs:practice_checkpoint_values+bx]
    mov [cs:practice_checkpoint_baseline_values+bx],al
    inc bx
    cmp bx,10
    jb .copy
    mov [cs:practice_checkpoint_baseline_values+4],dl
.done:
    popad
    popf
    ret

; A checkpoint saved at the first attack can be promoted before the first
; retarget; the selected attack will receive its own buffer on arrival.
practice_checkpoint_promote:
    pushf
    pushad
    cmp byte [cs:practice_checkpoint_baseline_valid],0
    jne .done
    cmp byte [cs:practice_checkpoint_valid],0
    je .done
    mov ax,[cs:practice_checkpoint_buffer]
    mov [cs:practice_checkpoint_baseline_buffer],ax
    mov word [cs:practice_checkpoint_buffer],0
    mov byte [cs:practice_checkpoint_baseline_valid],1
    mov byte [cs:practice_checkpoint_valid],0
    xor bx,bx
.copy:
    mov al,[cs:practice_checkpoint_values+bx]
    mov [cs:practice_checkpoint_baseline_values+bx],al
    inc bx
    cmp bx,10
    jb .copy
.done:
    popad
    popf
    ret

; A native-form checkpoint can be reused for a later attack in the same Boss
; encounter when every other setup field still matches its original value.
practice_checkpoint_can_seek:
    pushf
    pushad
    mov byte [cs:practice_checkpoint_retarget],0
    mov si,practice_checkpoint_values
    cmp byte [cs:practice_checkpoint_baseline_valid],0
    je .primary
    mov si,practice_checkpoint_baseline_values
    jmp .available
.primary:
    cmp byte [cs:practice_checkpoint_valid],0
    je .done
.available:
    cmp byte [cs:practice_checkpoint_seekable],0
    je .done
    cmp byte [cs:target_practice],0
    je .done
    cmp byte [cs:practice_initial_entry],2
    jne .done
    mov al,[cs:practice_form_values]
    cmp al,[cs:practice_initial_stage]
    jne .done
    cmp byte [cs:practice_form_values+3],2
    jne .done
%ifdef PRACTICE_RESTART_TH02
    cmp byte [cs:practice_initial_stage],1
    je .compatible
    cmp byte [cs:practice_initial_stage],4
    jne .done
    ; Mima replaces her sprite allocation before phase 9. Do not reuse an
    ; earlier checkpoint after that resource transition has freed its assets.
    cmp byte [cs:practice_form_values+4],9
    jne .compatible
    cmp byte [cs:si+4],9
    jne .done
.compatible:
%else
    ; Normal-stage Bosses and Gengetsu retain their loaded sprites while
    ; switching attacks. Mugetsu hands resources to a different encounter.
    cmp byte [cs:practice_initial_stage],5
    jbe .compatible
    cmp byte [cs:practice_initial_stage],6
    jne .done
    cmp byte [cs:practice_initial_gengetsu],1
    jne .done
.compatible:
%endif
    xor bx,bx
.compare:
    cmp bx,4
    je .next
    mov al,[cs:practice_form_values+bx]
    cmp al,[cs:si+bx]
    jne .done
.next:
    inc bx
    cmp bx,10
    jb .compare
    mov al,[cs:practice_form_values+4]
    cmp al,[cs:si+4]
    jb .done
    mov byte [cs:practice_checkpoint_retarget],1
.done:
    popad
    popf
    ret
practice_checkpoint_seekable: db 0
practice_checkpoint_retarget: db 0
practice_checkpoint_use_baseline: db 0
practice_checkpoint_values: times 10 db 0
practice_checkpoint_baseline_buffer: dw 0
practice_checkpoint_baseline_valid: db 0
practice_checkpoint_baseline_values: times 10 db 0
%endif
