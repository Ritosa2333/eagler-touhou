
// block 00436ba0
00436ba0  push       ebp
00436ba1  mov        ebp, esp
00436ba3  and        esp, 0xfffffff8
00436ba6  sub        esp, 0x44
00436ba9  push       ebx
00436baa  push       esi
00436bab  push       edi
00436bac  mov        edi, dword ptr [ebp + 8]
00436baf  mov        eax, dword ptr [edi + 0xa28]
00436bb5  cmp        eax, 4
00436bb8  ja         0x436f90

// block 00436bbe
00436bbe  jmp        dword ptr [eax*4 + 0x4375e8]

// block 00436bc5
00436bc5  mov        ecx, dword ptr [edi + 0xa34]
00436bcb  imul       ecx, ecx, 0x2800
00436bd1  mov        eax, 0x88888889
00436bd6  imul       ecx
00436bd8  add        edx, ecx
00436bda  sar        edx, 5
00436bdd  mov        ecx, edx
00436bdf  shr        ecx, 0x1f
00436be2  add        ecx, edx
00436be4  mov        eax, 0xf000
00436be9  sub        eax, ecx
00436beb  mov        dword ptr [esp + 0x14], eax
00436bef  mov        dword ptr [edi + 0x98c], eax
00436bf5  mov        eax, edi
00436bf7  fild       dword ptr [esp + 0x14]
00436bfb  fmul       qword ptr [0x4a3d40]
00436c01  fstp       dword ptr [edi + 0x980]
00436c07  call       0x412610

// block 00436c0c
00436c0c  mov        eax, dword ptr [edi + 0xa34]
00436c12  xor        ecx, ecx
00436c14  cmp        eax, 0x1e
00436c17  mov        dword ptr [edi + 0xa20], ecx
00436c1d  mov        dword ptr [edi + 0xa24], ecx
00436c23  mov        dword ptr [esp + 0x14], eax
00436c27  jl         0x436c3f

// block 00436c29
00436c29  lea        ebx, [ecx + 1]
00436c2c  call       0x40d230

// block 00436c31
00436c31  mov        edi, ebx
00436c33  xor        ebx, ebx
00436c35  call       0x428750

// block 00436c3a
00436c3a  mov        edi, dword ptr [ebp + 8]
00436c3d  jmp        0x436cb2

// block 00436c3f
00436c3f  fild       dword ptr [esp + 0x14]
00436c43  push       1
00436c45  push       ecx
00436c46  lea        esi, [edi + 0xa08]
00436c4c  fmul       qword ptr [0x4a43d0]
00436c52  push       ecx
00436c53  mov        ebx, esi
00436c55  fdiv       qword ptr [0x4a3e78]
00436c5b  fadd       qword ptr [0x4a3e88]
00436c61  fstp       dword ptr [esp + 0x1c]
00436c65  fld        dword ptr [esp + 0x1c]
00436c69  fstp       dword ptr [esp]
00436c6c  call       0x40caa0

// block 00436c71
00436c71  fld        dword ptr [esp + 0x10]
00436c75  push       0
00436c77  fmul       qword ptr [0x4a42b8]
00436c7d  push       0
00436c7f  push       ecx
00436c80  fstp       dword ptr [esp + 0x20]
00436c84  fld        dword ptr [esp + 0x20]
00436c88  fstp       dword ptr [esp]
00436c8b  call       0x40caa0

// block 00436c90
00436c90  fld        dword ptr [esp + 0x10]
00436c94  push       1
00436c96  push       0
00436c98  push       ecx
00436c99  fstp       dword ptr [esp]
00436c9c  call       0x4286f0

// block 00436ca1
00436ca1  fld        dword ptr [esp + 0x14]
00436ca5  push       0
00436ca7  push       0
00436ca9  push       ecx
00436caa  fstp       dword ptr [esp]
00436cad  call       0x4286f0

// block 00436cb2
00436cb2  cmp        dword ptr [edi + 0xa34], 0x3c
00436cb9  jl         0x436f90

// block 00436cbf
00436cbf  push       0
00436cc1  lea        eax, [edi + 0xa30]
00436cc7  mov        dword ptr [edi + 0xa28], 1
00436cd1  call       0x4067e0

// block 00436cd6
00436cd6  mov        eax, dword ptr [0x4b43e4]
00436cdb  test       eax, eax
00436cdd  je         0x436d28

