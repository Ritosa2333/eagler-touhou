
// block 00476bb1
00476bb1  mov        edi, edi
00476bb3  push       ebp
00476bb4  mov        ebp, esp
00476bb6  sub        esp, 0xc
00476bb9  and        dword ptr [ebp - 4], 0
00476bbd  cmp        dword ptr [ebp + 0xc], 1
00476bc1  push       ebx
00476bc2  push       esi
00476bc3  push       edi
00476bc4  mov        edi, dword ptr [ebp + 0x10]
00476bc7  mov        esi, eax
00476bc9  mov        eax, edi
00476bcb  jne        0x476cca

// block 00476bd1
00476bd1  and        eax, 0x80000003
00476bd6  jns        0x476bdd

// block 00476bd8
00476bd8  dec        eax
00476bd9  or         eax, 0xfffffffc
00476bdc  inc        eax

// block 00476bdd
00476bdd  mov        dword ptr [ebp - 0xc], eax
00476be0  jne        0x476bee

// block 00476be2
00476be2  mov        eax, edi
00476be4  push       0x64
00476be6  cdq        
00476be7  pop        ebx
00476be8  idiv       ebx
00476bea  test       edx, edx
00476bec  jne        0x476c0d

// block 00476bee
00476bee  lea        eax, [edi + 0x76c]
00476bf4  cdq        
00476bf5  mov        ebx, 0x190
00476bfa  idiv       ebx
00476bfc  test       edx, edx
00476bfe  je         0x476c0d

// block 00476c00
00476c00  mov        eax, esi
00476c02  shl        eax, 2
00476c05  mov        esi, dword ptr [eax + 0x4ae2cc]
00476c0b  jmp        0x476c18

// block 00476c0d
00476c0d  mov        eax, esi
00476c0f  shl        eax, 2
00476c12  mov        esi, dword ptr [eax + 0x4ae298]

// block 00476c18
00476c18  mov        dword ptr [ebp + 0xc], eax
00476c1b  lea        eax, [edi + 0x12b]
00476c21  cdq        
00476c22  lea        ebx, [edi - 1]
00476c25  mov        edi, 0x190
00476c2a  idiv       edi
00476c2c  push       0x64
00476c2e  pop        edi
00476c2f  inc        esi
00476c30  push       7
00476c32  mov        dword ptr [ebp - 8], eax
00476c35  mov        eax, ebx
00476c37  cdq        
00476c38  idiv       edi
00476c3a  mov        edx, dword ptr [ebp - 8]
00476c3d  mov        edi, dword ptr [ebp + 0x10]
00476c40  sub        edx, eax
00476c42  mov        ebx, edx
00476c44  lea        eax, [edi - 1]
00476c47  cdq        
00476c48  and        edx, 3
00476c4b  add        eax, edx
00476c4d  sar        eax, 2
00476c50  mov        edx, edi
00476c52  imul       edx, edx, 0x16d
00476c58  add        eax, esi
00476c5a  add        eax, ebx
00476c5c  lea        eax, [edx + eax - 0x63db]
00476c63  cdq        
00476c64  pop        ebx
00476c65  idiv       ebx
00476c67  mov        eax, dword ptr [ebp + 0x14]
00476c6a  imul       eax, eax, 7
00476c6d  sub        eax, edx
00476c6f  add        eax, dword ptr [ebp + 0x18]
00476c72  cmp        edx, dword ptr [ebp + 0x18]
00476c75  jg         0x476c7d

// block 00476c77
00476c77  lea        esi, [esi + eax - 7]
00476c7b  jmp        0x476c7f

// block 00476c7d
00476c7d  add        esi, eax

// block 00476c7f
00476c7f  cmp        dword ptr [ebp + 0x14], 5
00476c83  jne        0x476d09

// block 00476c89
00476c89  cmp        dword ptr [ebp - 0xc], 0
00476c8d  jne        0x476c9b

// block 00476c8f
00476c8f  mov        eax, edi
00476c91  push       0x64
00476c93  cdq        
00476c94  pop        ebx
00476c95  idiv       ebx
00476c97  test       edx, edx
00476c99  jne        0x476cb8

