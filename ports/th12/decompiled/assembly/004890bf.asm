
// block 004890bf
004890bf  mov        edi, edi
004890c1  push       ebp
004890c2  mov        ebp, esp
004890c4  sub        esp, 0x2c
004890c7  mov        eax, dword ptr [ebp + 8]
004890ca  movzx      ecx, word ptr [eax + 0xa]
004890ce  push       ebx
004890cf  mov        ebx, ecx
004890d1  and        ecx, 0x8000
004890d7  mov        dword ptr [ebp - 0x14], ecx
004890da  mov        ecx, dword ptr [eax + 6]
004890dd  mov        dword ptr [ebp - 0x20], ecx
004890e0  mov        ecx, dword ptr [eax + 2]
004890e3  movzx      eax, word ptr [eax]
004890e6  and        ebx, 0x7fff
004890ec  sub        ebx, 0x3fff
004890f2  shl        eax, 0x10
004890f5  push       esi
004890f6  push       edi
004890f7  mov        dword ptr [ebp - 0x1c], ecx
004890fa  mov        dword ptr [ebp - 0x18], eax
004890fd  cmp        ebx, 0xffffc001
00489103  jne        0x48911e

// block 00489105
00489105  xor        esi, esi
00489107  xor        eax, eax

// block 00489109
00489109  cmp        dword ptr [ebp + eax*4 - 0x20], esi
0048910d  jne        0x489231

// block 00489113
00489113  inc        eax
00489114  cmp        eax, 3
00489117  jl         0x489109

// block 00489119
00489119  jmp        0x4895a7

// block 0048911e
0048911e  mov        eax, dword ptr [ebp + 0x10]
00489121  and        dword ptr [ebp + 8], 0
00489125  lea        esi, [ebp - 0x20]
00489128  lea        edi, [ebp - 0x2c]
0048912b  movsd      dword ptr es:[edi], dword ptr [esi]
0048912c  movsd      dword ptr es:[edi], dword ptr [esi]
0048912d  movsd      dword ptr es:[edi], dword ptr [esi]
0048912e  mov        esi, dword ptr [eax + 8]
00489131  dec        esi
00489132  lea        ecx, [esi + 1]
00489135  mov        eax, ecx
00489137  cdq        
00489138  and        edx, 0x1f
0048913b  add        eax, edx
0048913d  sar        eax, 5
00489140  mov        edx, ecx
00489142  and        edx, 0x8000001f
00489148  mov        dword ptr [ebp - 0x10], ebx
0048914b  mov        dword ptr [ebp - 0xc], eax
0048914e  jns        0x489155

// block 00489150
00489150  dec        edx
00489151  or         edx, 0xffffffe0
00489154  inc        edx

// block 00489155
00489155  lea        edi, [ebp + eax*4 - 0x20]
00489159  push       0x1f
0048915b  xor        eax, eax
0048915d  pop        ecx
0048915e  sub        ecx, edx
00489160  inc        eax
00489161  shl        eax, cl
00489163  mov        dword ptr [ebp - 8], ecx
00489166  test       dword ptr [edi], eax
00489168  je         0x4891fb

// block 0048916e
0048916e  mov        eax, dword ptr [ebp - 0xc]
00489171  or         edx, 0xffffffff
00489174  shl        edx, cl
00489176  not        edx
00489178  test       dword ptr [ebp + eax*4 - 0x20], edx
0048917c  jmp        0x489183

// block 0048917e
0048917e  cmp        dword ptr [ebp + eax*4 - 0x20], 0

// block 00489183
00489183  jne        0x48918d

// block 00489185
00489185  inc        eax
00489186  cmp        eax, 3
00489189  jl         0x48917e

// block 0048918b
0048918b  jmp        0x4891fb

// block 0048918d
0048918d  mov        eax, esi
0048918f  cdq        
00489190  push       0x1f
00489192  pop        ecx
00489193  and        edx, ecx
00489195  add        eax, edx
00489197  sar        eax, 5
0048919a  and        esi, 0x8000001f
004891a0  jns        0x4891a7

// block 004891a2
004891a2  dec        esi
004891a3  or         esi, 0xffffffe0
004891a6  inc        esi

