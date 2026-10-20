
// block 00420f90
00420f90  sub        esp, 0x14
00420f93  push       ebx
00420f94  push       ebp
00420f95  push       edi
00420f96  jmp        dword ptr [eax*4 + 0x421334]

// block 00420f9d
00420f9d  mov        ecx, dword ptr [esi + 0x6c68]
00420fa3  push       ecx
00420fa4  call       0x461a70

// block 00420fa9
00420fa9  xor        ebp, ebp
00420fab  push       ebp
00420fac  push       0x2b
00420fae  lea        edx, [esp + 0x1c]
00420fb2  mov        dword ptr [esi + 0x6c68], ebp
00420fb8  mov        eax, dword ptr [esi + 0x6d44]
00420fbe  push       edx
00420fbf  push       eax
00420fc0  lea        eax, [ebp + 0x17]
00420fc3  xor        ecx, ecx
00420fc5  call       0x4615a0

// block 00420fca
00420fca  mov        ecx, dword ptr [esp + 0x14]
00420fce  mov        edx, dword ptr [esp + 0x24]
00420fd2  mov        dword ptr [esi + 0x6c68], ecx
00420fd8  mov        dword ptr [esp + 0x14], edx
00420fdc  mov        ebx, 0x989680
00420fe1  mov        dword ptr [esp + 0x10], ebp
00420fe5  lea        edi, [esi + 0x6c40]
00420feb  jmp        0x420ff0

// block 00420ff0
00420ff0  mov        eax, dword ptr [edi]
00420ff2  push       eax
00420ff3  call       0x461a70

// block 00420ff8
00420ff8  mov        eax, dword ptr [0x4b43b8]
00420ffd  push       0
00420fff  lea        ecx, [ebp + 5]
00421002  push       ecx
00421003  lea        edx, [esp + 0x20]
00421007  mov        dword ptr [edi], 0
0042100d  mov        ecx, dword ptr [eax + 0x18fb4]
00421013  push       edx
00421014  push       ecx
00421015  mov        eax, 0x17
0042101a  xor        ecx, ecx
0042101c  call       0x4615a0

// block 00421021
00421021  mov        eax, dword ptr [esp + 0x14]
00421025  cdq        
00421026  idiv       ebx
00421028  mov        ecx, dword ptr [esp + 0x18]
0042102c  mov        dword ptr [edi], ecx
0042102e  mov        dword ptr [esp + 0x1c], edx
00421032  mov        dword ptr [esp + 0x14], eax
00421036  test       eax, eax
00421038  je         0x421042

// block 0042103a
0042103a  mov        dword ptr [esp + 0x10], 1

// block 00421042
00421042  mov        edx, dword ptr [0x4ce8cc]
00421048  push       ecx
00421049  call       0x461920

// block 0042104e
0042104e  test       eax, eax
00421050  je         0x421067

// block 00421052
00421052  mov        ecx, dword ptr [esp + 0x14]
00421056  mov        edx, dword ptr [eax + 0x3f8]
0042105c  add        ecx, 0xf3
00421062  call       0x454b80

// block 00421067
00421067  cmp        dword ptr [esp + 0x10], 0
0042106c  mov        eax, edi
0042106e  jne        0x421077

// block 00421070
00421070  call       0x461d80

// block 00421075
00421075  jmp        0x42107c

// block 00421077
00421077  call       0x461d40

// block 0042107c
0042107c  mov        edx, dword ptr [esp + 0x1c]
00421080  mov        dword ptr [esp + 0x14], edx
00421084  mov        eax, 0x66666667
00421089  imul       ebx
0042108b  sar        edx, 2
0042108e  mov        eax, edx
00421090  shr        eax, 0x1f
00421093  add        eax, edx
00421095  inc        ebp
00421096  add        edi, 4
00421099  cmp        ebp, 8
0042109c  mov        ebx, eax
0042109e  jl         0x420ff0

// block 004210a4
004210a4  mov        ecx, dword ptr [esi + 0x6c60]
004210aa  push       ecx
004210ab  call       0x461a70

