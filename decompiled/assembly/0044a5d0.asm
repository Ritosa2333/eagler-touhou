
// block 0044a5d0
0044a5d0  push       ebp
0044a5d1  mov        ebp, esp
0044a5d3  and        esp, 0xffffffc0
0044a5d6  sub        esp, 0x38
0044a5d9  push       esi
0044a5da  mov        esi, dword ptr [0x4b43b8]
0044a5e0  push       edi
0044a5e1  xor        edi, edi
0044a5e3  cmp        dword ptr [ebx + 0x64], edi
0044a5e6  jle        0x44a6e3

// block 0044a5ec
0044a5ec  mov        eax, dword ptr [ebx + 0x54]
0044a5ef  fld        dword ptr [0x4a4014]
0044a5f5  mov        ecx, dword ptr [ebx + 0x58]
0044a5f8  mov        edx, dword ptr [ebx + 0x5c]
0044a5fb  fst        dword ptr [esi + 0x18f84]
0044a601  mov        dword ptr [esp + 0x34], eax
0044a605  fstp       dword ptr [esi + 0x18f88]
0044a60b  mov        dword ptr [esi + 0x18f98], 4
0044a615  mov        dword ptr [esi + 0x18f9c], 1
0044a61f  mov        eax, dword ptr [ebx + 0x64]
0044a622  mov        dword ptr [esp + 0x3c], edx
0044a626  cdq        
0044a627  mov        dword ptr [esp + 0x38], ecx
0044a62b  mov        ecx, 0xa
0044a630  idiv       ecx
0044a632  xor        eax, eax
0044a634  mov        dword ptr [esi + 0x18fa4], edi
0044a63a  mov        dword ptr [esi + 0x18fa8], edi
0044a640  lea        edi, [esp + 0x34]
0044a644  cmp        edx, 5
0044a647  setge      al
0044a64a  sub        esp, 8
0044a64d  dec        eax
0044a64e  and        eax, 0x7f7e80
0044a653  add        eax, 0xff808080
0044a658  mov        dword ptr [esi + 0x18f80], eax
0044a65e  fld        dword ptr [ebx + 0x60]
0044a661  fstp       qword ptr [esp]
0044a664  push       0x4a2154
0044a669  call       0x4020a0

// block 0044a66e
0044a66e  fld1       
0044a670  mov        esi, dword ptr [0x4b43b8]
0044a676  fst        dword ptr [esi + 0x18f84]
0044a67c  fst        dword ptr [esi + 0x18f88]
0044a682  or         edi, 0xffffffff
0044a685  fld        dword ptr [esp + 0x44]
0044a689  mov        dword ptr [esi + 0x18f80], edi
0044a68f  fadd       qword ptr [0x4a3d70]
0044a695  mov        ecx, dword ptr [ebx + 0x68]
0044a698  add        esp, 0xc
0044a69b  fstp       dword ptr [esp + 0x38]
0044a69f  test       ecx, ecx
0044a6a1  jle        0x44a6b6

// block 0044a6a3
0044a6a3  lea        edx, [esp + 0x34]
0044a6a7  fstp       st(0)
0044a6a9  call       0x4021a0

// block 0044a6ae
0044a6ae  fld1       
0044a6b0  mov        esi, dword ptr [0x4b43b8]

// block 0044a6b6
0044a6b6  mov        eax, 1
0044a6bb  fst        dword ptr [esi + 0x18f84]
0044a6c1  fstp       dword ptr [esi + 0x18f88]
0044a6c7  mov        dword ptr [esi + 0x18f80], edi
0044a6cd  mov        dword ptr [esi + 0x18f9c], 0
0044a6d7  mov        dword ptr [esi + 0x18fa4], eax
0044a6dd  mov        dword ptr [esi + 0x18fa8], eax

// block 0044a6e3
0044a6e3  mov        ecx, dword ptr [ebx + 0x38]
0044a6e6  test       ecx, ecx
0044a6e8  je         0x44a84e

// block 0044a6ee
0044a6ee  cmp        dword ptr [ebx + 0x14], 0x3c
0044a6f2  jl         0x44a84e

// block 0044a6f8
0044a6f8  mov        edx, dword ptr [0x4b43dc]
0044a6fe  mov        eax, dword ptr [edx + 0x68]
0044a701  test       eax, eax
0044a703  je         0x44a84e

// block 0044a709
0044a709  lea        esp, [esp]