// block 004891a7
004891a7  and        dword ptr [ebp - 4], 0
004891ab  sub        ecx, esi
004891ad  xor        edx, edx
004891af  inc        edx
004891b0  shl        edx, cl
004891b2  lea        ecx, [ebp + eax*4 - 0x20]
004891b6  mov        esi, dword ptr [ecx]
004891b8  add        esi, edx
004891ba  mov        dword ptr [ebp + 8], esi
004891bd  mov        esi, dword ptr [ecx]
004891bf  cmp        dword ptr [ebp + 8], esi
004891c2  jb         0x4891e6

// block 004891c4
004891c4  cmp        dword ptr [ebp + 8], edx
004891c7  jmp        0x4891e4

// block 004891c9
004891c9  test       ecx, ecx
004891cb  je         0x4891f8

// block 004891cd
004891cd  and        dword ptr [ebp - 4], 0
004891d1  lea        ecx, [ebp + eax*4 - 0x20]
004891d5  mov        edx, dword ptr [ecx]
004891d7  lea        esi, [edx + 1]
004891da  mov        dword ptr [ebp + 8], esi
004891dd  cmp        esi, edx
004891df  jb         0x4891e6

// block 004891e1
004891e1  cmp        esi, 1

// block 004891e4
004891e4  jae        0x4891ed

// block 004891e6
004891e6  mov        dword ptr [ebp - 4], 1

// block 004891ed
004891ed  dec        eax
004891ee  mov        edx, dword ptr [ebp + 8]
004891f1  mov        dword ptr [ecx], edx
004891f3  mov        ecx, dword ptr [ebp - 4]
004891f6  jns        0x4891c9

// block 004891f8
004891f8  mov        dword ptr [ebp + 8], ecx

// block 004891fb
004891fb  mov        ecx, dword ptr [ebp - 8]
004891fe  or         eax, 0xffffffff
00489201  shl        eax, cl
00489203  push       3
00489205  pop        ecx
00489206  and        dword ptr [edi], eax
00489208  mov        eax, dword ptr [ebp - 0xc]
0048920b  inc        eax
0048920c  cmp        eax, ecx
0048920e  jge        0x48921a

// block 00489210
00489210  lea        edi, [ebp + eax*4 - 0x20]
00489214  sub        ecx, eax
00489216  xor        eax, eax

// block 00489218
00489218  rep stosd  dword ptr es:[edi], eax

// block 0048921a
0048921a  xor        esi, esi
0048921c  cmp        dword ptr [ebp + 8], esi
0048921f  je         0x489222

// block 00489221
00489221  inc        ebx

// block 00489222
00489222  mov        ecx, dword ptr [ebp + 0x10]
00489225  mov        eax, dword ptr [ecx + 4]
00489228  mov        edx, eax
0048922a  sub        edx, dword ptr [ecx + 8]
0048922d  cmp        ebx, edx
0048922f  jge        0x48923e

// block 00489231
00489231  xor        eax, eax
00489233  lea        edi, [ebp - 0x20]
00489236  stosd      dword ptr es:[edi], eax
00489237  stosd      dword ptr es:[edi], eax
00489238  stosd      dword ptr es:[edi], eax
00489239  jmp        0x48944d

// block 0048923e
0048923e  cmp        ebx, eax
00489240  jg         0x489455

// block 00489246
00489246  sub        eax, dword ptr [ebp - 0x10]
00489249  lea        esi, [ebp - 0x2c]
0048924c  mov        ecx, eax
0048924e  lea        edi, [ebp - 0x20]
00489251  movsd      dword ptr es:[edi], dword ptr [esi]
00489252  cdq        
00489253  and        edx, 0x1f
00489256  add        eax, edx
00489258  movsd      dword ptr es:[edi], dword ptr [esi]
00489259  mov        edx, ecx
0048925b  sar        eax, 5
0048925e  and        edx, 0x8000001f
00489264  movsd      dword ptr es:[edi], dword ptr [esi]
00489265  jns        0x48926c

// block 00489267
00489267  dec        edx
00489268  or         edx, 0xffffffe0
0048926b  inc        edx

