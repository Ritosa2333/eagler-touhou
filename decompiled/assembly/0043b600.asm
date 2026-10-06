
// block 0043b600
0043b600  push       -1
0043b602  push       0x49763b
0043b607  mov        eax, dword ptr fs:[0]
0043b60d  push       eax
0043b60e  push       ecx
0043b60f  push       esi
0043b610  mov        eax, dword ptr [0x4ad138]
0043b615  xor        eax, esp
0043b617  push       eax
0043b618  lea        eax, [esp + 0xc]
0043b61c  mov        dword ptr fs:[0], eax
0043b622  push       0x2e0
0043b627  call       0x46c9ea

// block 0043b62c
0043b62c  mov        esi, eax
0043b62e  add        esp, 4
0043b631  mov        dword ptr [esp + 8], esi
0043b635  mov        dword ptr [esp + 0x14], 0
0043b63d  test       esi, esi
0043b63f  je         0x43b66d

// block 0043b641
0043b641  push       0x43cd80
0043b646  push       0x43cd40
0043b64b  push       8
0043b64d  push       0x24
0043b64f  lea        eax, [esi + 0xa8]
0043b655  push       eax
0043b656  call       0x46ce49

// block 0043b65b
0043b65b  push       0x2e0
0043b660  push       0
0043b662  push       esi
0043b663  call       0x477420

// block 0043b668
0043b668  add        esp, 0xc
0043b66b  jmp        0x43b66f

// block 0043b66d
0043b66d  xor        esi, esi

// block 0043b66f
0043b66f  mov        eax, dword ptr [esp + 0x1c]
0043b673  push       esi
0043b674  mov        ecx, 0x4b43e8
0043b679  mov        dword ptr [esp + 0x18], 0xffffffff
0043b681  call       0x43ae80

// block 0043b686
0043b686  test       eax, eax
0043b688  je         0x43b6b2

// block 0043b68a
0043b68a  test       esi, esi
0043b68c  je         0x43b69d

// block 0043b68e
0043b68e  push       esi
0043b68f  call       0x43b450

// block 0043b694
0043b694  push       esi
0043b695  call       0x46ca4f

// block 0043b69a
0043b69a  add        esp, 4

// block 0043b69d
0043b69d  xor        eax, eax
0043b69f  mov        ecx, dword ptr [esp + 0xc]
0043b6a3  mov        dword ptr fs:[0], ecx
0043b6aa  pop        ecx
0043b6ab  pop        esi
0043b6ac  add        esp, 0x10
0043b6af  ret        4

// block 0043b6b2
0043b6b2  mov        eax, esi
0043b6b4  mov        ecx, dword ptr [esp + 0xc]
0043b6b8  mov        dword ptr fs:[0], ecx
0043b6bf  pop        ecx
0043b6c0  pop        esi
0043b6c1  add        esp, 0x10
0043b6c4  ret        4
