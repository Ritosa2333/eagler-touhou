
// block 00425b10
00425b10  push       -1
00425b12  push       0x4977ab
00425b17  mov        eax, dword ptr fs:[0]
00425b1d  push       eax
00425b1e  push       ecx
00425b1f  push       esi
00425b20  mov        eax, dword ptr [0x4ad138]
00425b25  xor        eax, esp
00425b27  push       eax
00425b28  lea        eax, [esp + 0xc]
00425b2c  mov        dword ptr fs:[0], eax
00425b32  push       0x666fe4
00425b37  call       0x46c9ea

// block 00425b3c
00425b3c  mov        esi, eax
00425b3e  add        esp, 4
00425b41  mov        dword ptr [esp + 8], esi
00425b45  mov        dword ptr [esp + 0x14], 0
00425b4d  test       esi, esi
00425b4f  je         0x425b89

// block 00425b51
00425b51  push       0x425900
00425b56  push       0x4258d0
00425b5b  push       0xa68
00425b60  push       0x9d8
00425b65  lea        eax, [esi + 0x14]
00425b68  push       eax
00425b69  call       0x46ce49

// block 00425b6e
00425b6e  push       0x666fe4
00425b73  push       0
00425b75  push       esi
00425b76  call       0x477420

// block 00425b7b
00425b7b  add        esp, 0xc
00425b7e  or         dword ptr [esi], 2
00425b81  mov        dword ptr [0x4b44f0], esi
00425b87  jmp        0x425b8b

// block 00425b89
00425b89  xor        esi, esi

// block 00425b8b
00425b8b  push       esi
00425b8c  mov        dword ptr [esp + 0x18], 0xffffffff
00425b94  call       0x425940

// block 00425b99
00425b99  test       eax, eax
00425b9b  je         0x425bc3

// block 00425b9d
00425b9d  test       esi, esi
00425b9f  je         0x425bb0

// block 00425ba1
00425ba1  push       esi
00425ba2  call       0x425a00

// block 00425ba7
00425ba7  push       esi
00425ba8  call       0x46ca4f

// block 00425bad
00425bad  add        esp, 4

// block 00425bb0
00425bb0  xor        eax, eax
00425bb2  mov        ecx, dword ptr [esp + 0xc]
00425bb6  mov        dword ptr fs:[0], ecx
00425bbd  pop        ecx
00425bbe  pop        esi
00425bbf  add        esp, 0x10
00425bc2  ret        

// block 00425bc3
00425bc3  mov        eax, esi
00425bc5  mov        ecx, dword ptr [esp + 0xc]
00425bc9  mov        dword ptr fs:[0], ecx
00425bd0  pop        ecx
00425bd1  pop        esi
00425bd2  add        esp, 0x10
00425bd5  ret        