// block 00436cdf
00436cdf  cmp        dword ptr [eax + 0x6d30], 0
00436ce6  jne        0x436d28

// block 00436ce8
00436ce8  mov        eax, dword ptr [0x4b43dc]
00436ced  test       eax, eax
00436cef  je         0x436d28

// block 00436cf1
00436cf1  cmp        dword ptr [eax + 0x70], 0
00436cf5  je         0x436d28

// block 00436cf7
00436cf7  mov        eax, dword ptr [0x4b43c4]
00436cfc  test       eax, eax
00436cfe  je         0x436d28

// block 00436d00
00436d00  cmp        dword ptr [eax + 0x3c], 0
00436d04  jne        0x436d28

// block 00436d06
00436d06  cmp        dword ptr [0x4b0ca0], 0
00436d0d  je         0x436d28

// block 00436d0f
00436d0f  test       byte ptr [0x4d49d0], 2
00436d16  je         0x436d28

// block 00436d18
00436d18  call       0x406bf0

// block 00436d1d
00436d1d  call       0x422f20

// block 00436d22
00436d22  push       edi
00436d23  call       0x4385b0

// block 00436d28
00436d28  cmp        dword ptr [edi + 0xa34], 0x1e
00436d2f  jge        0x436d47

// block 00436d31
00436d31  mov        ebx, 1
00436d36  call       0x40d230

// block 00436d3b
00436d3b  mov        edi, ebx
00436d3d  xor        ebx, ebx
00436d3f  call       0x428750

// block 00436d44
00436d44  mov        edi, dword ptr [ebp + 8]

// block 00436d47
00436d47  call       0x4364f0

// block 00436d4c
00436d4c  jmp        0x436f8d

// block 00436d51
00436d51  cmp        dword ptr [edi + 0xa34], 8
00436d58  jl         0x436d77

// block 00436d5a
00436d5a  mov        esi, edi
00436d5c  call       0x4381e0

// block 00436d61
00436d61  mov        eax, dword ptr [0x4b43dc]
00436d66  mov        ebx, 1
00436d6b  add        dword ptr [eax + 0x10], ebx
00436d6e  mov        dword ptr [eax + 0x18], 0
00436d75  jmp        0x436dd9

// block 00436d77
00436d77  mov        eax, dword ptr [0x4b43c4]
00436d7c  test       eax, eax
00436d7e  je         0x436f90

// block 00436d84
00436d84  cmp        dword ptr [eax + 0x3c], 0
00436d88  jne        0x436f90

// block 00436d8e
00436d8e  cmp        dword ptr [0x4b0ca0], 0
00436d95  je         0x436f90

// block 00436d9b
00436d9b  test       byte ptr [0x4d49d0], 2
00436da2  je         0x436f90

// block 00436da8
00436da8  push       0x3c
00436daa  lea        eax, [edi + 0xa30]
00436db0  call       0x4067e0

// block 00436db5
00436db5  call       0x406bf0

// block 00436dba
00436dba  call       0x422f20

// block 00436dbf
00436dbf  push       edi
00436dc0  call       0x4385b0

// block 00436dc5
00436dc5  mov        dword ptr [edi + 0xa28], 1
00436dcf  jmp        0x436f90

// block 00436dd4
00436dd4  mov        ebx, 1

// block 00436dd9
00436dd9  cmp        dword ptr [edi + 0xa34], 3
00436de0  jne        0x436e91

// block 00436de6
00436de6  mov        edx, dword ptr [0x4b0cd4]
00436dec  push       edx
00436ded  call       0x439440

// block 00436df2
00436df2  fldz       
00436df4  fst        dword ptr [esp + 0x28]
00436df8  lea        eax, [esp + 0x28]
00436dfc  fld        dword ptr [edi + 0x980]
00436e02  mov        ecx, edi
00436e04  fsub       qword ptr [0x4a40b0]
00436e0a  fstp       dword ptr [esp + 0x2c]
00436e0e  fstp       dword ptr [esp + 0x30]
00436e12  call       0x437730

// block 00436e17
00436e17  xor        esi, esi
00436e19  fstp       dword ptr [esp + 0x10]
00436e1d  mov        dword ptr [esp + 0x34], ebx
00436e21  mov        dword ptr [esp + 0x38], ebx
00436e25  mov        dword ptr [esp + 0x3c], ebx
00436e29  mov        dword ptr [esp + 0x40], ebx
00436e2d  mov        dword ptr [esp + 0x44], ebx
00436e31  mov        dword ptr [esp + 0x48], ebx
00436e35  mov        dword ptr [esp + 0x4c], ebx
00436e39  mov        dword ptr [esp + 0x14], esi

