
// block 00410c60
00410c60  push       -1
00410c62  push       0x4975ab
00410c67  mov        eax, dword ptr fs:[0]
00410c6d  push       eax
00410c6e  sub        esp, 0xc
00410c71  push       ebx
00410c72  push       ebp
00410c73  push       esi
00410c74  push       edi
00410c75  mov        eax, dword ptr [0x4ad138]
00410c7a  xor        eax, esp
00410c7c  push       eax
00410c7d  lea        eax, [esp + 0x20]
00410c81  mov        dword ptr fs:[0], eax
00410c87  mov        ebp, dword ptr [esp + 0x30]
00410c8b  push       0x24
00410c8d  call       0x46c9ea

// block 00410c92
00410c92  xor        ecx, ecx
00410c94  add        esp, 4
00410c97  cmp        eax, ecx
00410c99  je         0x410cb7

// block 00410c9b
00410c9b  and        dword ptr [eax + 4], 0xfffffffe
00410c9f  mov        dword ptr [eax + 8], ecx
00410ca2  mov        dword ptr [eax + 0xc], ecx
00410ca5  mov        dword ptr [eax + 0x10], ecx
00410ca8  mov        dword ptr [eax], ecx
00410caa  mov        dword ptr [eax + 0x14], eax
00410cad  mov        dword ptr [eax + 0x18], ecx
00410cb0  mov        dword ptr [eax + 0x1c], ecx
00410cb3  mov        esi, eax
00410cb5  jmp        0x410cb9

// block 00410cb7
00410cb7  xor        esi, esi

// block 00410cb9
00410cb9  mov        edi, 3
00410cbe  or         dword ptr [esi + 4], edi
00410cc1  lea        ebx, [edi + 0x1a]
00410cc4  mov        dword ptr [esi + 8], 0x411190
00410ccb  mov        dword ptr [esi + 0xc], ecx
00410cce  mov        dword ptr [esi + 0x10], ecx
00410cd1  mov        dword ptr [esi + 0x20], ebp
00410cd4  call       0x462380

// block 00410cd9
00410cd9  push       0x24
00410cdb  mov        dword ptr [ebp + 8], esi
00410cde  call       0x46c9ea

// block 00410ce3
00410ce3  xor        ecx, ecx
00410ce5  add        esp, 4
00410ce8  cmp        eax, ecx
00410cea  je         0x410d08

// block 00410cec
00410cec  and        dword ptr [eax + 4], 0xfffffffe
00410cf0  mov        dword ptr [eax + 8], ecx
00410cf3  mov        dword ptr [eax + 0xc], ecx
00410cf6  mov        dword ptr [eax + 0x10], ecx
00410cf9  mov        dword ptr [eax], ecx
00410cfb  mov        dword ptr [eax + 0x14], eax
00410cfe  mov        dword ptr [eax + 0x18], ecx
00410d01  mov        dword ptr [eax + 0x1c], ecx
00410d04  mov        esi, eax
00410d06  jmp        0x410d0a

// block 00410d08
00410d08  xor        esi, esi

// block 00410d0a
00410d0a  or         dword ptr [esi + 4], edi
00410d0d  mov        ebx, 0x36
00410d12  mov        dword ptr [esi + 8], 0x4111a0
00410d19  mov        dword ptr [esi + 0xc], ecx
00410d1c  mov        dword ptr [esi + 0x10], ecx
00410d1f  mov        dword ptr [esi + 0x20], ebp
00410d22  call       0x462420

// block 00410d27
00410d27  fld        dword ptr [0x4a3ec0]
00410d2d  mov        eax, dword ptr [0x4b43b8]
00410d32  fstp       dword ptr [esp + 0x14]
00410d36  fld        dword ptr [0x4a4260]
00410d3c  mov        dword ptr [ebp + 0xc], esi
00410d3f  fstp       dword ptr [esp + 0x18]
00410d43  lea        esi, [eax + 0x18fbc]
00410d49  fldz       
00410d4b  xor        ebx, ebx
00410d4d  fstp       dword ptr [esp + 0x1c]
00410d51  cmp        dword ptr [esi], ebx
00410d53  jne        0x410d76

// block 00410d55
00410d55  mov        eax, dword ptr [eax + 0x18fb4]
00410d5b  push       ebx
00410d5c  push       0x12
00410d5e  lea        ecx, [esp + 0x38]
00410d62  push       ecx
00410d63  push       eax
00410d64  lea        eax, [ebx + 0x17]
00410d67  lea        ecx, [esp + 0x24]
00410d6b  call       0x4615a0

// block 00410d70
00410d70  mov        edx, dword ptr [esp + 0x30]
00410d74  mov        dword ptr [esi], edx

