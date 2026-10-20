
// block 0042d030
0042d030  push       ebp
0042d031  mov        ebp, esp
0042d033  and        esp, 0xfffffff8
0042d036  sub        esp, 0x220
0042d03c  push       ebx
0042d03d  push       ebp
0042d03e  push       esi
0042d03f  push       edi
0042d040  mov        ebp, ecx
0042d042  cmp        dword ptr [ebp + 0x42c], 0x12
0042d049  jge        0x42d6a3

// block 0042d04f
0042d04f  nop        

// block 0042d050
0042d050  mov        edi, dword ptr [ebp + 0x42c]
0042d056  fldz       
0042d058  lea        eax, [edi + edi*2]
0042d05b  mov        edx, dword ptr [ebp + eax*8 + 0x48c]
0042d062  lea        esi, [ebp + eax*8 + 0x47c]
0042d069  xor        ebx, ebx
0042d06b  cmp        edx, ebx
0042d06d  je         0x42d6a1

// block 0042d073
0042d073  cmp        dword ptr [esi + 0x14], ebx
0042d076  jne        0x42d084

// block 0042d078
0042d078  cmp        dword ptr [ebp + 0x430], ebx
0042d07e  jne        0x42d6a1

// block 0042d084
0042d084  mov        eax, dword ptr [ebp + 0x430]
0042d08a  test       edx, eax
0042d08c  jne        0x42d6a1

// block 0042d092
0042d092  cmp        edx, 0x80000000
0042d098  jne        0x42d0a8

// block 0042d09a
0042d09a  inc        edi
0042d09b  fstp       st(0)
0042d09d  mov        dword ptr [ebp + 0x42c], edi
0042d0a3  jmp        0x42d68c

// block 0042d0a8
0042d0a8  mov        ecx, edx
0042d0aa  cmp        ecx, 0x1000
0042d0b0  ja         0x42d3fa

// block 0042d0b6
0042d0b6  je         0x42d398

// block 0042d0bc
0042d0bc  cmp        ecx, 0x40
0042d0bf  ja         0x42d2a5

// block 0042d0c5
0042d0c5  je         0x42d209

// block 0042d0cb
0042d0cb  dec        ecx
0042d0cc  cmp        ecx, 0x1f
0042d0cf  ja         0x42d678

// block 0042d0d5
0042d0d5  movzx      ecx, byte ptr [ecx + 0x42d6c0]
0042d0dc  jmp        dword ptr [ecx*4 + 0x42d6ac]

// block 0042d0e3
0042d0e3  or         eax, 1
0042d0e6  fstp       st(0)
0042d0e8  mov        dword ptr [ebp + 0x430], eax
0042d0ee  push       ebx
0042d0ef  lea        eax, [ebp + 0x84]
0042d0f5  call       0x4067e0

// block 0042d0fa
0042d0fa  fldz       
0042d0fc  fstp       dword ptr [ebp + 0xa8]
0042d102  jmp        0x42d686

// block 0042d107
0042d107  or         eax, 4
0042d10a  fstp       st(0)
0042d10c  mov        dword ptr [ebp + 0x430], eax
0042d112  fld        dword ptr [esi]
0042d114  fstp       dword ptr [ebp + 0xcc]
0042d11a  fld        dword ptr [esi + 4]
0042d11d  fcomp      qword ptr [0x4a3e70]
0042d123  fnstsw     ax
0042d125  test       ah, 0x41
0042d128  jp         0x42d12f

// block 0042d12a
0042d12a  fld        dword ptr [ebp + 0x68]
0042d12d  jmp        0x42d14c

// block 0042d12f
0042d12f  fld        dword ptr [esi + 4]
0042d132  fcomp      qword ptr [0x4a4268]
0042d138  fnstsw     ax
0042d13a  test       ah, 1
0042d13d  jne        0x42d149

// block 0042d13f
0042d13f  lea        ecx, [ebp + 0x50]
0042d142  call       0x4377a0

// block 0042d147
0042d147  jmp        0x42d14c

// block 0042d149
0042d149  fld        dword ptr [esi + 4]

// block 0042d14c
0042d14c  fstp       dword ptr [esp + 0x14]
0042d150  push       ebx
0042d151  fld        dword ptr [esp + 0x18]
0042d155  lea        eax, [ebp + 0xb8]
0042d15b  fstp       dword ptr [ebp + 0xd0]
0042d161  call       0x4067e0

