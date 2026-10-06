
// block 00402f40
00402f40  push       -1
00402f42  push       0x49766b
00402f47  mov        eax, dword ptr fs:[0]
00402f4d  push       eax
00402f4e  push       ecx
00402f4f  push       esi
00402f50  mov        eax, dword ptr [0x4ad138]
00402f55  xor        eax, esp
00402f57  push       eax
00402f58  lea        eax, [esp + 0xc]
00402f5c  mov        dword ptr fs:[0], eax
00402f62  push       0x3724
00402f67  call       0x46c9ea

// block 00402f6c
00402f6c  add        esp, 4
00402f6f  mov        dword ptr [esp + 8], eax
00402f73  mov        dword ptr [esp + 0x14], 0
00402f7b  test       eax, eax
00402f7d  je         0x402f89

// block 00402f7f
00402f7f  push       eax
00402f80  call       0x402970

// block 00402f85
00402f85  mov        esi, eax
00402f87  jmp        0x402f8b

// block 00402f89
00402f89  xor        esi, esi

// block 00402f8b
00402f8b  mov        eax, dword ptr [esp + 0x1c]
00402f8f  push       eax
00402f90  push       esi
00402f91  mov        dword ptr [esp + 0x1c], 0xffffffff
00402f99  call       0x402a40

// block 00402f9e
00402f9e  test       eax, eax
00402fa0  je         0x402fca

// block 00402fa2
00402fa2  test       esi, esi
00402fa4  je         0x402fb5

// block 00402fa6
00402fa6  push       esi
00402fa7  call       0x402cc0

// block 00402fac
00402fac  push       esi
00402fad  call       0x46ca4f

// block 00402fb2
00402fb2  add        esp, 4

// block 00402fb5
00402fb5  xor        eax, eax
00402fb7  mov        ecx, dword ptr [esp + 0xc]
00402fbb  mov        dword ptr fs:[0], ecx
00402fc2  pop        ecx
00402fc3  pop        esi
00402fc4  add        esp, 0x10
00402fc7  ret        4

// block 00402fca
00402fca  mov        eax, esi
00402fcc  mov        ecx, dword ptr [esp + 0xc]
00402fd0  mov        dword ptr fs:[0], ecx
00402fd7  pop        ecx
00402fd8  pop        esi
00402fd9  add        esp, 0x10
00402fdc  ret        4
