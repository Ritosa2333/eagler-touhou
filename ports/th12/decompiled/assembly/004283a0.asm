
// block 004283a0
004283a0  mov        ecx, dword ptr [eax + 0x18]
004283a3  test       ecx, ecx
004283a5  je         0x4283c7

// block 004283a7
004283a7  push       esi
004283a8  jmp        0x4283b0

// block 004283b0
004283b0  cmp        dword ptr [ecx + 0xc], 1
004283b4  mov        esi, dword ptr [ecx + 8]
004283b7  je         0x4283c0

// block 004283b9
004283b9  mov        edx, dword ptr [ecx]
004283bb  mov        eax, dword ptr [edx + 0xc]
004283be  call       eax

// block 004283c0
004283c0  mov        ecx, esi
004283c2  test       esi, esi
004283c4  jne        0x4283b0

// block 004283c6
004283c6  pop        esi

// block 004283c7
004283c7  mov        eax, 1
004283cc  ret        