// block 0042d166
0042d166  fld        dword ptr [ebp + 0xcc]
0042d16c  mov        edx, dword ptr [esi + 8]
0042d16f  sub        esp, 8
0042d172  fstp       dword ptr [esp + 4]
0042d176  lea        ecx, [ebp + 0xd4]
0042d17c  fld        dword ptr [ebp + 0xd0]
0042d182  mov        dword ptr [ebp + 0xe0], edx
0042d188  fstp       dword ptr [esp]
0042d18b  call       0x42e880

// block 0042d190
0042d190  cmp        dword ptr [ebp + 0x42c], ebx
0042d196  je         0x42d686

// block 0042d19c
0042d19c  mov        edx, dword ptr [ebp + 0x630]
0042d1a2  cmp        edx, ebx
0042d1a4  jl         0x42d686

// block 0042d1aa
0042d1aa  call       0x453d90

// block 0042d1af
0042d1af  jmp        0x42d686

// block 0042d1b4
0042d1b4  or         eax, 8
0042d1b7  fstp       st(0)
0042d1b9  mov        dword ptr [ebp + 0x430], eax
0042d1bf  fld        dword ptr [esi]
0042d1c1  fstp       dword ptr [ebp + 0x100]
0042d1c7  push       ebx
0042d1c8  fld        dword ptr [esi + 4]
0042d1cb  lea        eax, [ebp + 0xec]
0042d1d1  fstp       dword ptr [ebp + 0x104]
0042d1d7  call       0x4067e0

// block 0042d1dc
0042d1dc  mov        eax, dword ptr [esi + 8]
0042d1df  mov        dword ptr [ebp + 0x114], eax
0042d1e5  cmp        dword ptr [ebp + 0x42c], ebx
0042d1eb  je         0x42d686

// block 0042d1f1
0042d1f1  mov        edx, dword ptr [ebp + 0x630]
0042d1f7  cmp        edx, ebx
0042d1f9  jl         0x42d686

// block 0042d1ff
0042d1ff  call       0x453d90

// block 0042d204
0042d204  jmp        0x42d686

// block 0042d209
0042d209  or         eax, edx
0042d20b  mov        dword ptr [ebp + 0x430], eax
0042d211  fld        dword ptr [esi]
0042d213  fstp       dword ptr [ebp + 0x138]
0042d219  fld        dword ptr [esi + 4]
0042d21c  fcomp      qword ptr [0x4a4150]
0042d222  fnstsw     ax
0042d224  test       ah, 0x41
0042d227  jne        0x42d22e

// block 0042d229
0042d229  fld        dword ptr [esi + 4]
0042d22c  jmp        0x42d231

// block 0042d22e
0042d22e  fld        dword ptr [ebp + 0x74]

// block 0042d231
0042d231  fstp       dword ptr [esp + 0x14]
0042d235  fld        dword ptr [esp + 0x14]
0042d239  fstp       dword ptr [ebp + 0x134]
0042d23f  mov        eax, dword ptr [ebp + 0x130]
0042d245  test       al, 1
0042d247  jne        0x42d272

// block 0042d249
0042d249  or         eax, 1
0042d24c  fst        dword ptr [ebp + 0x128]
0042d252  mov        dword ptr [ebp + 0x124], ebx
0042d258  mov        dword ptr [ebp + 0x120], 0xfff0bdc1
0042d262  mov        dword ptr [ebp + 0x12c], 0x4b2ed0
0042d26c  mov        dword ptr [ebp + 0x130], eax

// block 0042d272
0042d272  fstp       dword ptr [ebp + 0x128]
0042d278  mov        dword ptr [ebp + 0x124], ebx
0042d27e  mov        dword ptr [ebp + 0x120], 0xffffffff
0042d288  mov        ecx, dword ptr [esi + 8]
0042d28b  mov        dword ptr [ebp + 0x148], ecx
0042d291  mov        edx, dword ptr [esi + 0xc]
0042d294  mov        dword ptr [ebp + 0x14c], edx
0042d29a  mov        dword ptr [ebp + 0x150], ebx
0042d2a0  jmp        0x42d686

// block 0042d2a5
0042d2a5  cmp        ecx, 0x400
0042d2ab  ja         0x42d345

// block 0042d2b1
0042d2b1  je         0x42d31e

// block 0042d2b3
0042d2b3  cmp        ecx, 0x100
0042d2b9  je         0x42d2d7

// block 0042d2bb
0042d2bb  fstp       st(0)
0042d2bd  cmp        ecx, 0x200
0042d2c3  jne        0x42d686