// block 00436e3d
00436e3d  fld        dword ptr [0x4a4020]
00436e43  sub        esp, 8
00436e46  fstp       dword ptr [esp + 4]
00436e4a  lea        eax, [edi + 0x97c]
00436e50  fild       dword ptr [esp + 0x1c]
00436e54  fmul       qword ptr [0x4a3cd8]
00436e5a  fdiv       qword ptr [0x4a4140]
00436e60  fadd       dword ptr [esp + 0x18]
00436e64  fsub       qword ptr [0x4a4290]
00436e6a  fstp       dword ptr [esp + 0x1c]
00436e6e  fld        dword ptr [esp + 0x1c]
00436e72  fstp       dword ptr [esp]
00436e75  push       eax
00436e76  mov        eax, dword ptr [esp + esi*4 + 0x40]
00436e7a  push       eax
00436e7b  call       0x4273f0

// block 00436e80
00436e80  add        esi, ebx
00436e82  cmp        esi, 7
00436e85  mov        dword ptr [esp + 0x14], esi
00436e89  jl         0x436e3d

// block 00436e8b
00436e8b  push       edi
00436e8c  call       0x4385b0

// block 00436e91
00436e91  cmp        dword ptr [edi + 0xa34], 0x1e
00436e98  jl         0x436f90

// block 00436e9e
00436e9e  xor        ebx, ebx
00436ea0  cmp        dword ptr [0x4b0c98], ebx
00436ea6  jge        0x436ec8

// block 00436ea8
00436ea8  mov        ecx, dword ptr [0x4b4518]
00436eae  cmp        dword ptr [ecx + 0x10], 1
00436eb2  je         0x436ebe

// block 00436eb4
00436eb4  call       0x433710

// block 00436eb9
00436eb9  jmp        0x436f90

// block 00436ebe
00436ebe  call       0x432850

// block 00436ec3
00436ec3  jmp        0x436f90

// block 00436ec8
00436ec8  fld1       
00436eca  push       0x96
00436ecf  push       0x1e
00436ed1  mov        dword ptr [edi + 0xa28], ebx
00436ed7  fstp       dword ptr [0x4b2ed0]
00436edd  fld        dword ptr [0x4a3e68]
00436ee3  sub        esp, 8
00436ee6  fstp       dword ptr [esp + 4]
00436eea  lea        esi, [edi + 0x97c]
00436ef0  fld        dword ptr [0x4a3d78]
00436ef6  mov        eax, edi
00436ef8  fstp       dword ptr [esp]
00436efb  push       esi
00436efc  call       0x4390f0

// block 00436f01
00436f01  cmp        dword ptr [0x4b0ca0], 2
00436f08  jge        0x436f14

// block 00436f0a
00436f0a  mov        eax, 0x4b0c40
00436f0f  call       0x422f60

// block 00436f14
00436f14  mov        edx, dword ptr [esi]
00436f16  fldz       
00436f18  mov        eax, dword ptr [esi + 4]
00436f1b  mov        ecx, dword ptr [esi + 8]
00436f1e  fstp       dword ptr [esi]
00436f20  fld        dword ptr [0x4a3ec0]
00436f26  mov        dword ptr [edi + 0xa08], edx
00436f2c  mov        dword ptr [edi + 0xa0c], eax
00436f32  fstp       dword ptr [edi + 0x980]
00436f38  push       0x118
00436f3d  lea        eax, [edi + 0xc400]
00436f43  mov        dword ptr [edi + 0xa10], ecx
00436f49  mov        dword ptr [edi + 0x988], ebx
00436f4f  mov        dword ptr [edi + 0x98c], 0xf000
00436f59  call       0x4067e0

// block 00436f5e
00436f5e  push       ebx
00436f5f  lea        eax, [edi + 0xa30]
00436f65  call       0x4067e0

// block 00436f6a
00436f6a  jmp        0x436f90

// block 00436f6c
00436f6c  mov        eax, dword ptr [edi + 0xa34]
00436f72  cmp        eax, 4
00436f75  je         0x436f90

// block 00436f77
00436f77  cmp        eax, 0xf
00436f7a  jne        0x436f90

