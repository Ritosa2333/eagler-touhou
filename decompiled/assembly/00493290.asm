
// block 00493290
00493290  cmp        dword ptr [0x4d52d4], 0
00493297  je         0x493de4

// block 0049329d
0049329d  sub        esp, 8
004932a0  stmxcsr    dword ptr [esp + 4]
004932a5  mov        eax, dword ptr [esp + 4]
004932a9  and        eax, 0x1f80
004932ae  cmp        eax, 0x1f80
004932b3  jne        0x4932c4

// block 004932b5
004932b5  fnstcw     word ptr [esp]
004932b8  mov        ax, word ptr [esp]
004932bc  and        ax, 0x7f
004932c0  cmp        ax, 0x7f

// block 004932c4
004932c4  lea        esp, [esp + 8]
004932c8  jne        0x493de4

// block 004932ce
004932ce  jmp        0x4932d0

// block 004932d0
004932d0  movq       xmm0, qword ptr [esp + 4]
004932d6  movapd     xmm2, xmmword ptr [0x4a4480]
004932de  movapd     xmm1, xmm0
004932e2  movapd     xmm7, xmm0
004932e6  psrlq      xmm0, 0x34
004932eb  movd       eax, xmm0
004932ef  andpd      xmm0, xmmword ptr [0x4a44b0]
004932f7  psubd      xmm2, xmm0
004932fb  psrlq      xmm1, xmm2
004932ff  test       eax, 0x800
00493304  jne        0x493352

// block 00493306
00493306  cmp        eax, 0x3ff
0049330b  jl         0x49338a

// block 0049330d
0049330d  psllq      xmm1, xmm2
00493311  cmp        eax, 0x432
00493316  jg         0x493323

// block 00493318
00493318  movq       qword ptr [esp + 4], xmm1
0049331e  fld        qword ptr [esp + 4]
00493322  ret        

// block 00493323
00493323  ucomisd    xmm7, xmm7
00493327  jnp        0x49334d

// block 00493329
00493329  mov        edx, 0x3ed
0049332e  sub        esp, 0x10
00493331  mov        dword ptr [esp + 0xc], edx
00493335  mov        edx, esp
00493337  add        edx, 0x14
0049333a  mov        dword ptr [esp + 8], edx
0049333e  mov        dword ptr [esp + 4], edx
00493342  mov        dword ptr [esp], edx
00493345  call       0x493b07

// block 0049334a
0049334a  add        esp, 0x10

// block 0049334d
0049334d  fld        qword ptr [esp + 4]
00493351  ret        

// block 00493352
00493352  movq       xmm0, qword ptr [esp + 4]
00493358  psllq      xmm1, xmm2
0049335c  movapd     xmm3, xmm0
00493360  cmpltpd    xmm0, xmm1
00493365  cmp        eax, 0xbff
0049336a  jl         0x49338d

// block 0049336c
0049336c  cmp        eax, 0xc32
00493371  jg         0x493323

// block 00493373
00493373  andpd      xmm0, xmmword ptr [0x4a4470]
0049337b  subsd      xmm1, xmm0
0049337f  movq       qword ptr [esp + 4], xmm1
00493385  fld        qword ptr [esp + 4]
00493389  ret        

// block 0049338a
0049338a  fldz       
0049338c  ret        

// block 0049338d
0049338d  cmpltpd    xmm3, xmmword ptr [0x4a44a0]
00493396  orpd       xmm3, xmmword ptr [0x4a44a0]
0049339e  andpd      xmm3, xmmword ptr [0x4a4490]
004933a6  movq       qword ptr [esp + 4], xmm3
004933ac  fld        qword ptr [esp + 4]
004933b0  ret        

// block 00493de4
00493de4  mov        edi, edi
00493de6  push       ebp
00493de7  mov        ebp, esp
00493de9  push       ecx
00493dea  push       ecx
00493deb  push       ebx
00493dec  push       esi
00493ded  mov        esi, 0xffff
00493df2  push       esi
00493df3  push       dword ptr [0x4b35b8]
00493df9  call       0x491181

// block 00493dfe
00493dfe  fld        qword ptr [ebp + 8]
00493e01  pop        ecx
00493e02  pop        ecx
00493e03  mov        ecx, dword ptr [ebp + 0xe]
00493e06  mov        ebx, eax
00493e08  mov        eax, 0x7ff0
00493e0d  and        ecx, eax
00493e0f  push       ecx
00493e10  push       ecx
00493e11  fstp       qword ptr [esp]
00493e14  cmp        cx, ax
00493e17  jne        0x493e6e

// block 00493e19
00493e19  call       0x496a9d

// block 00493e1e
00493e1e  pop        ecx
00493e1f  pop        ecx
00493e20  test       eax, eax
00493e22  jle        0x493e51

// block 00493e24
00493e24  cmp        eax, 2
00493e27  jle        0x493e43

// block 00493e29
00493e29  cmp        eax, 3
00493e2c  jne        0x493e51

// block 00493e2e
00493e2e  fld        qword ptr [ebp + 8]
00493e31  push       ebx
00493e32  push       ecx
00493e33  push       ecx
00493e34  fstp       qword ptr [esp]
00493e37  push       0xb
00493e39  call       0x49679a

// block 00493e3e
00493e3e  add        esp, 0x10
00493e41  jmp        0x493eb5

// block 00493e43
00493e43  push       esi
00493e44  push       ebx
00493e45  call       0x491181

// block 00493e4a
00493e4a  fld        qword ptr [ebp + 8]
00493e4d  pop        ecx
00493e4e  pop        ecx
00493e4f  jmp        0x493eb5

// block 00493e51
00493e51  fld        qword ptr [ebp + 8]
00493e54  push       ebx
00493e55  fadd       qword ptr [0x4a3ce0]
00493e5b  sub        esp, 0x10
00493e5e  fstp       qword ptr [esp + 8]
00493e62  fld        qword ptr [ebp + 8]
00493e65  fstp       qword ptr [esp]
00493e68  push       0xb
00493e6a  push       8
00493e6c  jmp        0x493ead

// block 00493e6e
00493e6e  call       0x4969f1

// block 00493e73
00493e73  fstp       qword ptr [ebp - 8]
00493e76  fld        qword ptr [ebp + 8]
00493e79  pop        ecx
00493e7a  fcomp      qword ptr [ebp - 8]
00493e7d  pop        ecx
00493e7e  fnstsw     ax
00493e80  test       ah, 0x44
00493e83  jp         0x493e93

// block 00493e85
00493e85  push       esi
00493e86  push       ebx
00493e87  call       0x491181

// block 00493e8c
00493e8c  fld        qword ptr [ebp - 8]
00493e8f  pop        ecx
00493e90  pop        ecx
00493e91  jmp        0x493eb5

// block 00493e93
00493e93  test       bl, 0x20
00493e96  jne        0x493e85

// block 00493e98
00493e98  fld        qword ptr [ebp - 8]
00493e9b  push       ebx
00493e9c  sub        esp, 0x10
00493e9f  fstp       qword ptr [esp + 8]
00493ea3  fld        qword ptr [ebp + 8]
00493ea6  fstp       qword ptr [esp]
00493ea9  push       0xb
00493eab  push       0x10

// block 00493ead
00493ead  call       0x496850

// block 00493eb2
00493eb2  add        esp, 0x1c

// block 00493eb5
00493eb5  pop        esi
00493eb6  pop        ebx
00493eb7  leave      
00493eb8  ret        
