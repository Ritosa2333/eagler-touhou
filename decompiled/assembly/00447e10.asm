
// block 00447e10
00447e10  sub        esp, 0x18
00447e13  push       ebx
00447e14  push       ebp
00447e15  mov        ebp, dword ptr [esp + 0x24]
00447e19  mov        eax, dword ptr [ebp + 0x24]
00447e1c  push       esi
00447e1d  sub        eax, 2
00447e20  push       edi
00447e21  mov        eax, 1
00447e26  jne        0x4480a7

// block 00447e2c
00447e2c  fld        dword ptr [0x4a3f50]
00447e32  mov        edi, dword ptr [ebp + 0x100]
00447e38  mov        esi, dword ptr [0x4b43b8]
00447e3e  fstp       dword ptr [esp + 0x1c]
00447e42  fld        dword ptr [0x4a3f4c]
00447e48  mov        dword ptr [esi + 0x18f94], eax
00447e4e  cmp        dword ptr [ebp + 0x1d8], 0
00447e55  fstp       dword ptr [esp + 0x20]
00447e59  fldz       
00447e5b  mov        dword ptr [esp + 0x18], edi
00447e5f  fstp       dword ptr [esp + 0x24]
00447e63  jne        0x447f9e

// block 00447e69
00447e69  mov        dword ptr [esp + 0x2c], eax
00447e6d  imul       edi, edi, 0x118
00447e73  mov        eax, 0xff
00447e78  mov        dword ptr [esp + 0x10], eax
00447e7c  mov        dword ptr [esp + 0x14], 0xa
00447e84  jmp        0x447e8a

// block 00447e86
00447e86  mov        eax, dword ptr [esp + 0x10]

// block 00447e8a
00447e8a  mov        ecx, eax
00447e8c  shl        ecx, 8
00447e8f  or         ecx, eax
00447e91  shl        ecx, 8
00447e94  or         ecx, 0xff0000ff
00447e9a  mov        dword ptr [esi + 0x18f80], ecx
00447ea0  mov        eax, dword ptr [ebp + 0x28]
00447ea3  mov        ecx, dword ptr [0x4b451c]
00447ea9  imul       eax, eax, 0x45f4
00447eaf  lea        edx, [edi + eax]
00447eb2  mov        ebx, dword ptr [edx + ecx + 0x28]
00447eb6  or         ebx, dword ptr [edx + ecx + 0x2c]
00447eba  je         0x447f35

// block 00447ebc
00447ebc  lea        eax, [eax + ecx + 8]
00447ec0  lea        edx, [edi + eax + 0x20]
00447ec4  push       edx
00447ec5  call       0x46d8e4

// block 00447eca
00447eca  mov        ecx, dword ptr [ebp + 0x28]
00447ecd  mov        edx, dword ptr [0x4b451c]
00447ed3  imul       ecx, ecx, 0x45f4
00447ed9  add        ecx, edi
00447edb  add        ecx, edx
00447edd  movsx      edx, byte ptr [ecx + 0x1c]
00447ee1  fld        dword ptr [ecx + 0x30]
00447ee4  mov        edx, dword ptr [edx*4 + 0x4aee60]
00447eeb  push       ecx
00447eec  mov        esi, dword ptr [0x4b43b8]
00447ef2  add        ecx, 0x1e
00447ef5  fstp       qword ptr [esp]
00447ef8  push       edx
00447ef9  mov        edx, dword ptr [eax + 4]
00447efc  push       edx
00447efd  mov        edx, dword ptr [eax + 8]
00447f00  push       edx
00447f01  mov        edx, dword ptr [eax + 0xc]
00447f04  push       edx
00447f05  mov        edx, dword ptr [eax + 0x10]
00447f08  mov        eax, dword ptr [eax + 0x14]
00447f0b  inc        edx
00447f0c  push       edx
00447f0d  movsx      edx, byte ptr [ecx - 1]
00447f11  add        eax, 0x76c
00447f16  push       eax
00447f17  mov        eax, dword ptr [ecx - 6]
00447f1a  push       edx
00447f1b  push       eax
00447f1c  push       ecx
00447f1d  mov        ecx, dword ptr [esp + 0x58]
00447f21  push       ecx
00447f22  push       0x4a2014
00447f27  lea        ebx, [esp + 0x50]
00447f2b  call       0x4015c0

// block 00447f30
00447f30  add        esp, 0x34
00447f33  jmp        0x447f6f