// block 0042d2c9
0042d2c9  mov        eax, dword ptr [esi + 8]
0042d2cc  mov        dword ptr [ebp + 0x44c], eax
0042d2d2  jmp        0x42d686

// block 0042d2d7
0042d2d7  cmp        dword ptr [esi + 8], ebx
0042d2da  jle        0x42d678

// block 0042d2e0
0042d2e0  or         eax, edx
0042d2e2  mov        dword ptr [ebp + 0x430], eax
0042d2e8  fcomp      dword ptr [esi]
0042d2ea  fnstsw     ax
0042d2ec  test       ah, 0x41
0042d2ef  jp         0x42d2f5

// block 0042d2f1
0042d2f1  fld        dword ptr [esi]
0042d2f3  jmp        0x42d2f8

// block 0042d2f5
0042d2f5  fld        dword ptr [ebp + 0x74]

// block 0042d2f8
0042d2f8  fstp       dword ptr [ebp + 0x168]
0042d2fe  dec        dword ptr [esi + 8]
0042d301  mov        eax, dword ptr [esi + 8]
0042d304  mov        dword ptr [ebp + 0x180], eax
0042d30a  mov        dword ptr [ebp + 0x17c], ebx
0042d310  mov        ecx, dword ptr [esi + 0xc]
0042d313  mov        dword ptr [ebp + 0x184], ecx
0042d319  jmp        0x42d686

// block 0042d31e
0042d31e  or         eax, edx
0042d320  fstp       st(0)
0042d322  mov        dword ptr [ebp + 0x430], eax
0042d328  mov        edx, dword ptr [esi + 8]
0042d32b  push       edx
0042d32c  lea        eax, [ebp + 0x2f4]
0042d332  call       0x4067e0

// block 0042d337
0042d337  mov        eax, dword ptr [esi + 0xc]
0042d33a  mov        dword ptr [ebp + 0x31c], eax
0042d340  jmp        0x42d686

// block 0042d345
0042d345  fstp       st(0)
0042d347  cmp        ecx, 0x800
0042d34d  jne        0x42d686

// block 0042d353
0042d353  mov        ecx, dword ptr [esi + 8]
0042d356  mov        edx, dword ptr [0x4b43c8]
0042d35c  imul       ecx, ecx, 0xd0
0042d362  mov        edi, dword ptr [ecx + 0x4af280]
0042d368  add        edi, dword ptr [esi + 0xc]
0042d36b  mov        ebx, dword ptr [edx + 0x4debdc]
0042d371  lea        esi, [ebp + 0x634]
0042d377  call       0x402520

// block 0042d37c
0042d37c  push       edi
0042d37d  push       esi
0042d37e  mov        ecx, ebx
0042d380  mov        byte ptr [esi + 0x49d], 0x10
0042d387  mov        byte ptr [esi + 0x49c], 0x10
0042d38e  call       0x454d10

// block 0042d393
0042d393  jmp        0x42d686

// block 0042d398
0042d398  or         eax, edx
0042d39a  mov        dword ptr [ebp + 0x430], eax
0042d3a0  mov        eax, dword ptr [ebp + 0x198]
0042d3a6  mov        esi, dword ptr [esi + 8]
0042d3a9  mov        dword ptr [esp + 0x14], esi
0042d3ad  test       al, 1
0042d3af  jne        0x42d3dc

// block 0042d3b1
0042d3b1  or         eax, 1
0042d3b4  fstp       dword ptr [ebp + 0x190]
0042d3ba  mov        dword ptr [ebp + 0x18c], ebx
0042d3c0  mov        dword ptr [ebp + 0x188], 0xfff0bdc1
0042d3ca  mov        dword ptr [ebp + 0x194], 0x4b2ed0
0042d3d4  mov        dword ptr [ebp + 0x198], eax
0042d3da  jmp        0x42d3de

// block 0042d3dc
0042d3dc  fstp       st(0)

// block 0042d3de
0042d3de  fild       dword ptr [esp + 0x14]
0042d3e2  mov        dword ptr [ebp + 0x18c], esi
0042d3e8  dec        esi
0042d3e9  mov        dword ptr [ebp + 0x188], esi
0042d3ef  fstp       dword ptr [ebp + 0x190]
0042d3f5  jmp        0x42d686

// block 0042d3fa
0042d3fa  cmp        ecx, 0x80000
0042d400  ja         0x42d59b

// block 0042d406
0042d406  fstp       st(0)
0042d408  je         0x42d490

// block 0042d40e
0042d40e  cmp        ecx, 0x20000
0042d414  ja         0x42d468

// block 0042d416
0042d416  je         0x42d44c

