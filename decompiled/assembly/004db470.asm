
// block 004db470
004db470  salc       

// block 004db475
004db475  mov        bl, 0x99
004db478  inc        edx
004db479  test       byte ptr [esi], cl

// block 004db47b
004db47b  wait       
004db47d  adc        ah, byte ptr [esi]
004db47f  or         byte ptr [ebp - 0x4e], ch
004db482  movsb      byte ptr es:[edi], byte ptr [esi]
004db483  adc        ch, byte ptr [ebp + 0x72dd8a7f]
004db489  mov        ecx, 0x24bacf71

// block 004db490
004db490  inc        ebx
004db491  jmp        0x4db47b
