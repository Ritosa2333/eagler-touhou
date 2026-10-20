
// block 0049616f
0049616f  mov        edi, edi
00496171  push       ebp
00496172  mov        ebp, esp
00496174  mov        eax, dword ptr [ebp + 8]
00496177  mov        cl, byte ptr [ebp + 0x10]
0049617a  push       ebx
0049617b  push       esi
0049617c  push       edi
0049617d  xor        edi, edi
0049617f  mov        dword ptr [eax + 4], edi
00496182  mov        eax, dword ptr [ebp + 8]
00496185  xor        ebx, ebx
00496187  mov        dword ptr [eax + 8], edi
0049618a  mov        eax, dword ptr [ebp + 8]
0049618d  inc        ebx
0049618e  mov        dword ptr [eax + 0xc], edi
00496191  test       cl, 0x10
00496194  je         0x4961a3

// block 00496196
00496196  mov        eax, dword ptr [ebp + 8]
00496199  or         dword ptr [eax + 4], ebx
0049619c  mov        dword ptr [ebp + 0x10], 0xc000008f

// block 004961a3
004961a3  test       cl, 2
004961a6  je         0x4961b6

// block 004961a8
004961a8  mov        eax, dword ptr [ebp + 8]
004961ab  or         dword ptr [eax + 4], 2
004961af  mov        dword ptr [ebp + 0x10], 0xc0000093

// block 004961b6
004961b6  test       bl, cl
004961b8  je         0x4961c8

// block 004961ba
004961ba  mov        eax, dword ptr [ebp + 8]
004961bd  or         dword ptr [eax + 4], 4
004961c1  mov        dword ptr [ebp + 0x10], 0xc0000091

// block 004961c8
004961c8  test       cl, 4
004961cb  je         0x4961db

// block 004961cd
004961cd  mov        eax, dword ptr [ebp + 8]
004961d0  or         dword ptr [eax + 4], 8
004961d4  mov        dword ptr [ebp + 0x10], 0xc000008e

// block 004961db
004961db  test       cl, 8
004961de  je         0x4961ee

// block 004961e0
004961e0  mov        eax, dword ptr [ebp + 8]
004961e3  or         dword ptr [eax + 4], 0x10
004961e7  mov        dword ptr [ebp + 0x10], 0xc0000090

// block 004961ee
004961ee  mov        esi, dword ptr [ebp + 0xc]
004961f1  mov        ecx, dword ptr [esi]
004961f3  mov        eax, dword ptr [ebp + 8]
004961f6  shl        ecx, 4
004961f9  not        ecx
004961fb  xor        ecx, dword ptr [eax + 8]
004961fe  and        ecx, 0x10
00496201  xor        dword ptr [eax + 8], ecx
00496204  mov        ecx, dword ptr [esi]
00496206  mov        eax, dword ptr [ebp + 8]
00496209  add        ecx, ecx
0049620b  not        ecx
0049620d  xor        ecx, dword ptr [eax + 8]
00496210  and        ecx, 8
00496213  xor        dword ptr [eax + 8], ecx
00496216  mov        ecx, dword ptr [esi]
00496218  mov        eax, dword ptr [ebp + 8]
0049621b  shr        ecx, 1
0049621d  not        ecx
0049621f  xor        ecx, dword ptr [eax + 8]
00496222  and        ecx, 4
00496225  xor        dword ptr [eax + 8], ecx
00496228  mov        ecx, dword ptr [esi]
0049622a  mov        eax, dword ptr [ebp + 8]
0049622d  shr        ecx, 3
00496230  not        ecx
00496232  xor        ecx, dword ptr [eax + 8]
00496235  and        ecx, 2
00496238  xor        dword ptr [eax + 8], ecx
0049623b  mov        ecx, dword ptr [esi]
0049623d  mov        eax, dword ptr [ebp + 8]
00496240  shr        ecx, 5
00496243  not        ecx
00496245  xor        ecx, dword ptr [eax + 8]
00496248  and        ecx, ebx
0049624a  xor        dword ptr [eax + 8], ecx
0049624d  call       0x491160

// block 00496252
00496252  test       bl, al
00496254  je         0x49625d

// block 00496256
00496256  mov        ecx, dword ptr [ebp + 8]
00496259  or         dword ptr [ecx + 0xc], 0x10

// block 0049625d
0049625d  test       al, 4
0049625f  je         0x496268

// block 00496261
00496261  mov        ecx, dword ptr [ebp + 8]
00496264  or         dword ptr [ecx + 0xc], 8

// block 00496268
00496268  test       al, 8
0049626a  je         0x496273

// block 0049626c
0049626c  mov        ecx, dword ptr [ebp + 8]
0049626f  or         dword ptr [ecx + 0xc], 4