// block 0044a710
0044a710  mov        edx, dword ptr [eax]
0044a712  cmp        dword ptr [edx + 0x27b4], ecx
0044a718  je         0x44a72c

// block 0044a71a
0044a71a  mov        eax, dword ptr [eax + 4]
0044a71d  test       eax, eax
0044a71f  jne        0x44a710

// block 0044a721
0044a721  mov        eax, 1
0044a726  pop        edi
0044a727  pop        esi
0044a728  mov        esp, ebp
0044a72a  pop        ebp
0044a72b  ret        

// block 0044a72c
0044a72c  mov        dword ptr [esi + 0x18f98], 2
0044a736  mov        dword ptr [esi + 0x18f9c], 1
0044a740  mov        ecx, dword ptr [ebx + 0x38]
0044a743  call       0x412530

// block 0044a748
0044a748  fld        dword ptr [eax + 0x1074]
0044a74e  fadd       qword ptr [0x4a3d70]
0044a754  mov        dword ptr [esp + 0x30], eax
0044a758  fadd       qword ptr [0x4a3d28]
0044a75e  fstp       dword ptr [esp + 0x34]
0044a762  fld        dword ptr [eax + 0x1078]
0044a768  fadd       qword ptr [0x4a3d68]
0044a76e  fstp       dword ptr [esp + 0x38]
0044a772  fld        dword ptr [eax + 0x107c]
0044a778  mov        dword ptr [esi + 0x18f80], 0xc080ffc0
0044a782  mov        eax, dword ptr [ebx + 0x28]
0044a785  fstp       dword ptr [esp + 0x3c]
0044a789  fld        dword ptr [esp + 0x38]
0044a78d  sub        eax, dword ptr [ebx + 0x14]
0044a790  fadd       qword ptr [0x4a4170]
0044a796  mov        dword ptr [esp + 0x2c], eax
0044a79a  fstp       dword ptr [esp + 0x38]
0044a79e  fld        dword ptr [esp + 0x34]
0044a7a2  fsub       qword ptr [0x4a4128]
0044a7a8  fstp       dword ptr [esp + 0x34]
0044a7ac  fild       dword ptr [esp + 0x2c]
0044a7b0  fdiv       qword ptr [0x4a3d20]
0044a7b6  fstp       dword ptr [esp + 0x2c]
0044a7ba  fld        dword ptr [esp + 0x2c]
0044a7be  fld        qword ptr [0x4a3ce8]
0044a7c4  fmul       st(1)
0044a7c6  call       0x4931e0

// block 0044a7cb
0044a7cb  cdq        
0044a7cc  mov        ecx, 0x64
0044a7d1  idiv       ecx
0044a7d3  push       edx
0044a7d4  call       0x4931e0

// block 0044a7d9
0044a7d9  push       eax
0044a7da  push       0x4a215c
0044a7df  lea        edi, [esp + 0x40]
0044a7e3  call       0x4020a0

// block 0044a7e8
0044a7e8  mov        eax, dword ptr [0x4b43b8]
0044a7ed  mov        dword ptr [eax + 0x18f9c], 0
0044a7f7  mov        dword ptr [eax + 0x18f80], 0xffffffff
0044a801  mov        eax, dword ptr [esp + 0x3c]
0044a805  fild       dword ptr [eax + 0x2648]
0044a80b  mov        edx, dword ptr [ebx + 0x44]
0044a80e  lea        edi, [ebx + 0x44]
0044a811  add        esp, 0xc
0044a814  fidiv      dword ptr [eax + 0x264c]
0044a81a  push       edx
0044a81b  mov        edx, dword ptr [0x4ce8cc]
0044a821  fstp       dword ptr [esp + 0x34]
0044a825  call       0x461920

// block 0044a82a
0044a82a  test       eax, eax
0044a82c  jne        0x44a830

// block 0044a82e
0044a82e  mov        dword ptr [edi], eax

// block 0044a830
0044a830  fld        dword ptr [esp + 0x30]
0044a834  or         dword ptr [eax + 0x47c], 8
0044a83b  fstp       dword ptr [eax + 0x40]
0044a83e  mov        esi, dword ptr [ebx + 0x34]
0044a841  add        esi, 0x1074
0044a847  mov        eax, edi
0044a849  call       0x461e30

// block 0044a84e
0044a84e  pop        edi
0044a84f  mov        eax, 1
0044a854  pop        esi
0044a855  mov        esp, ebp
0044a857  pop        ebp
0044a858  ret        
