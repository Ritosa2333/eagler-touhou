
// block 00406b20
00406b20  push       esi
00406b21  push       0x524
00406b26  call       0x46c9ea

// block 00406b2b
00406b2b  mov        esi, eax
00406b2d  add        esp, 4
00406b30  test       esi, esi
00406b32  je         0x406b62

// block 00406b34
00406b34  mov        eax, 0xfffffffe
00406b39  and        dword ptr [esi + 0x24], eax
00406b3c  and        dword ptr [esi + 0x38], eax
00406b3f  lea        ecx, [esi + 0x4c]
00406b42  call       0x4027e0

// block 00406b47
00406b47  push       0x524
00406b4c  push       0
00406b4e  push       esi
00406b4f  call       0x477420

// block 00406b54
00406b54  add        esp, 0xc
00406b57  or         dword ptr [esi], 2
00406b5a  mov        dword ptr [0x4b43c4], esi
00406b60  jmp        0x406b64

// block 00406b62
00406b62  xor        esi, esi

// block 00406b64
00406b64  push       esi
00406b65  call       0x406880

// block 00406b6a
00406b6a  test       eax, eax
00406b6c  je         0x406b85

// block 00406b6e
00406b6e  test       esi, esi
00406b70  je         0x406b81

// block 00406b72
00406b72  push       esi
00406b73  call       0x406930

// block 00406b78
00406b78  push       esi
00406b79  call       0x46ca4f

// block 00406b7e
00406b7e  add        esp, 4

// block 00406b81
00406b81  xor        eax, eax
00406b83  pop        esi
00406b84  ret        

// block 00406b85
00406b85  mov        eax, esi
00406b87  pop        esi
00406b88  ret        