// block 00496273
00496273  test       al, 0x10
00496275  je         0x49627e

// block 00496277
00496277  mov        ecx, dword ptr [ebp + 8]
0049627a  or         dword ptr [ecx + 0xc], 2

// block 0049627e
0049627e  test       al, 0x20
00496280  je         0x496288

// block 00496282
00496282  mov        eax, dword ptr [ebp + 8]
00496285  or         dword ptr [eax + 0xc], ebx

// block 00496288
00496288  mov        eax, dword ptr [esi]
0049628a  mov        ecx, 0xc00
0049628f  and        eax, ecx
00496291  je         0x4962c8

// block 00496293
00496293  cmp        eax, 0x400
00496298  je         0x4962bc

// block 0049629a
0049629a  cmp        eax, 0x800
0049629f  je         0x4962ad

// block 004962a1
004962a1  cmp        eax, ecx
004962a3  jne        0x4962ce

// block 004962a5
004962a5  mov        eax, dword ptr [ebp + 8]
004962a8  or         dword ptr [eax], 3
004962ab  jmp        0x4962ce

// block 004962ad
004962ad  mov        eax, dword ptr [ebp + 8]
004962b0  mov        ecx, dword ptr [eax]
004962b2  and        ecx, 0xfffffffe
004962b5  or         ecx, 2

// block 004962b8
004962b8  mov        dword ptr [eax], ecx
004962ba  jmp        0x4962ce

// block 004962bc
004962bc  mov        eax, dword ptr [ebp + 8]
004962bf  mov        ecx, dword ptr [eax]
004962c1  and        ecx, 0xfffffffd
004962c4  or         ecx, ebx
004962c6  jmp        0x4962b8

// block 004962c8
004962c8  mov        eax, dword ptr [ebp + 8]
004962cb  and        dword ptr [eax], 0xfffffffc

// block 004962ce
004962ce  mov        eax, dword ptr [esi]
004962d0  mov        ecx, 0x300
004962d5  and        eax, ecx
004962d7  je         0x4962f9

// block 004962d9
004962d9  cmp        eax, 0x200
004962de  je         0x4962ec

// block 004962e0
004962e0  cmp        eax, ecx
004962e2  jne        0x496306

// block 004962e4
004962e4  mov        eax, dword ptr [ebp + 8]
004962e7  and        dword ptr [eax], 0xffffffe3
004962ea  jmp        0x496306

// block 004962ec
004962ec  mov        eax, dword ptr [ebp + 8]
004962ef  mov        ecx, dword ptr [eax]
004962f1  and        ecx, 0xffffffe7
004962f4  or         ecx, 4
004962f7  jmp        0x496304

// block 004962f9
004962f9  mov        eax, dword ptr [ebp + 8]
004962fc  mov        ecx, dword ptr [eax]
004962fe  and        ecx, 0xffffffeb
00496301  or         ecx, 8

// block 00496304
00496304  mov        dword ptr [eax], ecx

// block 00496306
00496306  mov        eax, dword ptr [ebp + 8]
00496309  mov        ecx, dword ptr [ebp + 0x14]
0049630c  shl        ecx, 5
0049630f  xor        ecx, dword ptr [eax]
00496311  and        ecx, 0x1ffe0
00496317  xor        dword ptr [eax], ecx
00496319  mov        eax, dword ptr [ebp + 8]
0049631c  or         dword ptr [eax + 0x20], ebx
0049631f  cmp        dword ptr [ebp + 0x20], edi
00496322  mov        eax, dword ptr [ebp + 8]
00496325  mov        edi, dword ptr [ebp + 0x1c]
00496328  je         0x496350

// block 0049632a
0049632a  and        dword ptr [eax + 0x20], 0xffffffe1
0049632e  mov        eax, dword ptr [ebp + 0x18]
00496331  fld        dword ptr [eax]
00496333  mov        eax, dword ptr [ebp + 8]
00496336  fstp       dword ptr [eax + 0x10]
00496339  mov        eax, dword ptr [ebp + 8]
0049633c  or         dword ptr [eax + 0x60], ebx
0049633f  mov        eax, dword ptr [ebp + 8]
00496342  and        dword ptr [eax + 0x60], 0xffffffe1
00496346  fld        dword ptr [edi]
00496348  mov        eax, dword ptr [ebp + 8]
0049634b  fstp       dword ptr [eax + 0x50]
0049634e  jmp        0x496384