// block 0042d418
0042d418  cmp        ecx, 0x2000
0042d41e  je         0x42d440

// block 0042d420
0042d420  cmp        ecx, 0x4000
0042d426  jne        0x42d686

// block 0042d42c
0042d42c  fld        dword ptr [ebp + 0x50]
0042d42f  mov        eax, dword ptr [esi + 8]
0042d432  push       ecx
0042d433  fstp       dword ptr [esp]
0042d436  call       0x453e20

// block 0042d43b
0042d43b  jmp        0x42d686

// block 0042d440
0042d440  mov        dword ptr [ebp + 0xc], 3
0042d447  jmp        0x42d686

// block 0042d44c
0042d44c  or         eax, edx
0042d44e  mov        dword ptr [ebp + 0x430], eax
0042d454  mov        eax, dword ptr [esi + 8]
0042d457  push       eax
0042d458  lea        eax, [ebp + 0x1bc]
0042d45e  call       0x4067e0

// block 0042d463
0042d463  jmp        0x42d686

// block 0042d468
0042d468  cmp        ecx, 0x40000
0042d46e  jne        0x42d686

// block 0042d474
0042d474  or         eax, edx
0042d476  mov        dword ptr [ebp + 0x430], eax
0042d47c  mov        ecx, dword ptr [esi + 8]
0042d47f  push       ecx
0042d480  lea        eax, [ebp + 0x1bc]
0042d486  call       0x4067e0

// block 0042d48b
0042d48b  jmp        0x42d686

// block 0042d490
0042d490  push       0x214
0042d495  lea        edx, [esp + 0x1c]
0042d499  push       ebx
0042d49a  push       edx
0042d49b  call       0x477420

// block 0042d4a0
0042d4a0  fld        dword ptr [ebp + 0x6c]
0042d4a3  add        esp, 4
0042d4a6  fstp       dword ptr [esp + 4]
0042d4aa  lea        ecx, [esp + 0x24]
0042d4ae  fld        dword ptr [ebp + 0x68]
0042d4b1  mov        dword ptr [esp + 0x228], 0xffffffff
0042d4bc  fstp       dword ptr [esp]
0042d4bf  call       0x42e880

// block 0042d4c4
0042d4c4  fld        dword ptr [ebp + 0x50]
0042d4c7  mov        eax, dword ptr [esi + 8]
0042d4ca  fadd       dword ptr [esp + 0x1c]
0042d4ce  movzx      edx, byte ptr [esi + 0xa]
0042d4d2  mov        ecx, eax
0042d4d4  shr        ecx, 0x18
0042d4d7  fstp       dword ptr [esp + 0x1c]
0042d4db  and        ecx, 0x7f
0042d4de  fld        dword ptr [ebp + 0x54]
0042d4e1  mov        word ptr [esp + 0x214], cx
0042d4e9  fadd       dword ptr [esp + 0x20]
0042d4ed  movzx      cx, byte ptr [esi + 9]
0042d4f2  fstp       dword ptr [esp + 0x20]
0042d4f6  add        esi, 0x18
0042d4f9  fldz       
0042d4fb  mov        word ptr [esp + 0x18], dx
0042d500  movzx      edx, word ptr [esi - 0xc]
0042d504  fstp       dword ptr [esp + 0x24]
0042d508  fld        dword ptr [esi - 0x18]
0042d50b  fstp       dword ptr [esp + 0x30]
0042d50f  fld        dword ptr [esi - 0x14]
0042d512  inc        dword ptr [ebp + 0x42c]
0042d518  fstp       dword ptr [esp + 0x34]
0042d51c  mov        ebx, eax
0042d51e  fld        dword ptr [esi]
0042d520  mov        word ptr [esp + 0x1a], cx
0042d525  mov        ecx, dword ptr [esi + 0xc]
0042d528  fstp       dword ptr [esp + 0x28]
0042d52c  fld        dword ptr [esi + 4]
0042d52f  and        eax, 0xff
0042d534  mov        dword ptr [esp + 0x224], eax
0042d53b  fstp       dword ptr [esp + 0x2c]
0042d53f  mov        ax, word ptr [esi + 8]
0042d543  mov        dword ptr [esp + 0x218], ecx
0042d54a  mov        word ptr [esp + 0x210], dx
0042d552  mov        word ptr [esp + 0x212], ax
0042d55a  lea        esi, [ebp + 0x47c]
0042d560  mov        ecx, 0x6c
0042d565  lea        edi, [esp + 0x3c]

