
// block 004916bf
004916bf  mov        edi, edi
004916c1  push       ebp
004916c2  mov        ebp, esp
004916c4  push       esi
004916c5  mov        esi, ecx
004916c7  push       0
004916c9  mov        dword ptr [esi + 0x18], 0xf
004916d0  call       0x44edf0

// block 004916d5
004916d5  push       dword ptr [ebp + 8]
004916d8  mov        ecx, esi
004916da  call       0x44ebd0

// block 004916df
004916df  mov        eax, esi
004916e1  pop        esi
004916e2  pop        ebp
004916e3  ret        4