// block 004210b0
004210b0  mov        edi, dword ptr [esp + 0x24]
004210b4  xor        ebx, ebx
004210b6  cmp        edi, 0xf4240
004210bc  mov        dword ptr [esi + 0x6c60], ebx
004210c2  jl         0x42110c

// block 004210c4
004210c4  mov        eax, dword ptr [0x4b43b8]
004210c9  mov        ecx, dword ptr [eax + 0x18fb4]
004210cf  push       ebx
004210d0  push       0xd
004210d2  lea        edx, [esp + 0x2c]
004210d6  push       edx
004210d7  push       ecx
004210d8  lea        eax, [ebx + 0x17]
004210db  xor        ecx, ecx
004210dd  call       0x4615a0

// block 004210e2
004210e2  mov        eax, dword ptr [esp + 0x24]
004210e6  mov        edx, dword ptr [0x4ce8cc]
004210ec  push       eax
004210ed  mov        dword ptr [esi + 0x6c60], eax
004210f3  call       0x461920

// block 004210f8
004210f8  cmp        eax, ebx
004210fa  je         0x42110c

// block 004210fc
004210fc  mov        edx, dword ptr [eax + 0x3f8]
00421102  mov        ecx, 0x101
00421107  call       0x454b80

// block 0042110c
0042110c  mov        edx, dword ptr [esi + 0x6c64]
00421112  push       edx
00421113  call       0x461a70

// block 00421118
00421118  cmp        edi, 0x3e8
0042111e  mov        dword ptr [esi + 0x6c64], ebx
00421124  jl         0x421171

// block 00421126
00421126  mov        ecx, dword ptr [0x4b43b8]
0042112c  mov        edx, dword ptr [ecx + 0x18fb4]
00421132  push       ebx
00421133  push       0xe
00421135  lea        eax, [esp + 0x2c]
00421139  push       eax
0042113a  push       edx
0042113b  mov        eax, 0x17
00421140  xor        ecx, ecx
00421142  call       0x4615a0

// block 00421147
00421147  mov        eax, dword ptr [esp + 0x24]
0042114b  mov        edx, dword ptr [0x4ce8cc]
00421151  push       eax
00421152  mov        dword ptr [esi + 0x6c64], eax
00421158  call       0x461920

// block 0042115d
0042115d  cmp        eax, ebx
0042115f  je         0x421171

// block 00421161
00421161  mov        edx, dword ptr [eax + 0x3f8]
00421167  mov        ecx, 0x101
0042116c  call       0x454b80

// block 00421171
00421171  mov        ecx, dword ptr [esi + 0x6d44]
00421177  push       ebx
00421178  push       0x4e
0042117a  lea        eax, [esp + 0x2c]
0042117e  push       eax
0042117f  push       ecx
00421180  mov        eax, 0x17
00421185  xor        ecx, ecx
00421187  mov        dword ptr [esi + 0x6cb8], 1
00421191  call       0x4615a0

// block 00421196
00421196  mov        edx, dword ptr [esp + 0x24]
0042119a  mov        dword ptr [esi + 0x6cb4], edx
004211a0  pop        edi
004211a1  pop        ebp
004211a2  pop        ebx
004211a3  add        esp, 0x14
004211a6  ret        4

// block 004211a9
004211a9  mov        eax, dword ptr [esi + 0x6c68]
004211af  push       eax
004211b0  call       0x461a70

// block 004211b5
004211b5  push       0
004211b7  push       0x2c
004211b9  lea        ecx, [esp + 0x2c]
004211bd  mov        dword ptr [esi + 0x6c68], 0
004211c7  mov        edx, dword ptr [esi + 0x6d44]
004211cd  push       ecx
004211ce  push       edx
004211cf  mov        eax, 0x17
004211d4  xor        ecx, ecx
004211d6  call       0x4615a0

// block 004211db
004211db  mov        eax, dword ptr [esp + 0x24]
004211df  mov        edx, dword ptr [esi + 0x6d44]
004211e5  push       0
004211e7  push       0x4e
004211e9  lea        ecx, [esp + 0x2c]
004211ed  push       ecx
004211ee  mov        dword ptr [esi + 0x6c68], eax
004211f4  push       edx
004211f5  mov        eax, 0x17
004211fa  xor        ecx, ecx
004211fc  mov        dword ptr [esi + 0x6cb8], 1
00421206  call       0x4615a0

