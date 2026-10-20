
// block 00428b00
00428b00  push       ebp
00428b01  mov        ebp, esp
00428b03  and        esp, 0xfffffff8
00428b06  sub        esp, 0x220
00428b0c  push       ebx
00428b0d  push       ebp
00428b0e  push       esi
00428b0f  push       edi
00428b10  mov        ebp, ecx
00428b12  cmp        dword ptr [ebp + 0x42c], 0x12
00428b19  jge        0x429118

// block 00428b1f
00428b1f  nop        

// block 00428b20
00428b20  mov        edx, dword ptr [ebp + 0x42c]
00428b26  fldz       
00428b28  lea        eax, [edx + edx*2]
00428b2b  mov        ecx, dword ptr [ebp + eax*8 + 0x494]
00428b32  lea        esi, [ebp + eax*8 + 0x484]
00428b39  xor        edi, edi
00428b3b  cmp        ecx, edi
00428b3d  je         0x429116

// block 00428b43
00428b43  cmp        dword ptr [esi + 0x14], edi
00428b46  jne        0x428b54

// block 00428b48
00428b48  cmp        dword ptr [ebp + 0x430], edi
00428b4e  jne        0x429116

// block 00428b54
00428b54  cmp        ecx, 0x80000000
00428b5a  jne        0x428b6a

// block 00428b5c
00428b5c  inc        edx
00428b5d  fstp       st(0)
00428b5f  mov        dword ptr [ebp + 0x42c], edx
00428b65  jmp        0x429101

// block 00428b6a
00428b6a  mov        eax, ecx
00428b6c  cmp        eax, 0x1000
00428b71  ja         0x428e86

// block 00428b77
00428b77  je         0x428e26

// block 00428b7d
00428b7d  cmp        eax, 0x20
00428b80  ja         0x428cc4

// block 00428b86
00428b86  je         0x428d2a

// block 00428b8c
00428b8c  dec        eax
00428b8d  cmp        eax, 0xf
00428b90  ja         0x428dcd

// block 00428b96
00428b96  movzx      edx, byte ptr [eax + 0x429134]
00428b9d  jmp        dword ptr [edx*4 + 0x429120]

// block 00428ba4
00428ba4  or         dword ptr [ebp + 0x430], 1
00428bab  fstp       st(0)
00428bad  push       edi
00428bae  lea        eax, [ebp + 0x84]
00428bb4  call       0x4067e0

// block 00428bb9
00428bb9  fldz       
00428bbb  fstp       dword ptr [ebp + 0xa8]
00428bc1  jmp        0x4290fb

// block 00428bc6
00428bc6  or         dword ptr [ebp + 0x430], 4
00428bcd  fstp       st(0)
00428bcf  fld        dword ptr [esi]
00428bd1  fstp       dword ptr [ebp + 0xcc]
00428bd7  fld        dword ptr [esi + 4]
00428bda  fcomp      qword ptr [0x4a3e70]
00428be0  fnstsw     ax
00428be2  test       ah, 0x41
00428be5  jp         0x428bec

// block 00428be7
00428be7  fld        dword ptr [ebp + 0x68]
00428bea  jmp        0x428c09

// block 00428bec
00428bec  fld        dword ptr [esi + 4]
00428bef  fcomp      qword ptr [0x4a4268]
00428bf5  fnstsw     ax
00428bf7  test       ah, 1
00428bfa  jne        0x428c06

// block 00428bfc
00428bfc  lea        ecx, [ebp + 0x50]
00428bff  call       0x4377a0

// block 00428c04
00428c04  jmp        0x428c09

// block 00428c06
00428c06  fld        dword ptr [esi + 4]

// block 00428c09
00428c09  fstp       dword ptr [esp + 0x14]
00428c0d  push       edi
00428c0e  fld        dword ptr [esp + 0x18]
00428c12  lea        eax, [ebp + 0xb8]
00428c18  fstp       dword ptr [ebp + 0xd0]
00428c1e  call       0x4067e0