// block 00436f7c
00436f7c  xor        ebx, ebx
00436f7e  call       0x40d230

// block 00436f83
00436f83  xor        edi, edi
00436f85  lea        ebx, [edi + 1]
00436f88  call       0x428750

// block 00436f8d
00436f8d  mov        edi, dword ptr [ebp + 8]

// block 00436f90
00436f90  fld1       
00436f92  lea        esi, [edi + 0x8988]
00436f98  fstp       dword ptr [edi + 0x825c]
00436f9e  mov        dword ptr [esp + 0x10], esi
00436fa2  fld1       
00436fa4  add        esi, 0x24
00436fa7  mov        dword ptr [esp + 0x14], 0x80
00436faf  mov        eax, 1

// block 00436fb4
00436fb4  test       byte ptr [esi + 0x4c], al
00436fb7  je         0x437087

// block 00436fbd
00436fbd  fstp       st(0)
00436fbf  test       byte ptr [esi + 0x24], al
00436fc2  jne        0x436fe2

// block 00436fc4
00436fc4  fld        dword ptr [esi + 0xc]
00436fc7  sub        esp, 8
00436fca  fstp       dword ptr [esp + 4]
00436fce  mov        ecx, esi
00436fd0  fld        dword ptr [esi + 0x10]
00436fd3  fstp       dword ptr [esp]
00436fd6  call       0x465390

// block 00436fdb
00436fdb  fldz       
00436fdd  fstp       dword ptr [esi + 8]
00436fe0  jmp        0x43700e

// block 00436fe2
00436fe2  fld        dword ptr [esi + 0x18]
00436fe5  push       ecx
00436fe6  fadd       dword ptr [esi + 0x14]
00436fe9  fstp       dword ptr [esi + 0x14]
00436fec  fld        dword ptr [esi + 0xc]
00436fef  fadd       dword ptr [esi + 0x10]
00436ff2  fstp       dword ptr [esp + 0x1c]
00436ff6  fld        dword ptr [esp + 0x1c]
00436ffa  fstp       dword ptr [esp]
00436ffd  call       0x4646e0

// block 00437002
00437002  push       ecx
00437003  fstp       dword ptr [esp]
00437006  call       0x4646e0

// block 0043700b
0043700b  fstp       dword ptr [esi + 0x10]

// block 0043700e
0043700e  lea        ebx, [esi - 0xc]
00437011  call       0x464db0

// block 00437016
00437016  fld        dword ptr [esi - 0x20]
00437019  mov        eax, dword ptr [esp + 0x10]
0043701d  fadd       dword ptr [eax]
0043701f  fstp       dword ptr [eax]
00437021  fld        dword ptr [esi - 0x18]
00437024  fadd       dword ptr [esi - 0x1c]
00437027  fstp       dword ptr [esi - 0x1c]
0043702a  mov        edx, dword ptr [esi + 0x2c]
0043702d  mov        ecx, dword ptr [esi + 0x34]
00437030  mov        dword ptr [esi + 0x28], edx
00437033  fld        dword ptr [ecx]
00437035  fcomp      qword ptr [0x4a3db0]
0043703b  fnstsw     ax
0043703d  test       ah, 0x41
00437040  jne        0x43705a

// block 00437042
00437042  fld        dword ptr [0x4a3dac]
00437048  fcomp      dword ptr [ecx]
0043704a  fnstsw     ax
0043704c  test       ah, 0x41
0043704f  jne        0x43705a

// block 00437051
00437051  fld        dword ptr [esi + 0x30]
00437054  fld1       
00437056  fsub       st(1), st(0)
00437058  jmp        0x43706f

// block 0043705a
0043705a  fld        dword ptr [esi + 0x30]
0043705d  fld        dword ptr [ecx]
0043705f  fld1       
00437061  fmul       st(1), st(0)
00437063  fxch       st(1)
00437065  fstp       dword ptr [esp + 0x18]
00437069  fld        dword ptr [esp + 0x18]
0043706d  fsubp      st(2)

// block 0043706f
0043706f  fxch       st(1)
00437071  fstp       dword ptr [esi + 0x30]
00437074  fld        dword ptr [esi + 0x30]
00437077  call       0x4931e0

// block 0043707c
0043707c  mov        dword ptr [esi + 0x2c], eax
0043707f  test       eax, eax
00437081  jg         0x437087

