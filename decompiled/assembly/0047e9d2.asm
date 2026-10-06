
// block 0047e9d2
0047e9d2  mov        edi, edi
0047e9d4  push       ebp
0047e9d5  mov        ebp, esp
0047e9d7  mov        eax, dword ptr [ecx]
0047e9d9  push       esi
0047e9da  mov        esi, dword ptr [ebp + 8]
0047e9dd  push       dword ptr [ebp + 0xc]
0047e9e0  mov        dword ptr [esi], eax
0047e9e2  mov        eax, dword ptr [ecx + 4]
0047e9e5  mov        ecx, esi
0047e9e7  mov        dword ptr [esi + 4], eax
0047e9ea  call       0x47e802

// block 0047e9ef
0047e9ef  mov        eax, esi
0047e9f1  pop        esi
0047e9f2  pop        ebp
0047e9f3  ret        8