// block 0048926c
0048926c  and        dword ptr [ebp - 0xc], 0
00489270  and        dword ptr [ebp + 8], 0
00489274  or         edi, 0xffffffff
00489277  mov        ecx, edx
00489279  shl        edi, cl
0048927b  mov        dword ptr [ebp - 4], 0x20
00489282  sub        dword ptr [ebp - 4], edx
00489285  not        edi

// block 00489287
00489287  mov        ebx, dword ptr [ebp + 8]
0048928a  lea        ebx, [ebp + ebx*4 - 0x20]
0048928e  mov        esi, dword ptr [ebx]
00489290  mov        ecx, esi
00489292  and        ecx, edi
00489294  mov        dword ptr [ebp - 0x10], ecx
00489297  mov        ecx, edx
00489299  shr        esi, cl
0048929b  mov        ecx, dword ptr [ebp - 4]
0048929e  or         esi, dword ptr [ebp - 0xc]
004892a1  mov        dword ptr [ebx], esi
004892a3  mov        esi, dword ptr [ebp - 0x10]
004892a6  shl        esi, cl
004892a8  inc        dword ptr [ebp + 8]
004892ab  cmp        dword ptr [ebp + 8], 3
004892af  mov        dword ptr [ebp - 0xc], esi
004892b2  jl         0x489287

// block 004892b4
004892b4  mov        esi, eax
004892b6  push       2
004892b8  shl        esi, 2
004892bb  lea        ecx, [ebp - 0x18]
004892be  pop        edx
004892bf  sub        ecx, esi

// block 004892c1
004892c1  cmp        edx, eax
004892c3  jl         0x4892cd

// block 004892c5
004892c5  mov        esi, dword ptr [ecx]
004892c7  mov        dword ptr [ebp + edx*4 - 0x20], esi
004892cb  jmp        0x4892d2

// block 004892cd
004892cd  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 004892d2
004892d2  dec        edx
004892d3  sub        ecx, 4
004892d6  test       edx, edx
004892d8  jge        0x4892c1

// block 004892da
004892da  mov        eax, dword ptr [ebp + 0x10]
004892dd  mov        esi, dword ptr [eax + 8]
004892e0  dec        esi
004892e1  lea        ecx, [esi + 1]
004892e4  mov        eax, ecx
004892e6  cdq        
004892e7  and        edx, 0x1f
004892ea  add        eax, edx
004892ec  sar        eax, 5
004892ef  mov        edx, ecx
004892f1  and        edx, 0x8000001f
004892f7  mov        dword ptr [ebp - 0xc], eax
004892fa  jns        0x489301

// block 004892fc
004892fc  dec        edx
004892fd  or         edx, 0xffffffe0
00489300  inc        edx

// block 00489301
00489301  push       0x1f
00489303  pop        ecx
00489304  sub        ecx, edx
00489306  xor        edx, edx
00489308  inc        edx
00489309  shl        edx, cl
0048930b  lea        ebx, [ebp + eax*4 - 0x20]
0048930f  mov        dword ptr [ebp - 0x10], ecx
00489312  test       dword ptr [ebx], edx
00489314  je         0x48939c

// block 0048931a
0048931a  or         edx, 0xffffffff
0048931d  shl        edx, cl
0048931f  not        edx
00489321  test       dword ptr [ebp + eax*4 - 0x20], edx
00489325  jmp        0x48932c

// block 00489327
00489327  cmp        dword ptr [ebp + eax*4 - 0x20], 0

// block 0048932c
0048932c  jne        0x489336

// block 0048932e
0048932e  inc        eax
0048932f  cmp        eax, 3
00489332  jl         0x489327

// block 00489334
00489334  jmp        0x48939c

// block 00489336
00489336  mov        eax, esi
00489338  cdq        
00489339  push       0x1f
0048933b  pop        ecx
0048933c  and        edx, ecx
0048933e  add        eax, edx
00489340  sar        eax, 5
00489343  and        esi, 0x8000001f
00489349  jns        0x489350

// block 0048934b
0048934b  dec        esi
0048934c  or         esi, 0xffffffe0
0048934f  inc        esi

