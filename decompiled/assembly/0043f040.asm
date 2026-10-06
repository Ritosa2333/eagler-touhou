
// block 0043f040
0043f040  mov        edx, dword ptr [0x4ce8cc]
0043f046  push       esi
0043f047  mov        esi, eax
0043f049  mov        eax, dword ptr [esi + 0x2c8]
0043f04f  push       eax
0043f050  call       0x461920

// block 0043f055
0043f055  test       eax, eax
0043f057  jne        0x43f05f

// block 0043f059
0043f059  mov        dword ptr [esi + 0x2c8], eax

// block 0043f05f
0043f05f  mov        edx, dword ptr [esp + 8]
0043f063  mov        esi, eax
0043f065  call       0x462020

// block 0043f06a
0043f06a  mov        esi, eax
0043f06c  mov        eax, dword ptr [esi + 0x494]
0043f072  mov        edx, 0x1d
0043f077  test       eax, eax
0043f079  je         0x43f08f

// block 0043f07b
0043f07b  mov        ecx, esi
0043f07d  call       eax

// block 0043f07f
0043f07f  mov        ecx, 0x1d
0043f084  mov        word ptr [esi + 0x3c4], cx
0043f08b  pop        esi
0043f08c  ret        4

// block 0043f08f
0043f08f  mov        word ptr [esi + 0x3c4], dx
0043f096  pop        esi
0043f097  ret        4
