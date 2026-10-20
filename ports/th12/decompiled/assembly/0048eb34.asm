
// block 0048eb34
0048eb34  mov        eax, dword ptr [0x4ae424]
0048eb39  push       esi
0048eb3a  mov        esi, dword ptr [0x4980a4]
0048eb40  cmp        eax, -1
0048eb43  je         0x48eb4d

// block 0048eb45
0048eb45  cmp        eax, -2
0048eb48  je         0x48eb4d

// block 0048eb4a
0048eb4a  push       eax
0048eb4b  call       esi

// block 0048eb4d
0048eb4d  mov        eax, dword ptr [0x4ae420]
0048eb52  cmp        eax, -1
0048eb55  je         0x48eb5f

// block 0048eb57
0048eb57  cmp        eax, -2
0048eb5a  je         0x48eb5f

// block 0048eb5c
0048eb5c  push       eax
0048eb5d  call       esi

// block 0048eb5f
0048eb5f  pop        esi
0048eb60  ret        