// block 00489350
00489350  and        dword ptr [ebp + 8], 0
00489354  xor        edx, edx
00489356  sub        ecx, esi
00489358  inc        edx
00489359  shl        edx, cl
0048935b  lea        ecx, [ebp + eax*4 - 0x20]
0048935f  mov        esi, dword ptr [ecx]
00489361  lea        edi, [esi + edx]
00489364  cmp        edi, esi
00489366  jb         0x48936c

// block 00489368
00489368  cmp        edi, edx
0048936a  jae        0x489373

// block 0048936c
0048936c  mov        dword ptr [ebp + 8], 1

// block 00489373
00489373  mov        dword ptr [ecx], edi
00489375  mov        ecx, dword ptr [ebp + 8]
00489378  jmp        0x489399

// block 0048937a
0048937a  test       ecx, ecx
0048937c  je         0x48939c

// block 0048937e
0048937e  lea        ecx, [ebp + eax*4 - 0x20]
00489382  mov        edx, dword ptr [ecx]
00489384  lea        esi, [edx + 1]
00489387  xor        edi, edi
00489389  cmp        esi, edx
0048938b  jb         0x489392

// block 0048938d
0048938d  cmp        esi, 1
00489390  jae        0x489395

// block 00489392
00489392  xor        edi, edi
00489394  inc        edi

// block 00489395
00489395  mov        dword ptr [ecx], esi
00489397  mov        ecx, edi

// block 00489399
00489399  dec        eax
0048939a  jns        0x48937a

// block 0048939c
0048939c  mov        ecx, dword ptr [ebp - 0x10]
0048939f  or         eax, 0xffffffff
004893a2  shl        eax, cl
004893a4  and        dword ptr [ebx], eax
004893a6  mov        eax, dword ptr [ebp - 0xc]
004893a9  inc        eax
004893aa  cmp        eax, 3
004893ad  jge        0x4893bc

// block 004893af
004893af  push       3
004893b1  pop        ecx
004893b2  lea        edi, [ebp + eax*4 - 0x20]
004893b6  sub        ecx, eax
004893b8  xor        eax, eax

// block 004893ba
004893ba  rep stosd  dword ptr es:[edi], eax

// block 004893bc
004893bc  mov        eax, dword ptr [ebp + 0x10]
004893bf  mov        ecx, dword ptr [eax + 0xc]
004893c2  inc        ecx
004893c3  mov        eax, ecx
004893c5  cdq        
004893c6  and        edx, 0x1f
004893c9  add        eax, edx
004893cb  mov        edx, ecx
004893cd  sar        eax, 5
004893d0  and        edx, 0x8000001f
004893d6  jns        0x4893dd

// block 004893d8
004893d8  dec        edx
004893d9  or         edx, 0xffffffe0
004893dc  inc        edx

// block 004893dd
004893dd  and        dword ptr [ebp - 0xc], 0
004893e1  and        dword ptr [ebp + 8], 0
004893e5  or         edi, 0xffffffff
004893e8  mov        ecx, edx
004893ea  shl        edi, cl
004893ec  mov        dword ptr [ebp - 4], 0x20
004893f3  sub        dword ptr [ebp - 4], edx
004893f6  not        edi

// block 004893f8
004893f8  mov        ebx, dword ptr [ebp + 8]
004893fb  lea        ebx, [ebp + ebx*4 - 0x20]
004893ff  mov        esi, dword ptr [ebx]
00489401  mov        ecx, esi
00489403  and        ecx, edi
00489405  mov        dword ptr [ebp - 0x10], ecx
00489408  mov        ecx, edx
0048940a  shr        esi, cl
0048940c  mov        ecx, dword ptr [ebp - 4]
0048940f  or         esi, dword ptr [ebp - 0xc]
00489412  mov        dword ptr [ebx], esi
00489414  mov        esi, dword ptr [ebp - 0x10]
00489417  shl        esi, cl
00489419  inc        dword ptr [ebp + 8]
0048941c  cmp        dword ptr [ebp + 8], 3
00489420  mov        dword ptr [ebp - 0xc], esi
00489423  jl         0x4893f8

// block 00489425
00489425  mov        esi, eax
00489427  push       2
00489429  shl        esi, 2
0048942c  lea        ecx, [ebp - 0x18]
0048942f  pop        edx
00489430  sub        ecx, esi

