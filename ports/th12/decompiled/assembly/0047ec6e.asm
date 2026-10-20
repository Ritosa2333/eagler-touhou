
// block 0047ec6e
0047ec6e  mov        edi, edi
0047ec70  push       ebp
0047ec71  mov        ebp, esp
0047ec73  mov        eax, dword ptr [ecx]
0047ec75  push       esi
0047ec76  mov        esi, dword ptr [ebp + 8]
0047ec79  push       dword ptr [ebp + 0xc]
0047ec7c  mov        dword ptr [esi], eax
0047ec7e  mov        eax, dword ptr [ecx + 4]
0047ec81  mov        ecx, esi
0047ec83  mov        dword ptr [esi + 4], eax
0047ec86  call       0x47e9f6

// block 0047ec8b
0047ec8b  mov        eax, esi
0047ec8d  pop        esi
0047ec8e  pop        ebp
0047ec8f  ret        8