// block 00437083
00437083  and        dword ptr [esi + 0x4c], 0xfffffffe

// block 00437087
00437087  add        dword ptr [esp + 0x10], 0x74
0043708c  mov        eax, 1
00437091  add        esi, 0x74
00437094  sub        dword ptr [esp + 0x14], eax
00437098  jne        0x436fb4

// block 0043709e
0043709e  cmp        dword ptr [edi + 0xc404], 0
004370a5  jle        0x43713e

// block 004370ab
004370ab  mov        eax, dword ptr [edi + 0xc404]
004370b1  mov        ecx, dword ptr [edi + 0xc40c]
004370b7  mov        dword ptr [edi + 0xc400], eax
004370bd  fld        dword ptr [ecx]
004370bf  fcomp      qword ptr [0x4a3db0]
004370c5  fnstsw     ax
004370c7  test       ah, 0x41
004370ca  jne        0x4370e3

// block 004370cc
004370cc  fld        dword ptr [0x4a3dac]
004370d2  fcomp      dword ptr [ecx]
004370d4  fnstsw     ax
004370d6  test       ah, 0x41
004370d9  jne        0x4370e3

// block 004370db
004370db  fsubr      dword ptr [edi + 0xc408]
004370e1  jmp        0x4370f7

// block 004370e3
004370e3  fld        dword ptr [edi + 0xc408]
004370e9  fld        dword ptr [ecx]
004370eb  fmulp      st(2)
004370ed  fxch       st(1)
004370ef  fstp       dword ptr [esp + 0x18]
004370f3  fsub       dword ptr [esp + 0x18]

// block 004370f7
004370f7  fstp       dword ptr [edi + 0xc408]
004370fd  fld        dword ptr [edi + 0xc408]
00437103  call       0x4931e0

// block 00437108
00437108  mov        dword ptr [edi + 0xc404], eax
0043710e  mov        eax, dword ptr [edi + 0xa34]
00437114  cmp        eax, dword ptr [edi + 0xa30]
0043711a  je         0x437140

// block 0043711c
0043711c  cdq        
0043711d  mov        ecx, 3
00437122  idiv       ecx
00437124  test       edx, edx
00437126  jne        0x437140

// block 00437128
00437128  or         dword ptr [edi + 0x490], 0x10000
00437132  mov        dword ptr [edi + 0x3d4], 0xff0000ff
0043713c  jmp        0x43714a

// block 0043713e
0043713e  fstp       st(0)

// block 00437140
00437140  and        dword ptr [edi + 0x490], 0xfffeffff

// block 0043714a
0043714a  lea        edx, [edi + 0x14]
0043714d  push       edx
0043714e  call       0x455630