// block 00489432
00489432  cmp        edx, eax
00489434  jl         0x48943e

// block 00489436
00489436  mov        esi, dword ptr [ecx]
00489438  mov        dword ptr [ebp + edx*4 - 0x20], esi
0048943c  jmp        0x489443

// block 0048943e
0048943e  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 00489443
00489443  dec        edx
00489444  sub        ecx, 4
00489447  test       edx, edx
00489449  jge        0x489432

// block 0048944b
0048944b  xor        esi, esi

// block 0048944d
0048944d  push       2
0048944f  pop        eax
00489450  jmp        0x4895a9

// block 00489455
00489455  cmp        ebx, dword ptr [ecx]
00489457  jl         0x489509

// block 0048945d
0048945d  xor        eax, eax
0048945f  lea        edi, [ebp - 0x20]
00489462  stosd      dword ptr es:[edi], eax
00489463  stosd      dword ptr es:[edi], eax
00489464  stosd      dword ptr es:[edi], eax
00489465  or         dword ptr [ebp - 0x20], 0x80000000
0048946c  mov        eax, ecx
0048946e  mov        ecx, dword ptr [eax + 0xc]
00489471  mov        eax, ecx
00489473  cdq        
00489474  and        edx, 0x1f
00489477  add        eax, edx
00489479  mov        edx, ecx
0048947b  sar        eax, 5
0048947e  and        edx, 0x8000001f
00489484  jns        0x48948b

// block 00489486
00489486  dec        edx
00489487  or         edx, 0xffffffe0
0048948a  inc        edx

// block 0048948b
0048948b  and        dword ptr [ebp - 0xc], 0
0048948f  and        dword ptr [ebp + 8], 0
00489493  or         esi, 0xffffffff
00489496  mov        ecx, edx
00489498  shl        esi, cl
0048949a  mov        dword ptr [ebp - 4], 0x20
004894a1  sub        dword ptr [ebp - 4], edx
004894a4  not        esi

// block 004894a6
004894a6  mov        ebx, dword ptr [ebp + 8]
004894a9  lea        ebx, [ebp + ebx*4 - 0x20]
004894ad  mov        edi, dword ptr [ebx]
004894af  mov        ecx, edi
004894b1  and        ecx, esi
004894b3  mov        dword ptr [ebp - 0x10], ecx
004894b6  mov        ecx, edx
004894b8  shr        edi, cl
004894ba  mov        ecx, dword ptr [ebp - 4]
004894bd  or         edi, dword ptr [ebp - 0xc]
004894c0  mov        dword ptr [ebx], edi
004894c2  mov        edi, dword ptr [ebp - 0x10]
004894c5  shl        edi, cl
004894c7  inc        dword ptr [ebp + 8]
004894ca  cmp        dword ptr [ebp + 8], 3
004894ce  mov        dword ptr [ebp - 0xc], edi
004894d1  jl         0x4894a6

// block 004894d3
004894d3  mov        esi, eax
004894d5  push       2
004894d7  shl        esi, 2
004894da  lea        ecx, [ebp - 0x18]
004894dd  pop        edx
004894de  sub        ecx, esi

// block 004894e0
004894e0  cmp        edx, eax
004894e2  jl         0x4894ec

// block 004894e4
004894e4  mov        esi, dword ptr [ecx]
004894e6  mov        dword ptr [ebp + edx*4 - 0x20], esi
004894ea  jmp        0x4894f1

// block 004894ec
004894ec  and        dword ptr [ebp + edx*4 - 0x20], 0

// block 004894f1
004894f1  dec        edx
004894f2  sub        ecx, 4
004894f5  test       edx, edx
004894f7  jge        0x4894e0

// block 004894f9
004894f9  mov        eax, dword ptr [ebp + 0x10]
004894fc  mov        esi, dword ptr [eax + 0x14]
004894ff  add        esi, dword ptr [eax]
00489501  xor        eax, eax
00489503  inc        eax
00489504  jmp        0x4895a9

