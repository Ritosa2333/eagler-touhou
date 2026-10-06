
// block 00458cf0
00458cf0  sub        esp, 0xc
00458cf3  mov        eax, dword ptr [edi + 0x24]
00458cf6  push       esi
00458cf7  mov        dword ptr [esp + 4], eax
00458cfb  test       eax, eax
00458cfd  jle        0x458d5f

// block 00458cff
00458cff  lea        esi, [edi + 0x10]
00458d02  call       0x464a80

// block 00458d07
00458d07  mov        ecx, dword ptr [edi + 0x24]
00458d0a  cmp        dword ptr [edi + 0x14], ecx
00458d0d  mov        dword ptr [esp + 4], ecx
00458d11  jl         0x458d5f

// block 00458d13
00458d13  mov        eax, dword ptr [esi + 0x10]
00458d16  mov        dword ptr [esp + 4], ecx
00458d1a  test       al, 1
00458d1c  jne        0x458d3d

// block 00458d1e
00458d1e  fldz       
00458d20  or         eax, 1
00458d23  fstp       dword ptr [esi + 8]
00458d26  mov        dword ptr [esi + 4], 0
00458d2d  mov        dword ptr [esi], 0xfff0bdc1
00458d33  mov        dword ptr [esi + 0xc], 0x4b2ed0
00458d3a  mov        dword ptr [esi + 0x10], eax

// block 00458d3d
00458d3d  fild       dword ptr [esp + 4]
00458d41  mov        dword ptr [esi + 4], ecx
00458d44  dec        ecx
00458d45  mov        dword ptr [esi], ecx
00458d47  fstp       dword ptr [esi + 8]
00458d4a  cmp        dword ptr [edi + 0x28], 7
00458d4e  mov        dword ptr [edi + 0x24], 0
00458d55  je         0x458d6c

// block 00458d57
00458d57  mov        eax, dword ptr [edi + 4]
00458d5a  pop        esi
00458d5b  add        esp, 0xc
00458d5e  ret        

// block 00458d5f
00458d5f  mov        eax, dword ptr [edi + 0x28]
00458d62  cmp        eax, 7
00458d65  jne        0x458d73

// block 00458d67
00458d67  mov        eax, dword ptr [edi + 4]
00458d6a  add        dword ptr [edi], eax

// block 00458d6c
00458d6c  mov        eax, dword ptr [edi]
00458d6e  pop        esi
00458d6f  add        esp, 0xc
00458d72  ret        

// block 00458d73
00458d73  cmp        eax, 0x11
00458d76  jne        0x458d8c

// block 00458d78
00458d78  mov        eax, dword ptr [edi + 0xc]
00458d7b  add        dword ptr [edi], eax
00458d7d  mov        ecx, dword ptr [edi + 4]
00458d80  add        ecx, eax
00458d82  mov        eax, dword ptr [edi]
00458d84  mov        dword ptr [edi + 0xc], ecx
00458d87  pop        esi
00458d88  add        esp, 0xc
00458d8b  ret        

// block 00458d8c
00458d8c  cmp        eax, 8
00458d8f  jne        0x458e15

// block 00458d95
00458d95  fld        dword ptr [edi + 0x18]
00458d98  pop        esi
00458d99  fidiv      dword ptr [esp]
00458d9c  fstp       dword ptr [esp]
00458d9f  fld        dword ptr [esp]
00458da2  fld        st(0)
00458da4  fld1       
00458da6  fsub       st(1), st(0)
00458da8  fld        st(1)
00458daa  fld        st(3)
00458dac  fadd       st(0), st(0)
00458dae  fld        st(4)
00458db0  fld        st(5)
00458db2  fsubr      st(4)
00458db4  fstp       qword ptr [esp + 4]
00458db8  fld        qword ptr [0x4a3fd8]
00458dbe  fsub       st(2)
00458dc0  fld        st(1)
00458dc2  fmulp      st(2)
00458dc4  fmulp      st(1)
00458dc6  fstp       dword ptr [esp]
00458dc9  fld        dword ptr [esp]
00458dcc  fimul      dword ptr [edi + 4]
00458dcf  fxch       st(1)
00458dd1  faddp      st(3)
00458dd3  fld        st(1)
00458dd5  fmulp      st(2)
00458dd7  fxch       st(2)
00458dd9  fmulp      st(1)
00458ddb  fstp       dword ptr [esp]
00458dde  fld        dword ptr [esp]
00458de1  fimul      dword ptr [edi]
00458de3  faddp      st(1)
00458de5  fld        qword ptr [esp + 4]
00458de9  fmul       st(0), st(0)
00458deb  fmul       st(3)
00458ded  fstp       dword ptr [esp]
00458df0  fld        dword ptr [esp]
00458df3  fimul      dword ptr [edi + 8]
00458df6  faddp      st(1)
00458df8  fld        st(2)
00458dfa  fmulp      st(2)
00458dfc  fxch       st(1)
00458dfe  fmulp      st(2)
00458e00  fxch       st(1)
00458e02  fstp       dword ptr [esp]
00458e05  fld        dword ptr [esp]
00458e08  fimul      dword ptr [edi + 0xc]
00458e0b  faddp      st(1)
00458e0d  add        esp, 0xc
00458e10  jmp        0x4931e0

// block 00458e15
00458e15  mov        esi, dword ptr [edi]
00458e17  mov        eax, edi
00458e19  mov        dword ptr [esp + 8], esi
00458e1d  call       0x459330

// block 00458e22
00458e22  mov        edx, dword ptr [edi + 4]
00458e25  sub        edx, esi
00458e27  mov        dword ptr [esp + 4], edx
00458e2b  fimul      dword ptr [esp + 4]
00458e2f  pop        esi
00458e30  fiadd      dword ptr [esp + 4]
00458e34  add        esp, 0xc
00458e37  jmp        0x4931e0