// block 00437153
00437153  fld        dword ptr [edi + 0x97c]
00437159  fsub       dword ptr [edi + 0x9e4]
0043715f  fstp       dword ptr [esp + 0x1c]
00437163  mov        eax, dword ptr [esp + 0x1c]
00437167  fld        dword ptr [edi + 0x980]
0043716d  fsub       dword ptr [edi + 0x9e8]
00437173  fstp       dword ptr [esp + 0x20]
00437177  mov        ecx, dword ptr [esp + 0x20]
0043717b  fld        dword ptr [edi + 0x984]
00437181  fsub       dword ptr [edi + 0x9ec]
00437187  mov        dword ptr [edi + 0x9cc], eax
0043718d  mov        dword ptr [edi + 0x9d0], ecx
00437193  fstp       dword ptr [esp + 0x24]
00437197  mov        edx, dword ptr [esp + 0x24]
0043719b  mov        dword ptr [edi + 0x9d4], edx
004371a1  fld        dword ptr [edi + 0x97c]
004371a7  fadd       dword ptr [edi + 0x9e4]
004371ad  fstp       dword ptr [esp + 0x1c]
004371b1  mov        eax, dword ptr [esp + 0x1c]
004371b5  fld        dword ptr [edi + 0x980]
004371bb  fadd       dword ptr [edi + 0x9e8]
004371c1  fstp       dword ptr [esp + 0x20]
004371c5  mov        ecx, dword ptr [esp + 0x20]
004371c9  fld        dword ptr [edi + 0x984]
004371cf  fadd       dword ptr [edi + 0x9ec]
004371d5  mov        dword ptr [edi + 0x9d8], eax
004371db  mov        dword ptr [edi + 0x9dc], ecx
004371e1  fstp       dword ptr [esp + 0x24]
004371e5  mov        edx, dword ptr [esp + 0x24]
004371e9  mov        dword ptr [edi + 0x9e0], edx
004371ef  fld        dword ptr [edi + 0x9f0]
004371f5  fld        qword ptr [0x4a3cf8]
004371fb  fmul       st(1), st(0)
004371fd  fxch       st(1)
004371ff  fstp       dword ptr [esp + 0x1c]
00437203  fld        dword ptr [edi + 0x9f4]
00437209  fmul       st(1)
0043720b  fstp       dword ptr [esp + 0x20]
0043720f  fld        dword ptr [edi + 0x9f8]
00437215  fmul       st(1)
00437217  fstp       dword ptr [esp + 0x24]
0043721b  fld        dword ptr [edi + 0x97c]
00437221  fsub       dword ptr [esp + 0x1c]
00437225  fstp       dword ptr [esp + 0x28]
00437229  mov        eax, dword ptr [esp + 0x28]
0043722d  fld        dword ptr [edi + 0x980]
00437233  fsub       dword ptr [esp + 0x20]
00437237  fstp       dword ptr [esp + 0x2c]
0043723b  mov        ecx, dword ptr [esp + 0x2c]
0043723f  fld        dword ptr [edi + 0x984]
00437245  mov        dword ptr [edi + 0xc444], eax
0043724b  fsub       dword ptr [esp + 0x24]
0043724f  mov        dword ptr [edi + 0xc448], ecx
00437255  fstp       dword ptr [esp + 0x30]
00437259  mov        edx, dword ptr [esp + 0x30]
0043725d  mov        dword ptr [edi + 0xc44c], edx
00437263  fld        dword ptr [edi + 0x9f0]
00437269  fmul       st(1)
0043726b  fstp       dword ptr [esp + 0x28]
0043726f  fld        dword ptr [edi + 0x9f4]
00437275  fmul       st(1)
00437277  fstp       dword ptr [esp + 0x2c]
0043727b  fmul       dword ptr [edi + 0x9f8]
00437281  fstp       dword ptr [esp + 0x30]
00437285  fld        dword ptr [esp + 0x28]
00437289  fadd       dword ptr [edi + 0x97c]
0043728f  fstp       dword ptr [esp + 0x1c]
00437293  mov        eax, dword ptr [esp + 0x1c]
00437297  fld        dword ptr [edi + 0x980]
0043729d  fadd       dword ptr [esp + 0x2c]
004372a1  fstp       dword ptr [esp + 0x20]
004372a5  mov        ecx, dword ptr [esp + 0x20]
004372a9  fld        dword ptr [edi + 0x984]
004372af  mov        dword ptr [edi + 0xc450], eax
004372b5  fadd       dword ptr [esp + 0x30]
004372b9  mov        dword ptr [edi + 0xc454], ecx
004372bf  fstp       dword ptr [esp + 0x24]
004372c3  mov        edx, dword ptr [esp + 0x24]
004372c7  mov        dword ptr [edi + 0xc458], edx
004372cd  fld        dword ptr [edi + 0x97c]
004372d3  fsub       dword ptr [edi + 0x9fc]
004372d9  fstp       dword ptr [esp + 0x28]
004372dd  mov        eax, dword ptr [esp + 0x28]

