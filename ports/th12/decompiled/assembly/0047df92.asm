
// block 0047df92
0047df92  mov        edi, edi
0047df94  push       esi
0047df95  mov        esi, ecx
0047df97  cmp        dword ptr [esi + 0xc], 0
0047df9b  jge        0x47dfb8

// block 0047df9d
0047df9d  mov        ecx, dword ptr [esi + 8]
0047dfa0  mov        eax, dword ptr [ecx]
0047dfa2  push       ebx
0047dfa3  push       edi
0047dfa4  mov        edi, dword ptr [esi + 4]
0047dfa7  call       dword ptr [eax]

// block 0047dfa9
0047dfa9  mov        ebx, eax
0047dfab  mov        eax, dword ptr [edi]
0047dfad  mov        ecx, edi
0047dfaf  call       dword ptr [eax]

// block 0047dfb1
0047dfb1  add        ebx, eax
0047dfb3  pop        edi
0047dfb4  mov        dword ptr [esi + 0xc], ebx
0047dfb7  pop        ebx

// block 0047dfb8
0047dfb8  mov        eax, dword ptr [esi + 0xc]
0047dfbb  pop        esi
0047dfbc  ret        