// block 00428c23
00428c23  fld        dword ptr [ebp + 0xcc]
00428c29  mov        eax, dword ptr [esi + 8]
00428c2c  sub        esp, 8
00428c2f  fstp       dword ptr [esp + 4]
00428c33  lea        ecx, [ebp + 0xd4]
00428c39  fld        dword ptr [ebp + 0xd0]
00428c3f  mov        dword ptr [ebp + 0xe0], eax
00428c45  fstp       dword ptr [esp]
00428c48  call       0x42e880

// block 00428c4d
00428c4d  cmp        dword ptr [ebp + 0x42c], edi
00428c53  je         0x4290fb

// block 00428c59
00428c59  mov        edx, dword ptr [ebp + 0x638]
00428c5f  cmp        edx, edi
00428c61  jl         0x4290fb

// block 00428c67
00428c67  call       0x453d90

// block 00428c6c
00428c6c  jmp        0x4290fb

// block 00428c71
00428c71  or         dword ptr [ebp + 0x430], 8
00428c78  fstp       st(0)
00428c7a  fld        dword ptr [esi]
00428c7c  push       edi
00428c7d  fstp       dword ptr [ebp + 0x100]
00428c83  lea        eax, [ebp + 0xec]
00428c89  fld        dword ptr [esi + 4]
00428c8c  fstp       dword ptr [ebp + 0x104]
00428c92  call       0x4067e0

// block 00428c97
00428c97  mov        ecx, dword ptr [esi + 8]
00428c9a  mov        dword ptr [ebp + 0x114], ecx
00428ca0  cmp        dword ptr [ebp + 0x42c], edi
00428ca6  je         0x4290fb

// block 00428cac
00428cac  mov        edx, dword ptr [ebp + 0x638]
00428cb2  cmp        edx, edi
00428cb4  jl         0x4290fb

// block 00428cba
00428cba  call       0x453d90

// block 00428cbf
00428cbf  jmp        0x4290fb

// block 00428cc4
00428cc4  cmp        eax, 0x200
00428cc9  ja         0x428dd4

// block 00428ccf
00428ccf  je         0x428dc4

// block 00428cd5
00428cd5  cmp        eax, 0x40
00428cd8  je         0x428d2a

// block 00428cda
00428cda  cmp        eax, 0x100
00428cdf  jne        0x428dcd

// block 00428ce5
00428ce5  cmp        dword ptr [esi + 8], edi
00428ce8  jle        0x428dcd

// block 00428cee
00428cee  or         dword ptr [ebp + 0x430], ecx
00428cf4  fcomp      dword ptr [esi]
00428cf6  fnstsw     ax
00428cf8  test       ah, 0x41
00428cfb  jp         0x428d01

// block 00428cfd
00428cfd  fld        dword ptr [esi]
00428cff  jmp        0x428d04

// block 00428d01
00428d01  fld        dword ptr [ebp + 0x74]

// block 00428d04
00428d04  fstp       dword ptr [ebp + 0x168]
00428d0a  dec        dword ptr [esi + 8]
00428d0d  mov        eax, dword ptr [esi + 8]
00428d10  mov        dword ptr [ebp + 0x180], eax
00428d16  mov        dword ptr [ebp + 0x17c], edi
00428d1c  mov        edx, dword ptr [esi + 0xc]
00428d1f  mov        dword ptr [ebp + 0x184], edx
00428d25  jmp        0x4290fb

// block 00428d2a
00428d2a  or         dword ptr [ebp + 0x430], ecx
00428d30  fld        dword ptr [esi]
00428d32  fstp       dword ptr [ebp + 0x138]
00428d38  fld        dword ptr [esi + 4]
00428d3b  fcomp      qword ptr [0x4a4150]
00428d41  fnstsw     ax
00428d43  test       ah, 0x41
00428d46  jne        0x428d4d

// block 00428d48
00428d48  fld        dword ptr [esi + 4]
00428d4b  jmp        0x428d50

// block 00428d4d
00428d4d  fld        dword ptr [ebp + 0x74]

// block 00428d50
00428d50  fstp       dword ptr [esp + 0x14]
00428d54  fld        dword ptr [esp + 0x14]
00428d58  fstp       dword ptr [ebp + 0x134]
00428d5e  mov        eax, dword ptr [ebp + 0x130]
00428d64  test       al, 1
00428d66  jne        0x428d91