// block 004372e1
004372e1  fld        dword ptr [edi + 0x980]
004372e7  fsub       dword ptr [edi + 0xa00]
004372ed  fstp       dword ptr [esp + 0x2c]
004372f1  mov        ecx, dword ptr [esp + 0x2c]
004372f5  fld        dword ptr [edi + 0x984]
004372fb  fsub       dword ptr [edi + 0xa04]
00437301  mov        dword ptr [edi + 0xc45c], eax
00437307  mov        dword ptr [edi + 0xc460], ecx
0043730d  fstp       dword ptr [esp + 0x30]
00437311  mov        edx, dword ptr [esp + 0x30]
00437315  mov        dword ptr [edi + 0xc464], edx
0043731b  fld        dword ptr [edi + 0x97c]
00437321  fadd       dword ptr [edi + 0x9fc]
00437327  fstp       dword ptr [esp + 0x28]
0043732b  mov        eax, dword ptr [esp + 0x28]
0043732f  fld        dword ptr [edi + 0xa00]
00437335  fadd       dword ptr [edi + 0x980]
0043733b  fstp       dword ptr [esp + 0x2c]
0043733f  mov        ecx, dword ptr [esp + 0x2c]
00437343  fld        dword ptr [edi + 0xa04]
00437349  fadd       dword ptr [edi + 0x984]
0043734f  mov        dword ptr [edi + 0xc468], eax
00437355  mov        dword ptr [edi + 0xc46c], ecx
0043735b  fstp       dword ptr [esp + 0x30]
0043735f  mov        edx, dword ptr [esp + 0x30]
00437363  mov        dword ptr [edi + 0xc470], edx
00437369  fld        dword ptr [edi + 0x97c]
0043736f  fsub       dword ptr [edi + 0x9f0]
00437375  fstp       dword ptr [esp + 0x28]
00437379  mov        eax, dword ptr [esp + 0x28]
0043737d  fld        dword ptr [edi + 0x980]
00437383  fsub       dword ptr [edi + 0x9f4]
00437389  fstp       dword ptr [esp + 0x2c]
0043738d  mov        ecx, dword ptr [esp + 0x2c]
00437391  fld        dword ptr [edi + 0x984]
00437397  fsub       dword ptr [edi + 0x9f8]
0043739d  mov        dword ptr [edi + 0xc474], eax
004373a3  mov        dword ptr [edi + 0xc478], ecx
004373a9  fstp       dword ptr [esp + 0x30]
004373ad  mov        edx, dword ptr [esp + 0x30]
004373b1  mov        dword ptr [edi + 0xc47c], edx
004373b7  fld        dword ptr [edi + 0x9f0]
004373bd  fadd       dword ptr [edi + 0x97c]
004373c3  fstp       dword ptr [esp + 0x28]
004373c7  mov        eax, dword ptr [esp + 0x28]
004373cb  fld        dword ptr [edi + 0x9f4]
004373d1  fadd       dword ptr [edi + 0x980]
004373d7  fstp       dword ptr [esp + 0x2c]
004373db  mov        ecx, dword ptr [esp + 0x2c]
004373df  fld        dword ptr [edi + 0x9f8]
004373e5  fadd       dword ptr [edi + 0x984]
004373eb  mov        dword ptr [edi + 0xc480], eax
004373f1  mov        dword ptr [edi + 0xc484], ecx
004373f7  fstp       dword ptr [esp + 0x30]
004373fb  mov        edx, dword ptr [esp + 0x30]
004373ff  mov        dword ptr [edi + 0xc488], edx
00437405  mov        edx, dword ptr [edi + 0xa34]
0043740b  mov        ecx, dword ptr [edi + 0xa3c]
00437411  mov        dword ptr [edi + 0xa30], edx
00437417  fld        dword ptr [ecx]
00437419  fld        qword ptr [0x4a3db0]
0043741f  fcom       st(1)
00437421  fnstsw     ax
00437423  fstp       st(1)
00437425  test       ah, 5
00437428  jp         0x437454

// block 0043742a
0043742a  fld        dword ptr [0x4a3dac]
00437430  fcomp      dword ptr [ecx]
00437432  fnstsw     ax
00437434  test       ah, 0x41
00437437  jne        0x437454

// block 00437439
00437439  fld        dword ptr [edi + 0xa38]
0043743f  inc        edx
00437440  fadd       qword ptr [0x4a3ce0]
00437446  mov        dword ptr [edi + 0xa34], edx
0043744c  fstp       dword ptr [edi + 0xa38]
00437452  jmp        0x437475

// block 00437454
00437454  fld        dword ptr [ecx]
00437456  fadd       dword ptr [edi + 0xa38]
0043745c  fstp       dword ptr [esp + 0x18]
00437460  fld        dword ptr [esp + 0x18]
00437464  fst        dword ptr [edi + 0xa38]
0043746a  call       0x4931e0

// block 0043746f
0043746f  mov        dword ptr [edi + 0xa34], eax

// block 00437475
00437475  mov        edx, dword ptr [edi + 0xa48]
0043747b  mov        ecx, dword ptr [edi + 0xa50]
00437481  mov        dword ptr [edi + 0xa44], edx
00437487  fld        dword ptr [ecx]
00437489  fcompp     
0043748b  fnstsw     ax
0043748d  test       ah, 0x41
00437490  jne        0x4374bc

