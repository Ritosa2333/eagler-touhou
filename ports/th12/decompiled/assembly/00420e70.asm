
// block 00420e70
00420e70  push       ebx
00420e71  mov        ebx, dword ptr [0x4b0c44]
00420e77  push       esi
00420e78  mov        esi, dword ptr [0x4b43e4]
00420e7e  push       edi
00420e7f  mov        edi, esi
00420e81  mov        ecx, dword ptr [edi + 0x6cdc]
00420e87  cmp        ebx, ecx
00420e89  je         0x420ef9

// block 00420e8b
00420e8b  mov        eax, ebx
00420e8d  sub        eax, ecx
00420e8f  cdq        
00420e90  and        edx, 0x1f
00420e93  add        eax, edx
00420e95  sar        eax, 5
00420e98  cmp        eax, 0x8d55e
00420e9d  jl         0x420ea6

// block 00420e9f
00420e9f  mov        eax, 0x8d55e
00420ea4  jmp        0x420eaf

// block 00420ea6
00420ea6  test       eax, eax
00420ea8  jne        0x420eaf

// block 00420eaa
00420eaa  mov        eax, 1

// block 00420eaf
00420eaf  cmp        dword ptr [edi + 0x6ce0], eax
00420eb5  jge        0x420ec3

// block 00420eb7
00420eb7  mov        dword ptr [edi + 0x6ce0], eax
00420ebd  mov        ebx, dword ptr [0x4b0c44]

// block 00420ec3
00420ec3  mov        eax, ebx
00420ec5  sub        eax, ecx
00420ec7  cmp        dword ptr [edi + 0x6ce0], eax
00420ecd  jle        0x420ed5

// block 00420ecf
00420ecf  mov        dword ptr [edi + 0x6ce0], eax

// block 00420ed5
00420ed5  mov        eax, dword ptr [edi + 0x6ce0]
00420edb  add        dword ptr [edi + 0x6cdc], eax
00420ee1  mov        eax, dword ptr [edi + 0x6cdc]
00420ee7  cmp        eax, dword ptr [0x4b0c44]
00420eed  jl         0x420ef9

// block 00420eef
00420eef  mov        dword ptr [edi + 0x6ce0], 0

// block 00420ef9
00420ef9  mov        eax, dword ptr [edi + 0x6cdc]
00420eff  cmp        dword ptr [0x4b0c40], eax
00420f05  jge        0x420f35

// block 00420f07
00420f07  mov        ecx, dword ptr [0x4b0cc4]
00420f0d  mov        dword ptr [0x4b0c40], eax
00420f12  mov        eax, dword ptr [0x4b0ce0]
00420f17  or         eax, 4
00420f1a  mov        dword ptr [0x4b0cc8], ecx
00420f20  mov        dword ptr [0x4b0ce0], eax
00420f25  test       al, 4
00420f27  jne        0x420f35

// block 00420f29
00420f29  push       0
00420f2b  mov        eax, 3
00420f30  call       0x420f90

// block 00420f35
00420f35  test       byte ptr [edi + 0x6d18], 0x20
00420f3c  jne        0x420f80

// block 00420f3e
00420f3e  cmp        dword ptr [0x4b0ca8], 4
00420f45  je         0x420f5b

// block 00420f47
00420f47  mov        edx, dword ptr [edi + 0x6cdc]
00420f4d  mov        eax, dword ptr [0x4b0cd8]
00420f52  cmp        edx, dword ptr [eax*4 + 0x4aeeb4]
00420f59  jmp        0x420f6e

// block 00420f5b
00420f5b  mov        ecx, dword ptr [edi + 0x6cdc]
00420f61  mov        edx, dword ptr [0x4b0cd8]
00420f67  cmp        ecx, dword ptr [edx*4 + 0x4af0f8]

// block 00420f6e
00420f6e  jl         0x420f80

// block 00420f70
00420f70  mov        eax, 0x4b0c40
00420f75  call       0x422ce0

// block 00420f7a
00420f7a  inc        dword ptr [0x4b0cd8]

// block 00420f80
00420f80  pop        edi
00420f81  pop        esi
00420f82  pop        ebx
00420f83  ret        