// block 0042d569
0042d569  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0042d56b
0042d56b  lea        esi, [esp + 0x18]
0042d56f  and        ebx, 0x80000000
0042d575  call       0x40b5e0

// block 0042d57a
0042d57a  inc        dword ptr [ebp + 0x42c]
0042d580  test       ebx, ebx
0042d582  je         0x42d68c

// block 0042d588
0042d588  mov        edx, dword ptr [ebp]
0042d58b  mov        eax, dword ptr [edx + 0x14]
0042d58e  push       0
0042d590  push       0
0042d592  mov        ecx, ebp
0042d594  call       eax

// block 0042d596
0042d596  jmp        0x42d686

// block 0042d59b
0042d59b  cmp        ecx, 0x800000
0042d5a1  ja         0x42d652

// block 0042d5a7
0042d5a7  je         0x42d5e2

// block 0042d5a9
0042d5a9  fstp       st(0)
0042d5ab  cmp        ecx, 0x200000
0042d5b1  je         0x42d5cd

// block 0042d5b3
0042d5b3  cmp        ecx, 0x400000
0042d5b9  jne        0x42d686

// block 0042d5bf
0042d5bf  mov        ecx, dword ptr [esi + 8]
0042d5c2  mov        dword ptr [ebp + 0x42c], ecx
0042d5c8  jmp        0x42d68c

// block 0042d5cd
0042d5cd  mov        edx, dword ptr [esi + 8]
0042d5d0  inc        edi
0042d5d1  mov        dword ptr [ebp + 0x80], edx
0042d5d7  mov        dword ptr [ebp + 0x42c], edi
0042d5dd  jmp        0x42d68c

// block 0042d5e2
0042d5e2  or         eax, 0x800000
0042d5e7  mov        dword ptr [ebp + 0x430], eax
0042d5ed  fld        dword ptr [esi]
0042d5ef  fstp       dword ptr [ebp + 0x238]
0042d5f5  fld        dword ptr [esi + 4]
0042d5f8  fstp       dword ptr [ebp + 0x23c]
0042d5fe  mov        eax, dword ptr [ebp + 0x234]
0042d604  test       al, 1
0042d606  jne        0x42d631

// block 0042d608
0042d608  or         eax, 1
0042d60b  fst        dword ptr [ebp + 0x22c]
0042d611  mov        dword ptr [ebp + 0x228], ebx
0042d617  mov        dword ptr [ebp + 0x224], 0xfff0bdc1
0042d621  mov        dword ptr [ebp + 0x230], 0x4b2ed0
0042d62b  mov        dword ptr [ebp + 0x234], eax

// block 0042d631
0042d631  fstp       dword ptr [ebp + 0x22c]
0042d637  mov        dword ptr [ebp + 0x228], ebx
0042d63d  mov        dword ptr [ebp + 0x224], 0xffffffff
0042d647  mov        eax, dword ptr [esi + 8]
0042d64a  mov        dword ptr [ebp + 0x24c], eax
0042d650  jmp        0x42d686

// block 0042d652
0042d652  fstp       st(0)
0042d654  cmp        ecx, 0x10000000
0042d65a  jne        0x42d686

// block 0042d65c
0042d65c  cmp        dword ptr [esi + 8], ebx
0042d65f  je         0x42d67c

// block 0042d661
0042d661  mov        ecx, dword ptr [ebp + 0xab0]
0042d667  and        ecx, 0xffffff3f
0042d66d  or         ecx, 0x20
0042d670  mov        dword ptr [ebp + 0xab0], ecx
0042d676  jmp        0x42d686

// block 0042d678
0042d678  fstp       st(0)
0042d67a  jmp        0x42d686

// block 0042d67c
0042d67c  and        dword ptr [ebp + 0xab0], 0xffffff1f

// block 0042d686
0042d686  inc        dword ptr [ebp + 0x42c]

// block 0042d68c
0042d68c  cmp        dword ptr [ebp + 0x42c], 0x12
0042d693  jl         0x42d050

// block 0042d699
0042d699  pop        edi
0042d69a  pop        esi
0042d69b  pop        ebp
0042d69c  pop        ebx
0042d69d  mov        esp, ebp
0042d69f  pop        ebp
0042d6a0  ret        

// block 0042d6a1
0042d6a1  fstp       st(0)

// block 0042d6a3
0042d6a3  pop        edi
0042d6a4  pop        esi
0042d6a5  pop        ebp
0042d6a6  pop        ebx
0042d6a7  mov        esp, ebp
0042d6a9  pop        ebp
0042d6aa  ret        