// block 00489509
00489509  and        dword ptr [ebp - 0x20], 0x7fffffff
00489510  mov        eax, ecx
00489512  mov        ecx, dword ptr [eax + 0xc]
00489515  mov        esi, dword ptr [eax + 0x14]
00489518  mov        eax, ecx
0048951a  cdq        
0048951b  and        edx, 0x1f
0048951e  add        eax, edx
00489520  add        esi, ebx
00489522  mov        ebx, eax
00489524  mov        edx, ecx
00489526  sar        ebx, 5
00489529  and        edx, 0x8000001f
0048952f  jns        0x489536

// block 00489531
00489531  dec        edx
00489532  or         edx, 0xffffffe0
00489535  inc        edx

// block 00489536
00489536  and        dword ptr [ebp - 0xc], 0
0048953a  and        dword ptr [ebp + 8], 0
0048953e  or         edi, 0xffffffff
00489541  mov        ecx, edx
00489543  shl        edi, cl
00489545  mov        dword ptr [ebp - 4], 0x20
0048954c  sub        dword ptr [ebp - 4], edx
0048954f  not        edi

// block 00489551
00489551  mov        eax, dword ptr [ebp + 8]
00489554  mov        eax, dword ptr [ebp + eax*4 - 0x20]
00489558  mov        ecx, eax
0048955a  and        ecx, edi
0048955c  mov        dword ptr [ebp - 0x10], ecx
0048955f  mov        ecx, edx
00489561  shr        eax, cl
00489563  mov        ecx, dword ptr [ebp + 8]
00489566  or         eax, dword ptr [ebp - 0xc]
00489569  mov        dword ptr [ebp + ecx*4 - 0x20], eax
0048956d  mov        eax, dword ptr [ebp - 0x10]
00489570  mov        ecx, dword ptr [ebp - 4]
00489573  shl        eax, cl
00489575  inc        dword ptr [ebp + 8]
00489578  cmp        dword ptr [ebp + 8], 3
0048957c  mov        dword ptr [ebp - 0xc], eax
0048957f  jl         0x489551

// block 00489581
00489581  mov        edx, ebx
00489583  push       2
00489585  shl        edx, 2
00489588  lea        eax, [ebp - 0x18]
0048958b  pop        ecx
0048958c  sub        eax, edx

// block 0048958e
0048958e  cmp        ecx, ebx
00489590  jl         0x48959a

// block 00489592
00489592  mov        edx, dword ptr [eax]
00489594  mov        dword ptr [ebp + ecx*4 - 0x20], edx
00489598  jmp        0x48959f

// block 0048959a
0048959a  and        dword ptr [ebp + ecx*4 - 0x20], 0

// block 0048959f
0048959f  dec        ecx
004895a0  sub        eax, 4
004895a3  test       ecx, ecx
004895a5  jge        0x48958e

// block 004895a7
004895a7  xor        eax, eax

// block 004895a9
004895a9  mov        edx, dword ptr [ebp + 0x10]
004895ac  push       0x1f
004895ae  pop        ecx
004895af  sub        ecx, dword ptr [edx + 0xc]
004895b2  shl        esi, cl
004895b4  mov        ecx, dword ptr [ebp - 0x14]
004895b7  neg        ecx
004895b9  sbb        ecx, ecx
004895bb  and        ecx, 0x80000000
004895c1  or         esi, ecx
004895c3  mov        ecx, dword ptr [edx + 0x10]
004895c6  or         esi, dword ptr [ebp - 0x20]
004895c9  cmp        ecx, 0x40
004895cc  jne        0x4895db

// block 004895ce
004895ce  mov        ecx, dword ptr [ebp + 0xc]
004895d1  mov        edx, dword ptr [ebp - 0x1c]
004895d4  mov        dword ptr [ecx + 4], esi
004895d7  mov        dword ptr [ecx], edx
004895d9  jmp        0x4895e5

// block 004895db
004895db  cmp        ecx, 0x20
004895de  jne        0x4895e5

// block 004895e0
004895e0  mov        ecx, dword ptr [ebp + 0xc]
004895e3  mov        dword ptr [ecx], esi

// block 004895e5
004895e5  pop        edi
004895e6  pop        esi
004895e7  pop        ebx
004895e8  leave      
004895e9  ret        
