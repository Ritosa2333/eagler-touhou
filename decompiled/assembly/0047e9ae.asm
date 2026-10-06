
// block 0047e9ae
0047e9ae  mov        edi, edi
0047e9b0  push       ebp
0047e9b1  mov        ebp, esp
0047e9b3  mov        eax, dword ptr [ecx]
0047e9b5  push       esi
0047e9b6  mov        esi, dword ptr [ebp + 8]
0047e9b9  push       dword ptr [ebp + 0xc]
0047e9bc  mov        dword ptr [esi], eax
0047e9be  mov        eax, dword ptr [ecx + 4]
0047e9c1  mov        ecx, esi
0047e9c3  mov        dword ptr [esi + 4], eax
0047e9c6  call       0x47e7bf

// block 0047e9cb
0047e9cb  mov        eax, esi
0047e9cd  pop        esi
0047e9ce  pop        ebp
0047e9cf  ret        8
