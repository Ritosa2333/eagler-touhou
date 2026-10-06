
// block 0040d860
0040d860  and        dword ptr [esi + 0x34], 0xfffffffe
0040d864  push       0xc0
0040d869  push       0
0040d86b  push       esi
0040d86c  call       0x477420

// block 0040d871
0040d871  or         dword ptr [esi], 2
0040d874  add        esp, 0xc
0040d877  mov        dword ptr [0x4b43cc], esi
0040d87d  mov        eax, esi
0040d87f  ret        
