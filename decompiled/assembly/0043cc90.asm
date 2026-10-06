
// block 0043cc90
0043cc90  mov        eax, dword ptr [esi + 0x18a8]
0043cc96  xor        ecx, ecx
0043cc98  cmp        eax, ecx
0043cc9a  je         0x43cca5

// block 0043cc9c
0043cc9c  mov        edx, dword ptr [esi + 0x18ac]
0043cca2  mov        dword ptr [eax + 8], edx

// block 0043cca5
0043cca5  mov        eax, dword ptr [esi + 0x18ac]
0043ccab  cmp        eax, ecx
0043ccad  je         0x43ccb8

// block 0043ccaf
0043ccaf  mov        edx, dword ptr [esi + 0x18a8]
0043ccb5  mov        dword ptr [eax + 4], edx

// block 0043ccb8
0043ccb8  push       esi
0043ccb9  mov        dword ptr [esi + 0x18a8], ecx
0043ccbf  mov        dword ptr [esi + 0x18ac], ecx
0043ccc5  call       0x46ca4f

// block 0043ccca
0043ccca  add        esp, 4
0043cccd  mov        eax, esi
0043cccf  ret        
