
// block 00459470
00459470  mov        eax, dword ptr [eax + 0x3f4]
00459476  mov        ecx, dword ptr [0x4ce8cc]
0045947c  test       eax, eax
0045947e  jne        0x459483

// block 00459480
00459480  xor        eax, eax
00459482  ret        

// block 00459483
00459483  mov        eax, dword ptr [eax]
00459485  test       eax, eax
00459487  jl         0x459480

// block 00459489
00459489  cmp        dword ptr [ecx + eax*4 + 0x4b50c0], 0
00459491  je         0x459480

// block 00459493
00459493  mov        ecx, dword ptr [ecx + eax*4 + 0x4b50c0]
0045949a  xor        eax, eax
0045949c  cmp        dword ptr [ecx + 0x120], eax
004594a2  setne      al
004594a5  ret        