// block 00428d68
00428d68  or         eax, 1
00428d6b  fst        dword ptr [ebp + 0x128]
00428d71  mov        dword ptr [ebp + 0x124], edi
00428d77  mov        dword ptr [ebp + 0x120], 0xfff0bdc1
00428d81  mov        dword ptr [ebp + 0x12c], 0x4b2ed0
00428d8b  mov        dword ptr [ebp + 0x130], eax

// block 00428d91
00428d91  fstp       dword ptr [ebp + 0x128]
00428d97  mov        dword ptr [ebp + 0x124], edi
00428d9d  mov        dword ptr [ebp + 0x120], 0xffffffff
00428da7  mov        eax, dword ptr [esi + 8]
00428daa  mov        dword ptr [ebp + 0x148], eax
00428db0  mov        ecx, dword ptr [esi + 0xc]
00428db3  mov        dword ptr [ebp + 0x14c], ecx
00428db9  mov        dword ptr [ebp + 0x150], edi
00428dbf  jmp        0x4290fb

// block 00428dc4
00428dc4  mov        edx, dword ptr [esi + 8]
00428dc7  mov        dword ptr [ebp + 0x44c], edx

// block 00428dcd
00428dcd  fstp       st(0)
00428dcf  jmp        0x4290fb

// block 00428dd4
00428dd4  fstp       st(0)
00428dd6  cmp        eax, 0x800
00428ddb  jne        0x4290fb

// block 00428de1
00428de1  mov        eax, dword ptr [esi + 8]
00428de4  mov        ecx, dword ptr [0x4b43c8]
00428dea  imul       eax, eax, 0xd0
00428df0  mov        edi, dword ptr [eax + 0x4af280]
00428df6  add        edi, dword ptr [esi + 0xc]
00428df9  mov        ebx, dword ptr [ecx + 0x4debdc]
00428dff  lea        esi, [ebp + 0x63c]
00428e05  call       0x402520

// block 00428e0a
00428e0a  push       edi
00428e0b  push       esi
00428e0c  mov        ecx, ebx
00428e0e  mov        byte ptr [esi + 0x49d], 0x10
00428e15  mov        byte ptr [esi + 0x49c], 0x10
00428e1c  call       0x454d10

// block 00428e21
00428e21  jmp        0x4290fb

// block 00428e26
00428e26  or         dword ptr [ebp + 0x430], ecx
00428e2c  mov        eax, dword ptr [ebp + 0x198]
00428e32  mov        esi, dword ptr [esi + 8]
00428e35  mov        dword ptr [esp + 0x14], esi
00428e39  test       al, 1
00428e3b  jne        0x428e68

// block 00428e3d
00428e3d  or         eax, 1
00428e40  fstp       dword ptr [ebp + 0x190]
00428e46  mov        dword ptr [ebp + 0x18c], edi
00428e4c  mov        dword ptr [ebp + 0x188], 0xfff0bdc1
00428e56  mov        dword ptr [ebp + 0x194], 0x4b2ed0
00428e60  mov        dword ptr [ebp + 0x198], eax
00428e66  jmp        0x428e6a

// block 00428e68
00428e68  fstp       st(0)

// block 00428e6a
00428e6a  fild       dword ptr [esp + 0x14]
00428e6e  mov        dword ptr [ebp + 0x18c], esi
00428e74  dec        esi
00428e75  mov        dword ptr [ebp + 0x188], esi
00428e7b  fstp       dword ptr [ebp + 0x190]
00428e81  jmp        0x4290fb

// block 00428e86
00428e86  cmp        eax, 0x80000
00428e8b  ja         0x42901a

// block 00428e91
00428e91  fstp       st(0)
00428e93  je         0x428f0f

// block 00428e95
00428e95  cmp        eax, 0x20000
00428e9a  ja         0x428eea

// block 00428e9c
00428e9c  je         0x428ed0

// block 00428e9e
00428e9e  cmp        eax, 0x2000
00428ea3  je         0x428ec4