// block 00476c9b
00476c9b  lea        eax, [edi + 0x76c]
00476ca1  cdq        
00476ca2  mov        ebx, 0x190
00476ca7  idiv       ebx
00476ca9  test       edx, edx
00476cab  je         0x476cb8

// block 00476cad
00476cad  mov        eax, dword ptr [ebp + 0xc]
00476cb0  mov        eax, dword ptr [eax + 0x4ae2d0]
00476cb6  jmp        0x476cc1

// block 00476cb8
00476cb8  mov        eax, dword ptr [ebp + 0xc]
00476cbb  mov        eax, dword ptr [eax + 0x4ae29c]

// block 00476cc1
00476cc1  cmp        esi, eax
00476cc3  jle        0x476d09

// block 00476cc5
00476cc5  sub        esi, 7
00476cc8  jmp        0x476d09

// block 00476cca
00476cca  and        eax, 0x80000003
00476ccf  jns        0x476cd6

// block 00476cd1
00476cd1  dec        eax
00476cd2  or         eax, 0xfffffffc
00476cd5  inc        eax

// block 00476cd6
00476cd6  jne        0x476ce4

// block 00476cd8
00476cd8  mov        eax, edi
00476cda  push       0x64
00476cdc  cdq        
00476cdd  pop        ebx
00476cde  idiv       ebx
00476ce0  test       edx, edx
00476ce2  jne        0x476cff

// block 00476ce4
00476ce4  lea        eax, [edi + 0x76c]
00476cea  cdq        
00476ceb  mov        ebx, 0x190
00476cf0  idiv       ebx
00476cf2  test       edx, edx
00476cf4  je         0x476cff

// block 00476cf6
00476cf6  mov        esi, dword ptr [esi*4 + 0x4ae2cc]
00476cfd  jmp        0x476d06

// block 00476cff
00476cff  mov        esi, dword ptr [esi*4 + 0x4ae298]

// block 00476d06
00476d06  add        esi, dword ptr [ebp + 0x1c]

// block 00476d09
00476d09  imul       ecx, ecx, 0x3c
00476d0c  add        ecx, dword ptr [ebp + 0x20]
00476d0f  imul       ecx, ecx, 0x3c
00476d12  add        ecx, dword ptr [ebp + 0x24]
00476d15  imul       ecx, ecx, 0x3e8
00476d1b  add        ecx, dword ptr [ebp + 0x28]
00476d1e  cmp        dword ptr [ebp + 8], 1
00476d22  jne        0x476d38

// block 00476d24
00476d24  mov        dword ptr [0x4adae4], esi
00476d2a  mov        dword ptr [0x4adae8], ecx
00476d30  mov        dword ptr [0x4adae0], edi
00476d36  jmp        0x476da3

// block 00476d38
00476d38  lea        eax, [ebp - 4]
00476d3b  push       eax
00476d3c  mov        dword ptr [0x4adaf0], esi
00476d42  mov        dword ptr [0x4adaf4], ecx
00476d48  call       0x4772eb

// block 00476d4d
00476d4d  pop        ecx
00476d4e  test       eax, eax
00476d50  je         0x476d61

// block 00476d52
00476d52  xor        eax, eax
00476d54  push       eax
00476d55  push       eax
00476d56  push       eax
00476d57  push       eax
00476d58  push       eax
00476d59  call       0x470dc6

// block 00476d5e
00476d5e  add        esp, 0x14

// block 00476d61
00476d61  mov        eax, dword ptr [ebp - 4]
00476d64  imul       eax, eax, 0x3e8
00476d6a  add        dword ptr [0x4adaf4], eax
00476d70  jns        0x476d84

// block 00476d72
00476d72  add        dword ptr [0x4adaf4], 0x5265c00
00476d7c  dec        dword ptr [0x4adaf0]
00476d82  jmp        0x476d9d

// block 00476d84
00476d84  mov        eax, 0x5265c00
00476d89  cmp        dword ptr [0x4adaf4], eax
00476d8f  jl         0x476d9d

// block 00476d91
00476d91  sub        dword ptr [0x4adaf4], eax
00476d97  inc        dword ptr [0x4adaf0]

// block 00476d9d
00476d9d  mov        dword ptr [0x4adaec], edi

// block 00476da3
00476da3  pop        edi
00476da4  pop        esi
00476da5  pop        ebx
00476da6  leave      
00476da7  ret        