// block 0042120b
0042120b  mov        eax, dword ptr [esp + 0x24]
0042120f  mov        dword ptr [esi + 0x6cb4], eax
00421215  pop        edi
00421216  pop        ebp
00421217  pop        ebx
00421218  add        esp, 0x14
0042121b  ret        4

// block 0042121e
0042121e  mov        ecx, dword ptr [esi + 0x6c6c]
00421224  push       ecx
00421225  call       0x461a70

// block 0042122a
0042122a  push       0
0042122c  push       0x2d
0042122e  lea        edx, [esp + 0x2c]
00421232  mov        dword ptr [esi + 0x6c6c], 0
0042123c  mov        eax, dword ptr [esi + 0x6d44]
00421242  push       edx
00421243  push       eax
00421244  mov        eax, 0x17
00421249  xor        ecx, ecx
0042124b  call       0x4615a0

// block 00421250
00421250  mov        ecx, dword ptr [esp + 0x24]
00421254  mov        dword ptr [esi + 0x6c6c], ecx
0042125a  pop        edi
0042125b  pop        ebp
0042125c  pop        ebx
0042125d  add        esp, 0x14
00421260  ret        4

// block 00421263
00421263  mov        edx, dword ptr [esi + 0x6c6c]
00421269  push       edx
0042126a  call       0x461a70

// block 0042126f
0042126f  push       0
00421271  push       0x2e
00421273  lea        eax, [esp + 0x2c]
00421277  mov        dword ptr [esi + 0x6c6c], 0
00421281  mov        ecx, dword ptr [esi + 0x6d44]
00421287  push       eax
00421288  push       ecx
00421289  mov        eax, 0x17
0042128e  xor        ecx, ecx
00421290  call       0x4615a0

// block 00421295
00421295  mov        edx, dword ptr [esp + 0x24]
00421299  mov        dword ptr [esi + 0x6c6c], edx
0042129f  pop        edi
004212a0  pop        ebp
004212a1  pop        ebx
004212a2  add        esp, 0x14
004212a5  ret        4

// block 004212a8
004212a8  mov        eax, dword ptr [esi + 0x6c6c]
004212ae  push       eax
004212af  call       0x461a70

// block 004212b4
004212b4  push       0
004212b6  push       0x2f
004212b8  lea        ecx, [esp + 0x2c]
004212bc  mov        dword ptr [esi + 0x6c6c], 0
004212c6  mov        edx, dword ptr [esi + 0x6d44]
004212cc  push       ecx
004212cd  push       edx
004212ce  mov        eax, 0x17
004212d3  xor        ecx, ecx
004212d5  call       0x4615a0

// block 004212da
004212da  mov        eax, dword ptr [esp + 0x24]
004212de  mov        dword ptr [esi + 0x6c6c], eax
004212e4  pop        edi
004212e5  pop        ebp
004212e6  pop        ebx
004212e7  add        esp, 0x14
004212ea  ret        4

// block 004212ed
004212ed  mov        ecx, dword ptr [esi + 0x6c68]
004212f3  push       ecx
004212f4  call       0x461a70

// block 004212f9
004212f9  push       0
004212fb  push       0x30
004212fd  lea        edx, [esp + 0x2c]
00421301  mov        dword ptr [esi + 0x6c68], 0
0042130b  mov        eax, dword ptr [esi + 0x6d44]
00421311  push       edx
00421312  push       eax
00421313  mov        eax, 0x17
00421318  xor        ecx, ecx
0042131a  call       0x4615a0

// block 0042131f
0042131f  mov        ecx, dword ptr [esp + 0x24]
00421323  mov        dword ptr [esi + 0x6c68], ecx

// block 00421329
00421329  pop        edi
0042132a  pop        ebp
0042132b  pop        ebx
0042132c  add        esp, 0x14
0042132f  ret        4