// block 00428ea5
00428ea5  cmp        eax, 0x4000
00428eaa  jne        0x4290fb

// block 00428eb0
00428eb0  fld        dword ptr [ebp + 0x50]
00428eb3  mov        eax, dword ptr [esi + 8]
00428eb6  push       ecx
00428eb7  fstp       dword ptr [esp]
00428eba  call       0x453e20

// block 00428ebf
00428ebf  jmp        0x4290fb

// block 00428ec4
00428ec4  mov        dword ptr [ebp + 0xc], 3
00428ecb  jmp        0x4290fb

// block 00428ed0
00428ed0  or         dword ptr [ebp + 0x430], ecx
00428ed6  mov        edx, dword ptr [esi + 8]
00428ed9  push       edx
00428eda  lea        eax, [ebp + 0x1bc]
00428ee0  call       0x4067e0

// block 00428ee5
00428ee5  jmp        0x4290fb

// block 00428eea
00428eea  cmp        eax, 0x40000
00428eef  jne        0x4290fb

// block 00428ef5
00428ef5  or         dword ptr [ebp + 0x430], ecx
00428efb  mov        eax, dword ptr [esi + 8]
00428efe  push       eax
00428eff  lea        eax, [ebp + 0x1bc]
00428f05  call       0x4067e0

// block 00428f0a
00428f0a  jmp        0x4290fb

// block 00428f0f
00428f0f  push       0x214
00428f14  lea        ecx, [esp + 0x1c]
00428f18  push       edi
00428f19  push       ecx
00428f1a  call       0x477420

// block 00428f1f
00428f1f  fld        dword ptr [ebp + 0x6c]
00428f22  add        esp, 4
00428f25  fstp       dword ptr [esp + 4]
00428f29  lea        ecx, [esp + 0x24]
00428f2d  fld        dword ptr [ebp + 0x68]
00428f30  mov        dword ptr [esp + 0x228], 0xffffffff
00428f3b  fstp       dword ptr [esp]
00428f3e  call       0x42e880

// block 00428f43
00428f43  fld        dword ptr [ebp + 0x50]
00428f46  mov        eax, dword ptr [esi + 8]
00428f49  fadd       dword ptr [esp + 0x1c]
00428f4d  movzx      ecx, byte ptr [esi + 0xa]
00428f51  mov        edx, eax
00428f53  shr        edx, 0x18
00428f56  fstp       dword ptr [esp + 0x1c]
00428f5a  mov        ebx, eax
00428f5c  fld        dword ptr [ebp + 0x54]
00428f5f  and        edx, 0x7f
00428f62  fadd       dword ptr [esp + 0x20]
00428f66  and        eax, 0xff
00428f6b  add        esi, 0x18
00428f6e  mov        word ptr [esp + 0x214], dx
00428f76  movzx      dx, byte ptr [esi - 0xf]
00428f7b  fstp       dword ptr [esp + 0x20]
00428f7f  fldz       
00428f81  fstp       dword ptr [esp + 0x24]
00428f85  fld        dword ptr [esi - 0x18]
00428f88  mov        dword ptr [esp + 0x224], eax
00428f8f  mov        ax, word ptr [esi - 0xc]
00428f93  fstp       dword ptr [esp + 0x30]
00428f97  fld        dword ptr [esi - 0x14]
00428f9a  inc        dword ptr [ebp + 0x42c]
00428fa0  fstp       dword ptr [esp + 0x34]
00428fa4  mov        word ptr [esp + 0x18], cx
00428fa9  movzx      ecx, word ptr [esi + 8]
00428fad  fld        dword ptr [esi]
00428faf  fstp       dword ptr [esp + 0x28]
00428fb3  fld        dword ptr [esi + 4]
00428fb6  mov        word ptr [esp + 0x1a], dx
00428fbb  fstp       dword ptr [esp + 0x2c]
00428fbf  mov        edx, dword ptr [esi + 0xc]
00428fc2  mov        word ptr [esp + 0x212], cx
00428fca  mov        word ptr [esp + 0x210], ax
00428fd2  mov        dword ptr [esp + 0x218], edx
00428fd9  lea        esi, [ebp + 0x484]
00428fdf  mov        ecx, 0x6c
00428fe4  lea        edi, [esp + 0x3c]