// block 00447f35
00447f35  mov        edx, dword ptr [ebp + 0x28]
00447f38  imul       edx, edx, 0x45f4
00447f3e  add        edx, edi
00447f40  lea        eax, [edx + ecx]
00447f43  movsx      ecx, byte ptr [eax + 0x1d]
00447f47  fld        dword ptr [eax + 0x30]
00447f4a  mov        edx, dword ptr [eax + 0x18]
00447f4d  sub        esp, 8
00447f50  add        eax, 0x1e
00447f53  lea        ebx, [esp + 0x24]
00447f57  fstp       qword ptr [esp]
00447f5a  push       ecx
00447f5b  push       edx
00447f5c  push       eax
00447f5d  mov        eax, dword ptr [esp + 0x40]
00447f61  push       eax
00447f62  push       0x4a204c
00447f67  call       0x4015c0

// block 00447f6c
00447f6c  add        esp, 0x1c

// block 00447f6f
00447f6f  fld        dword ptr [esp + 0x20]
00447f73  sub        dword ptr [esp + 0x10], 0x10
00447f78  fadd       qword ptr [0x4a3f08]
00447f7e  mov        esi, dword ptr [0x4b43b8]
00447f84  mov        eax, 1
00447f89  add        dword ptr [esp + 0x2c], eax
00447f8d  add        edi, 0x1c
00447f90  fstp       dword ptr [esp + 0x20]
00447f94  sub        dword ptr [esp + 0x14], eax
00447f98  jne        0x447e86

// block 00447f9e
00447f9e  mov        edx, dword ptr [0x4b451c]
00447fa4  fld        dword ptr [0x4a3f60]
00447faa  or         edi, 0xffffffff
00447fad  fstp       dword ptr [esp + 0x1c]
00447fb1  fld        dword ptr [0x4a3f5c]
00447fb7  mov        dword ptr [esi + 0x18f80], edi
00447fbd  mov        ecx, dword ptr [ebp + 0x28]
00447fc0  fstp       dword ptr [esp + 0x20]
00447fc4  imul       ecx, ecx, 0x45f4
00447fca  mov        eax, dword ptr [ecx + edx + 0x590]
00447fd1  push       eax
00447fd2  push       0x4a2080
00447fd7  lea        ebx, [esp + 0x24]
00447fdb  call       0x4015c0

// block 00447fe0
00447fe0  mov        ecx, dword ptr [ebp + 0x28]
00447fe3  mov        edx, dword ptr [0x4b451c]
00447fe9  imul       ecx, ecx, 0x45f4
00447fef  fld        dword ptr [0x4a3f58]
00447ff5  fstp       dword ptr [esp + 0x28]
00447ff9  mov        ecx, dword ptr [ecx + edx + 0x594]
00448000  mov        eax, 0x88888889
00448005  imul       ecx
00448007  add        edx, ecx
00448009  sar        edx, 5
0044800c  mov        eax, edx
0044800e  shr        eax, 0x1f
00448011  add        eax, edx
00448013  cdq        
00448014  mov        esi, 0x3c
00448019  idiv       esi
0044801b  mov        eax, 0x91a2b3c5
00448020  push       edx
00448021  imul       ecx
00448023  add        edx, ecx
00448025  sar        edx, 0xb
00448028  mov        eax, edx
0044802a  shr        eax, 0x1f
0044802d  add        eax, edx
0044802f  cdq        
00448030  idiv       esi
00448032  mov        esi, dword ptr [0x4b43b8]
00448038  mov        eax, 0x9b583739
0044803d  push       edx
0044803e  imul       ecx
00448040  add        edx, ecx
00448042  sar        edx, 0x11
00448045  mov        eax, edx
00448047  shr        eax, 0x1f
0044804a  add        eax, edx
0044804c  push       eax
0044804d  push       0x4a2088
00448052  call       0x4015c0

// block 00448057
00448057  mov        ecx, dword ptr [ebp + 0x28]
0044805a  mov        edx, dword ptr [0x4b451c]
00448060  fld        dword ptr [0x4a3f54]
00448066  imul       ecx, ecx, 0x117d
0044806c  fstp       dword ptr [esp + 0x38]
00448070  add        ecx, dword ptr [esp + 0x30]
00448074  mov        esi, dword ptr [0x4b43b8]
0044807a  mov        eax, dword ptr [edx + ecx*4 + 0x598]
00448081  push       eax
00448082  push       0x4a2080
00448087  call       0x4015c0

// block 0044808c
0044808c  mov        eax, dword ptr [0x4b43b8]
00448091  mov        dword ptr [eax + 0x18f80], edi
00448097  mov        dword ptr [eax + 0x18f94], 0
004480a1  add        esp, 0x20
004480a4  lea        eax, [edi + 2]

// block 004480a7
004480a7  pop        edi
004480a8  pop        esi
004480a9  pop        ebp
004480aa  pop        ebx
004480ab  add        esp, 0x18
004480ae  ret        4