// block 00410d76
00410d76  mov        eax, dword ptr [0x4b0c94]
00410d7b  mov        ecx, dword ptr [0x4b0c90]
00410d81  lea        eax, [eax + ecx*2]
00410d84  mov        dword ptr [ebp + 0x1c], eax
00410d87  cmp        dword ptr [0x4b0cc4], ebx
00410d8d  jne        0x410d95

// block 00410d8f
00410d8f  add        eax, 6
00410d92  mov        dword ptr [ebp + 0x1c], eax

// block 00410d95
00410d95  mov        ecx, dword ptr [ebp + 0x1c]
00410d98  mov        eax, dword ptr [0x4b451c]
00410d9d  mov        edx, 1
00410da2  cmp        byte ptr [ecx + eax + 0x1e9ca], bl
00410da9  jne        0x410dae

// block 00410dab
00410dab  or         dword ptr [ebp + 0x20], edx

// block 00410dae
00410dae  cmp        byte ptr [eax + 0x1e9d6], bl
00410db4  jne        0x410dba

// block 00410db6
00410db6  or         dword ptr [ebp + 0x20], 2

// block 00410dba
00410dba  or         byte ptr [ecx + eax + 0x1e9ca], dl
00410dc1  cmp        dword ptr [0x4b0ca8], edx
00410dc7  lea        ecx, [ecx + eax + 0x1e9ca]
00410dce  jl         0x410de2

// block 00410dd0
00410dd0  mov        ecx, dword ptr [ebp + 0x1c]
00410dd3  or         byte ptr [ecx + eax + 0x1e9ca], 0x10
00410ddb  lea        ecx, [ecx + eax + 0x1e9ca]

// block 00410de2
00410de2  cmp        dword ptr [ebp + 0x1c], 6
00410de6  jl         0x410dee

// block 00410de8
00410de8  mov        byte ptr [eax + 0x1e9d6], dl

// block 00410dee
00410dee  mov        edx, dword ptr [ebp + 0x1c]
00410df1  mov        eax, dword ptr [edx*4 + 0x4af128]
00410df8  mov        byte ptr [0x4d4f38], bl
00410dfe  mov        ecx, eax

// block 00410e00
00410e00  mov        dl, byte ptr [eax]
00410e02  inc        eax
00410e03  cmp        dl, bl
00410e05  jne        0x410e00

// block 00410e07
00410e07  mov        edi, 0x4d4f38
00410e0c  sub        eax, ecx
00410e0e  mov        esi, ecx
00410e10  dec        edi

// block 00410e11
00410e11  mov        cl, byte ptr [edi + 1]
00410e14  inc        edi
00410e15  cmp        cl, bl
00410e17  jne        0x410e11

// block 00410e19
00410e19  mov        ecx, eax
00410e1b  shr        ecx, 2

// block 00410e1e
00410e1e  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00410e20
00410e20  mov        ecx, eax
00410e22  push       ebx
00410e23  and        ecx, 3
00410e26  push       ebx
00410e27  mov        eax, 0x4d4f38

// block 00410e2c
00410e2c  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 00410e2e
00410e2e  call       0x463c10

// block 00410e33
00410e33  mov        dword ptr [ebp + 0x14], eax
00410e36  cmp        eax, ebx
00410e38  jne        0x410e51

// block 00410e3a
00410e3a  push       0x49f434
00410e3f  mov        ecx, 0x4b0ec8
00410e44  call       0x464220

// block 00410e49
00410e49  add        esp, 4
00410e4c  or         eax, 0xffffffff
00410e4f  jmp        0x410e82

// block 00410e51
00410e51  push       0xf0
00410e56  call       0x46c9ea

// block 00410e5b
00410e5b  add        esp, 4
00410e5e  mov        dword ptr [esp + 0x30], eax
00410e62  mov        dword ptr [esp + 0x28], ebx
00410e66  cmp        eax, ebx
00410e68  je         0x410e7b

// block 00410e6a
00410e6a  mov        ecx, dword ptr [ebp + 0x14]
00410e6d  mov        edx, dword ptr [ecx + 4]
00410e70  add        edx, ecx
00410e72  push       edx
00410e73  push       eax
00410e74  call       0x4111b0

// block 00410e79
00410e79  jmp        0x410e7d

// block 00410e7b
00410e7b  xor        eax, eax

// block 00410e7d
00410e7d  mov        dword ptr [ebp + 0x18], eax
00410e80  xor        eax, eax

// block 00410e82
00410e82  mov        ecx, dword ptr [esp + 0x20]
00410e86  mov        dword ptr fs:[0], ecx
00410e8d  pop        ecx
00410e8e  pop        edi
00410e8f  pop        esi
00410e90  pop        ebp
00410e91  pop        ebx
00410e92  add        esp, 0x18
00410e95  ret        4