// block 00428fe8
00428fe8  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00428fea
00428fea  lea        esi, [esp + 0x18]
00428fee  and        ebx, 0x80000000
00428ff4  call       0x40b5e0

// block 00428ff9
00428ff9  inc        dword ptr [ebp + 0x42c]
00428fff  test       ebx, ebx
00429001  je         0x429101

// block 00429007
00429007  mov        eax, dword ptr [ebp]
0042900a  mov        edx, dword ptr [eax + 0x14]
0042900d  push       0
0042900f  push       0
00429011  mov        ecx, ebp
00429013  call       edx

// block 00429015
00429015  jmp        0x4290fb

// block 0042901a
0042901a  cmp        eax, 0x800000
0042901f  ja         0x4290cd

// block 00429025
00429025  je         0x42905e

// block 00429027
00429027  fstp       st(0)
00429029  cmp        eax, 0x200000
0042902e  je         0x429049

// block 00429030
00429030  cmp        eax, 0x400000
00429035  jne        0x4290fb

// block 0042903b
0042903b  mov        eax, dword ptr [esi + 8]
0042903e  mov        dword ptr [ebp + 0x42c], eax
00429044  jmp        0x429101

// block 00429049
00429049  mov        ecx, dword ptr [esi + 8]
0042904c  inc        edx
0042904d  mov        dword ptr [ebp + 0x80], ecx
00429053  mov        dword ptr [ebp + 0x42c], edx
00429059  jmp        0x429101

// block 0042905e
0042905e  or         dword ptr [ebp + 0x430], 0x800000
00429068  fld        dword ptr [esi]
0042906a  fstp       dword ptr [ebp + 0x238]
00429070  fld        dword ptr [esi + 4]
00429073  fstp       dword ptr [ebp + 0x23c]
00429079  mov        eax, dword ptr [ebp + 0x234]
0042907f  test       al, 1
00429081  jne        0x4290ac

// block 00429083
00429083  or         eax, 1
00429086  fst        dword ptr [ebp + 0x22c]
0042908c  mov        dword ptr [ebp + 0x228], edi
00429092  mov        dword ptr [ebp + 0x224], 0xfff0bdc1
0042909c  mov        dword ptr [ebp + 0x230], 0x4b2ed0
004290a6  mov        dword ptr [ebp + 0x234], eax

// block 004290ac
004290ac  fstp       dword ptr [ebp + 0x22c]
004290b2  mov        dword ptr [ebp + 0x228], edi
004290b8  mov        dword ptr [ebp + 0x224], 0xffffffff
004290c2  mov        edx, dword ptr [esi + 8]
004290c5  mov        dword ptr [ebp + 0x24c], edx
004290cb  jmp        0x4290fb

// block 004290cd
004290cd  fstp       st(0)
004290cf  cmp        eax, 0x10000000
004290d4  jne        0x4290fb

// block 004290d6
004290d6  cmp        dword ptr [esi + 8], edi
004290d9  je         0x4290f1

// block 004290db
004290db  mov        eax, dword ptr [ebp + 0xab8]
004290e1  and        eax, 0xffffff3f
004290e6  or         eax, 0x20
004290e9  mov        dword ptr [ebp + 0xab8], eax
004290ef  jmp        0x4290fb

// block 004290f1
004290f1  and        dword ptr [ebp + 0xab8], 0xffffff1f

// block 004290fb
004290fb  inc        dword ptr [ebp + 0x42c]

// block 00429101
00429101  cmp        dword ptr [ebp + 0x42c], 0x12
00429108  jl         0x428b20

// block 0042910e
0042910e  pop        edi
0042910f  pop        esi
00429110  pop        ebp
00429111  pop        ebx
00429112  mov        esp, ebp
00429114  pop        ebp
00429115  ret        

// block 00429116
00429116  fstp       st(0)

// block 00429118
00429118  pop        edi
00429119  pop        esi
0042911a  pop        ebp
0042911b  pop        ebx
0042911c  mov        esp, ebp
0042911e  pop        ebp
0042911f  ret        
