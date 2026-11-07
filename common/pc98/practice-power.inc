; DS: game data. POWER_BYTE and POWER_REFRESH are provided by each game.
; Capture power on the first frame with F4 enabled. Restore it through the
; game's own updater so shot level and HUD follow the stored value.
practice_power_lock:
    test byte [cs:practice_assists], 8
    jnz .enabled
    mov byte [cs:practice_power_valid], 0
    ret
.enabled:
    cmp byte [cs:practice_power_valid], 0
    jne .restore
    mov al, [POWER_BYTE]
    mov [cs:practice_power_value], al
    mov byte [cs:practice_power_valid], 1
    ret
.restore:
    mov al, [cs:practice_power_value]
    cmp [POWER_BYTE], al
    je .done
    mov [POWER_BYTE], al
    POWER_REFRESH
.done:
    ret
practice_power_valid: db 0
practice_power_value: db 0
