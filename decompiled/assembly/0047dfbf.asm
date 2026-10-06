
// block 0047dfbf
0047dfbf  push       esi
0047dfc0  mov        esi, ecx
0047dfc2  mov        ecx, dword ptr [esi + 8]
0047dfc5  mov        eax, dword ptr [ecx]
0047dfc7  call       dword ptr [eax + 4]

// block 0047dfca
0047dfca  test       al, al
0047dfcc  jne        0x47dfd7

// block 0047dfce
0047dfce  mov        ecx, dword ptr [esi + 4]
0047dfd1  mov        eax, dword ptr [ecx]
0047dfd3  pop        esi
0047dfd4  jmp        dword ptr [eax + 4]

// block 0047dfd7
0047dfd7  pop        esi
0047dfd8  ret        