// block 00437492
00437492  fld        dword ptr [0x4a3dac]
00437498  fcomp      dword ptr [ecx]
0043749a  fnstsw     ax
0043749c  test       ah, 0x41
0043749f  jne        0x4374bc

// block 004374a1
004374a1  fld        dword ptr [edi + 0xa4c]
004374a7  inc        edx
004374a8  fadd       qword ptr [0x4a3ce0]
004374ae  mov        dword ptr [edi + 0xa48], edx
004374b4  fstp       dword ptr [edi + 0xa4c]
004374ba  jmp        0x4374dd

// block 004374bc
004374bc  fld        dword ptr [ecx]
004374be  fadd       dword ptr [edi + 0xa4c]
004374c4  fstp       dword ptr [esp + 0x18]
004374c8  fld        dword ptr [esp + 0x18]
004374cc  fst        dword ptr [edi + 0xa4c]
004374d2  call       0x4931e0

// block 004374d7
004374d7  mov        dword ptr [edi + 0xa48], eax

// block 004374dd
004374dd  mov        esi, dword ptr [0x4b43e4]
004374e3  cmp        dword ptr [esi + 0x6d30], 0
004374ea  mov        ecx, dword ptr [0x4b43dc]
004374f0  jne        0x43753d

// block 004374f2
004374f2  test       ecx, ecx
004374f4  je         0x43753d

// block 004374f6
004374f6  cmp        dword ptr [ecx + 0x70], 0
004374fa  je         0x43753d

// block 004374fc
004374fc  mov        eax, dword ptr [edi + 0xa34]
00437502  cdq        
00437503  mov        ebx, 0x3c
00437508  idiv       ebx
0043750a  test       edx, edx
0043750c  jne        0x43753d

// block 0043750e
0043750e  mov        eax, dword ptr [0x4b0ccc]
00437513  inc        eax
00437514  cmp        eax, 0x400
00437519  mov        dword ptr [0x4b0ccc], eax
0043751e  jle        0x43752c

// block 00437520
00437520  mov        dword ptr [0x4b0ccc], 0x400
0043752a  jmp        0x43753d

// block 0043752c
0043752c  cmp        eax, 0xfffffc00
00437531  jge        0x43753d

// block 00437533
00437533  mov        dword ptr [0x4b0ccc], 0xfffffc00

// block 0043753d
0043753d  xor        edx, edx
0043753f  cmp        dword ptr [esi + 0x6d30], edx
00437545  jne        0x437573

// block 00437547
00437547  cmp        ecx, edx
00437549  je         0x437573

// block 0043754b
0043754b  cmp        dword ptr [ecx + 0x70], edx
0043754e  je         0x437573

// block 00437550
00437550  test       byte ptr [esi + 0x6d18], 0x10
00437557  jne        0x437573

// block 00437559
00437559  push       edi
0043755a  call       0x439a40

// block 0043755f
0043755f  push       edi
00437560  call       0x439b10

// block 00437565
00437565  mov        eax, 1
0043756a  pop        edi
0043756b  pop        esi
0043756c  pop        ebx
0043756d  mov        esp, ebp
0043756f  pop        ebp
00437570  ret        4

// block 00437573
00437573  mov        eax, dword ptr [edi + 0xc430]
00437579  test       al, 1
0043757b  jne        0x4375a8

// block 0043757d
0043757d  fldz       
0043757f  or         eax, 1
00437582  fstp       dword ptr [edi + 0xc428]
00437588  mov        dword ptr [edi + 0xc424], edx
0043758e  mov        dword ptr [edi + 0xc420], 0xfff0bdc1
00437598  mov        dword ptr [edi + 0xc42c], 0x4b2ed0
004375a2  mov        dword ptr [edi + 0xc430], eax

// block 004375a8
004375a8  fld        dword ptr [0x4a3da8]
004375ae  mov        dword ptr [edi + 0xc424], 0xffffffff
004375b8  fstp       dword ptr [edi + 0xc428]
004375be  mov        dword ptr [edi + 0xc420], 0xfffffffe
004375c8  push       edi
004375c9  mov        dword ptr [edi + 0x8980], edx
004375cf  mov        byte ptr [edi + 0x8984], dl
004375d5  call       0x439b10

// block 004375da
004375da  pop        edi
004375db  pop        esi
004375dc  mov        eax, 1
004375e1  pop        ebx
004375e2  mov        esp, ebp
004375e4  pop        ebp
004375e5  ret        4
