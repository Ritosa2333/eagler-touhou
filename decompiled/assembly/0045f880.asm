
// block 0045f880
0045f880  sub        esp, 0x44
0045f883  test       byte ptr [0x4ceae8], 1
0045f88a  push       ebx
0045f88b  push       ebp
0045f88c  mov        ebp, dword ptr [esp + 0x50]
0045f890  mov        ebx, ecx
0045f892  mov        dword ptr [esp + 0xc], eax
0045f896  je         0x45f8bf

// block 0045f898
0045f898  mov        eax, dword ptr [eax*4 + 0x4a2338]
0045f89f  cmp        eax, 0x15
0045f8a2  je         0x45f8b7

// block 0045f8a4
0045f8a4  test       eax, eax
0045f8a6  je         0x45f8b7

// block 0045f8a8
0045f8a8  cmp        eax, 0x14
0045f8ab  jne        0x45f8bf

// block 0045f8ad
0045f8ad  mov        dword ptr [esp + 0xc], 3
0045f8b5  jmp        0x45f8bf

// block 0045f8b7
0045f8b7  mov        dword ptr [esp + 0xc], 5

// block 0045f8bf
0045f8bf  and        dword ptr [esi + 0x10], 0xfffffffe
0045f8c3  mov        ecx, dword ptr [esp + 0xc]
0045f8c7  mov        edx, dword ptr [ecx*4 + 0x4a2338]
0045f8ce  mov        ecx, dword ptr [esi + 4]
0045f8d1  lea        eax, [esp + 0x10]
0045f8d5  push       eax
0045f8d6  push       0
0045f8d8  mov        eax, dword ptr [esi + 8]
0045f8db  push       0
0045f8dd  push       0
0045f8df  push       -1
0045f8e1  push       1
0045f8e3  push       1
0045f8e5  push       edx
0045f8e6  mov        edx, dword ptr [0x4ce8f0]
0045f8ec  push       0
0045f8ee  push       0
0045f8f0  push       0
0045f8f2  push       0
0045f8f4  push       eax
0045f8f5  push       ecx
0045f8f6  push       edx
0045f8f7  call       0x46c710

// block 0045f8fc
0045f8fc  test       eax, eax
0045f8fe  je         0x45f90b

// block 0045f900
0045f900  or         eax, 0xffffffff
0045f903  pop        ebp
0045f904  pop        ebx
0045f905  add        esp, 0x44
0045f908  ret        8

// block 0045f90b
0045f90b  mov        eax, dword ptr [esp + 0x10]
0045f90f  mov        ecx, dword ptr [eax]
0045f911  lea        edx, [esp + 8]
0045f915  push       edx
0045f916  push       0
0045f918  push       eax
0045f919  mov        eax, dword ptr [ecx + 0x48]
0045f91c  call       eax

// block 0045f91e
0045f91e  mov        eax, dword ptr [esp + 8]
0045f922  mov        ecx, dword ptr [eax]
0045f924  lea        edx, [esp + 0x28]
0045f928  push       edx
0045f929  push       eax
0045f92a  mov        eax, dword ptr [ecx + 0x30]
0045f92d  call       eax

// block 0045f92f
0045f92f  cmp        dword ptr [esp + 0x40], edi
0045f933  jne        0x45f962

// block 0045f935
0045f935  cmp        dword ptr [esp + 0x44], ebp
0045f939  jne        0x45f962

// block 0045f93b
0045f93b  mov        eax, dword ptr [esp + 8]
0045f93f  mov        ecx, dword ptr [esp + 0x10]
0045f943  mov        dword ptr [esi], ecx
0045f945  test       eax, eax
0045f947  je         0x45fa4b

// block 0045f94d
0045f94d  mov        edx, dword ptr [eax]
0045f94f  push       eax
0045f950  mov        eax, dword ptr [edx + 8]
0045f953  call       eax

// block 0045f955
0045f955  mov        dword ptr [esp + 8], 0
0045f95d  jmp        0x45fa4b

// block 0045f962
0045f962  mov        ecx, dword ptr [esp + 0xc]
0045f966  mov        edx, dword ptr [ecx*4 + 0x4a2338]
0045f96d  mov        eax, dword ptr [0x4ce8f0]
0045f972  push       esi
0045f973  push       1
0045f975  push       edx
0045f976  push       0
0045f978  push       0
0045f97a  push       ebp
0045f97b  push       edi
0045f97c  push       eax
0045f97d  call       0x46c6e6

// block 0045f982
0045f982  mov        eax, dword ptr [esi]
0045f984  mov        ecx, dword ptr [eax]
0045f986  lea        edx, [esp + 0x14]
0045f98a  push       edx
0045f98b  push       0
0045f98d  push       eax
0045f98e  mov        eax, dword ptr [ecx + 0x48]
0045f991  call       eax

