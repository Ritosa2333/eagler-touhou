
// block 0047e896
0047e896  mov        edi, edi
0047e898  push       ebp
0047e899  mov        ebp, esp
0047e89b  mov        eax, dword ptr [ebp + 8]
0047e89e  xor        edx, edx
0047e8a0  push       esi
0047e8a1  mov        esi, ecx
0047e8a3  mov        byte ptr [esi + 4], dl
0047e8a6  and        dword ptr [esi + 4], 0xffff00ff
0047e8ad  mov        dword ptr [esi], edx
0047e8af  xor        ecx, ecx
0047e8b1  cmp        byte ptr [eax], dl
0047e8b3  je         0x47e8bb

// block 0047e8b5
0047e8b5  inc        ecx
0047e8b6  cmp        byte ptr [ecx + eax], dl
0047e8b9  jne        0x47e8b5

// block 0047e8bb
0047e8bb  push       ecx
0047e8bc  push       eax
0047e8bd  mov        ecx, esi
0047e8bf  call       0x47e4f1

// block 0047e8c4
0047e8c4  mov        eax, esi
0047e8c6  pop        esi
0047e8c7  pop        ebp
0047e8c8  ret        4
