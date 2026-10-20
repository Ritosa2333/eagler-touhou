
// block 004604a0
004604a0  cmp        esi, 0x20
004604a3  jae        0x4604d2

// block 004604a5
004604a5  push       edi
004604a6  mov        edi, dword ptr [ebx + esi*4 + 0x4b50c0]
004604ad  test       edi, edi
004604af  je         0x4604d1

// block 004604b1
004604b1  call       0x4604e0

// block 004604b6
004604b6  mov        eax, dword ptr [ebx + esi*4 + 0x4b50c0]
004604bd  push       eax
004604be  call       0x46ca4f

// block 004604c3
004604c3  add        esp, 4
004604c6  mov        dword ptr [ebx + esi*4 + 0x4b50c0], 0

// block 004604d1
004604d1  pop        edi

// block 004604d2
004604d2  ret        
