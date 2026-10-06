
// block 0043f0a0
0043f0a0  mov        edx, dword ptr [0x4ce8cc]
0043f0a6  push       esi
0043f0a7  mov        esi, eax
0043f0a9  mov        eax, dword ptr [edi + esi*4 + 0x2c8]
0043f0b0  push       eax
0043f0b1  call       0x461920

// block 0043f0b6
0043f0b6  test       eax, eax
0043f0b8  jne        0x43f0c1

// block 0043f0ba
0043f0ba  mov        dword ptr [edi + esi*4 + 0x2c8], eax

// block 0043f0c1
0043f0c1  mov        edx, dword ptr [esp + 8]
0043f0c5  mov        esi, eax
0043f0c7  call       0x462020

// block 0043f0cc
0043f0cc  mov        esi, eax
0043f0ce  mov        eax, dword ptr [esi + 0x494]
0043f0d4  test       eax, eax
0043f0d6  je         0x43f0df

// block 0043f0d8
0043f0d8  movsx      edx, bx
0043f0db  mov        ecx, esi
0043f0dd  call       eax

// block 0043f0df
0043f0df  push       esi
0043f0e0  mov        word ptr [esi + 0x3c4], bx
0043f0e7  call       0x455630

// block 0043f0ec
0043f0ec  pop        esi
0043f0ed  ret        4