// block 0045f993
0045f993  mov        eax, dword ptr [esp + 0x40]
0045f997  mov        ecx, ebp
0045f999  lea        ebp, [edi + ebx]
0045f99c  cmp        ebp, eax
0045f99e  mov        edx, edi
0045f9a0  jle        0x45f9a6

// block 0045f9a2
0045f9a2  sub        eax, ebx
0045f9a4  mov        edx, eax

// block 0045f9a6
0045f9a6  mov        ebp, dword ptr [esp + 0x54]
0045f9aa  mov        eax, ecx
0045f9ac  add        eax, ebp
0045f9ae  mov        ebp, dword ptr [esp + 0x44]
0045f9b2  cmp        eax, ebp
0045f9b4  mov        eax, dword ptr [esp + 0x54]
0045f9b8  jle        0x45f9be

// block 0045f9ba
0045f9ba  sub        ebp, eax
0045f9bc  mov        ecx, ebp

// block 0045f9be
0045f9be  add        edx, ebx
0045f9c0  mov        dword ptr [esp + 0x18], ebx
0045f9c4  xor        ebx, ebx
0045f9c6  push       ebx
0045f9c7  add        ecx, eax
0045f9c9  push       1
0045f9cb  mov        dword ptr [esp + 0x2c], ecx
0045f9cf  lea        ecx, [esp + 0x20]
0045f9d3  push       ecx
0045f9d4  push       ebx
0045f9d5  mov        dword ptr [esp + 0x30], edx
0045f9d9  mov        edx, dword ptr [esp + 0x18]
0045f9dd  push       edx
0045f9de  push       ebx
0045f9df  mov        dword ptr [esp + 0x34], eax
0045f9e3  mov        eax, dword ptr [esp + 0x2c]
0045f9e7  push       ebx
0045f9e8  push       eax
0045f9e9  call       0x46c71c

// block 0045f9ee
0045f9ee  test       eax, eax
0045f9f0  je         0x45fa0b

// block 0045f9f2
0045f9f2  mov        ecx, dword ptr [esp + 0xc]
0045f9f6  mov        edx, dword ptr [ecx*4 + 0x4a2338]
0045f9fd  push       edx
0045f9fe  push       0x4a325c
0045fa03  call       0x4622e0

// block 0045fa08
0045fa08  add        esp, 8

// block 0045fa0b
0045fa0b  mov        eax, dword ptr [esp + 8]
0045fa0f  cmp        eax, ebx
0045fa11  je         0x45fa1f

// block 0045fa13
0045fa13  mov        ecx, dword ptr [eax]
0045fa15  mov        edx, dword ptr [ecx + 8]
0045fa18  push       eax
0045fa19  call       edx

// block 0045fa1b
0045fa1b  mov        dword ptr [esp + 8], ebx

// block 0045fa1f
0045fa1f  mov        eax, dword ptr [esp + 0x14]
0045fa23  cmp        eax, ebx
0045fa25  je         0x45fa33

// block 0045fa27
0045fa27  mov        ecx, dword ptr [eax]
0045fa29  mov        edx, dword ptr [ecx + 8]
0045fa2c  push       eax
0045fa2d  call       edx

// block 0045fa2f
0045fa2f  mov        dword ptr [esp + 0x14], ebx

// block 0045fa33
0045fa33  mov        eax, dword ptr [esp + 0x10]
0045fa37  cmp        eax, ebx
0045fa39  je         0x45fa47

// block 0045fa3b
0045fa3b  mov        ecx, dword ptr [eax]
0045fa3d  mov        edx, dword ptr [ecx + 8]
0045fa40  push       eax
0045fa41  call       edx

// block 0045fa43
0045fa43  mov        dword ptr [esp + 0x10], ebx

// block 0045fa47
0045fa47  mov        ebp, dword ptr [esp + 0x50]

// block 0045fa4b
0045fa4b  mov        eax, esi
0045fa4d  call       0x45ee00

// block 0045fa52
0045fa52  mov        eax, dword ptr [esp + 0xc]
0045fa56  mov        ecx, dword ptr [eax*4 + 0x4a235c]
0045fa5d  lea        eax, [eax*4 + 0x4a235c]
0045fa64  mov        dword ptr [esi + 0xc], ecx
0045fa67  mov        eax, dword ptr [eax]
0045fa69  imul       eax, edi
0045fa6c  imul       eax, ebp
0045fa6f  pop        ebp
0045fa70  pop        ebx
0045fa71  add        esp, 0x44
0045fa74  ret        8
