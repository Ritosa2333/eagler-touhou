
// block 0047e79b
0047e79b  mov        edi, edi
0047e79d  push       ebp
0047e79e  mov        ebp, esp
0047e7a0  mov        eax, dword ptr [ecx]
0047e7a2  push       esi
0047e7a3  mov        esi, dword ptr [ebp + 8]
0047e7a6  push       dword ptr [ebp + 0xc]
0047e7a9  mov        dword ptr [esi], eax
0047e7ab  mov        eax, dword ptr [ecx + 4]
0047e7ae  mov        ecx, esi
0047e7b0  mov        dword ptr [esi + 4], eax
0047e7b3  call       0x47e4af

// block 0047e7b8
0047e7b8  mov        eax, esi
0047e7ba  pop        esi
0047e7bb  pop        ebp
0047e7bc  ret        8
