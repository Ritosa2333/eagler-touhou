
// block 00469730
00469730  mov        eax, dword ptr [esi + 0x102c]
00469736  push       ecx
00469737  call       0x4694f0

// block 0046973c
0046973c  fldz       
0046973e  mov        edx, dword ptr [esi + 4]
00469741  mov        dword ptr [edx + 4], eax
00469744  mov        eax, dword ptr [esi + 4]
00469747  fstp       dword ptr [eax]
00469749  xor        eax, eax
0046974b  ret        