// block 00496350
00496350  mov        ecx, dword ptr [eax + 0x20]
00496353  and        ecx, 0xffffffe3
00496356  or         ecx, 2
00496359  mov        dword ptr [eax + 0x20], ecx
0049635c  mov        eax, dword ptr [ebp + 0x18]
0049635f  fld        qword ptr [eax]
00496361  mov        eax, dword ptr [ebp + 8]
00496364  fstp       qword ptr [eax + 0x10]
00496367  mov        eax, dword ptr [ebp + 8]
0049636a  or         dword ptr [eax + 0x60], ebx
0049636d  mov        eax, dword ptr [ebp + 8]
00496370  mov        ecx, dword ptr [eax + 0x60]
00496373  and        ecx, 0xffffffe3
00496376  or         ecx, 2
00496379  mov        dword ptr [eax + 0x60], ecx
0049637c  fld        qword ptr [edi]
0049637e  mov        eax, dword ptr [ebp + 8]
00496381  fstp       qword ptr [eax + 0x50]

// block 00496384
00496384  call       0x491170

// block 00496389
00496389  lea        eax, [ebp + 8]
0049638c  push       eax
0049638d  push       ebx
0049638e  push       0
00496390  push       dword ptr [ebp + 0x10]
00496393  call       dword ptr [0x498178]

// block 00496399
00496399  mov        ecx, dword ptr [ebp + 8]
0049639c  test       byte ptr [ecx + 8], 0x10
004963a0  je         0x4963a5

// block 004963a2
004963a2  and        dword ptr [esi], 0xfffffffe

// block 004963a5
004963a5  test       byte ptr [ecx + 8], 8
004963a9  je         0x4963ae

// block 004963ab
004963ab  and        dword ptr [esi], 0xfffffffb

// block 004963ae
004963ae  test       byte ptr [ecx + 8], 4
004963b2  je         0x4963b7

// block 004963b4
004963b4  and        dword ptr [esi], 0xfffffff7

// block 004963b7
004963b7  test       byte ptr [ecx + 8], 2
004963bb  je         0x4963c0

// block 004963bd
004963bd  and        dword ptr [esi], 0xffffffef

// block 004963c0
004963c0  test       byte ptr [ecx + 8], bl
004963c3  je         0x4963c8

// block 004963c5
004963c5  and        dword ptr [esi], 0xffffffdf

// block 004963c8
004963c8  mov        eax, dword ptr [ecx]
004963ca  and        eax, 3
004963cd  xor        ebx, ebx
004963cf  sub        eax, ebx
004963d1  mov        edx, 0xfffff3ff
004963d6  je         0x496407

// block 004963d8
004963d8  dec        eax
004963d9  je         0x4963f9

// block 004963db
004963db  dec        eax
004963dc  je         0x4963e9

// block 004963de
004963de  dec        eax
004963df  jne        0x496409

// block 004963e1
004963e1  or         dword ptr [esi], 0xc00
004963e7  jmp        0x496409

// block 004963e9
004963e9  mov        eax, dword ptr [esi]
004963eb  and        eax, 0xfffffbff
004963f0  or         eax, 0x800

// block 004963f5
004963f5  mov        dword ptr [esi], eax
004963f7  jmp        0x496409

// block 004963f9
004963f9  mov        eax, dword ptr [esi]
004963fb  and        eax, 0xfffff7ff
00496400  or         eax, 0x400
00496405  jmp        0x4963f5

// block 00496407
00496407  and        dword ptr [esi], edx

// block 00496409
00496409  mov        eax, dword ptr [ecx]
0049640b  shr        eax, 2
0049640e  and        eax, 7
00496411  sub        eax, ebx
00496413  je         0x49642a

// block 00496415
00496415  dec        eax
00496416  je         0x49641f

// block 00496418
00496418  dec        eax
00496419  jne        0x496435

// block 0049641b
0049641b  and        dword ptr [esi], edx
0049641d  jmp        0x496435

// block 0049641f
0049641f  mov        eax, dword ptr [esi]
00496421  and        eax, edx
00496423  or         eax, 0x200
00496428  jmp        0x496433

// block 0049642a
0049642a  mov        eax, dword ptr [esi]
0049642c  and        eax, edx
0049642e  or         eax, 0x300

// block 00496433
00496433  mov        dword ptr [esi], eax

// block 00496435
00496435  cmp        dword ptr [ebp + 0x20], ebx
00496438  je         0x496441

// block 0049643a
0049643a  fld        dword ptr [ecx + 0x50]
0049643d  fstp       dword ptr [edi]
0049643f  jmp        0x496446

// block 00496441
00496441  fld        qword ptr [ecx + 0x50]
00496444  fstp       qword ptr [edi]

// block 00496446
00496446  pop        edi
00496447  pop        esi
00496448  pop        ebx
00496449  pop        ebp
0049644a  ret        
