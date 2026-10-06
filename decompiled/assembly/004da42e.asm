
// block 004da42e
004da42e  lodsd      eax, dword ptr [esi]
004da42f  inc        ebx
004da430  loopne     0x4da3da

// block 004da432
004da432  jno        0x4da419

// block 004da434
004da434  loope      0x4da46e

// block 004da436
004da436  inc        esp
004da437  mov        ecx, 0xee1b2a83
004da43c  sal        byte ptr [ecx + edi*8 - 0x65], 1
004da440  lahf       
004da441  cmp        esp, dword ptr [edi + esi*8]
004da444  cmpsd      dword ptr [esi], dword ptr es:[edi]
004da445  mov        cl, 0xe9

// block 004da46e
004da46e  and        dword ptr [edi], esi
004da470  cwde       
004da471  add        ch, byte ptr [0x9fc98642]
004da477  int3       
