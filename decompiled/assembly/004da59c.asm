
// block 004da59c
004da59c  in         eax, 4
004da59e  ja         0x4da578

// block 004da5a0
004da5a0  lodsb      al, byte ptr [esi]
004da5a1  jo         0x4da5d1

// block 004da5a3
004da5a3  adc        ah, bl
004da5a5  xchg       ah, dh
004da5a7  leave      
