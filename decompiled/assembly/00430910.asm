
// block 00430910
00430910  sub        esp, 0x38
00430913  push       ebx
00430914  push       esi
00430915  mov        esi, dword ptr [0x4ce8cc]
0043091b  test       esi, esi
0043091d  je         0x430924

// block 0043091f
0043091f  call       0x45a3c0

// block 00430924
00430924  mov        eax, dword ptr [edi + 0xd4]
0043092a  fild       dword ptr [edi + 0xd4]
00430930  test       eax, eax
00430932  jge        0x43093a

// block 00430934
00430934  fadd       dword ptr [0x4a3e10]

// block 0043093a
0043093a  mov        ecx, dword ptr [edi + 0xd8]
00430940  fstp       dword ptr [esp + 0xc]
00430944  fld        dword ptr [esp + 0xc]
00430948  fld        st(0)
0043094a  fld        qword ptr [0x4a3cf8]
00430950  fmul       st(1), st(0)
00430952  fxch       st(1)
00430954  fstp       dword ptr [esp + 8]
00430958  fild       dword ptr [edi + 0xd8]
0043095e  test       ecx, ecx
00430960  jge        0x430968

// block 00430962
00430962  fadd       dword ptr [0x4a3e10]

// block 00430968
00430968  fstp       dword ptr [esp + 0xc]
0043096c  push       ecx
0043096d  fld        dword ptr [esp + 0x10]
00430971  fld        st(0)
00430973  fmulp      st(2)
00430975  fxch       st(1)
00430977  fstp       dword ptr [esp + 0x10]
0043097b  fdivp      st(1)
0043097d  fstp       dword ptr [esp + 0x14]
00430981  fld        dword ptr [esp + 0xc]
00430985  fstp       dword ptr [esp + 0x38]
00430989  fld        dword ptr [esp + 0x10]
0043098d  fst        dword ptr [esp + 0x3c]
00430991  fstp       qword ptr [esp + 0x18]
00430995  fld        dword ptr [0x4a3e24]
0043099b  fstp       dword ptr [esp]
0043099e  call       0x4317d0

// block 004309a3
004309a3  fdivr      qword ptr [esp + 0x14]
004309a7  lea        edx, [esp + 0x1c]
004309ab  push       edx
004309ac  lea        eax, [esp + 0x2c]
004309b0  push       eax
004309b1  lea        ecx, [esp + 0x3c]
004309b5  push       ecx
004309b6  lea        esi, [edi + 0x4c]
004309b9  push       esi
004309ba  fstp       dword ptr [esp + 0x4c]
004309be  fld        dword ptr [esp + 0x18]
004309c2  fstp       dword ptr [esp + 0x38]
004309c6  fld        dword ptr [esp + 0x1c]
004309ca  fstp       dword ptr [esp + 0x3c]
004309ce  fldz       
004309d0  fst        dword ptr [esp + 0x40]
004309d4  fst        dword ptr [esp + 0x2c]
004309d8  fld        dword ptr [0x4a3da8]
004309de  fstp       dword ptr [esp + 0x30]
004309e2  fstp       dword ptr [esp + 0x34]
004309e6  call       0x46c6da

// block 004309eb
004309eb  fld        dword ptr [0x4a3e20]
004309f1  sub        esp, 0x10
004309f4  fstp       dword ptr [esp + 0xc]
004309f8  lea        ebx, [edi + 0x8c]
004309fe  fld1       
00430a00  fstp       dword ptr [esp + 8]
00430a04  fld        dword ptr [esp + 0x20]
00430a08  fstp       dword ptr [esp + 4]
00430a0c  fld        dword ptr [0x4a3e1c]
00430a12  fstp       dword ptr [esp]
00430a15  push       ebx
00430a16  call       0x46c6e0

// block 00430a1b
00430a1b  mov        eax, dword ptr [0x4ce8f0]
00430a20  mov        edx, dword ptr [eax]
00430a22  push       esi
00430a23  push       2
00430a25  push       eax
00430a26  mov        eax, dword ptr [edx + 0xb0]
00430a2c  call       eax

// block 00430a2e
00430a2e  mov        eax, dword ptr [0x4ce8f0]
00430a33  mov        ecx, dword ptr [eax]
00430a35  mov        edx, dword ptr [ecx + 0xb0]
00430a3b  push       ebx
00430a3c  push       3
00430a3e  push       eax
00430a3f  call       edx

// block 00430a41
00430a41  mov        eax, dword ptr [0x4ce8cc]
00430a46  pop        esi
00430a47  pop        ebx
00430a48  test       eax, eax
00430a4a  je         0x430a64

// block 00430a4c
00430a4c  fld        dword ptr [edi + 0xe8]
00430a52  fstp       dword ptr [eax + 0xb0]
00430a58  fld        dword ptr [edi + 0xec]
00430a5e  fstp       dword ptr [eax + 0xb4]

// block 00430a64
00430a64  add        esp, 0x38
00430a67  ret        
