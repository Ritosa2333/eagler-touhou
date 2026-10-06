
// block 00486001
00486001  mov        edi, edi
00486003  push       ebp
00486004  mov        ebp, esp
00486006  push       ebx
00486007  push       esi
00486008  push       edi
00486009  call       0x475467

// block 0048600e
0048600e  mov        ebx, dword ptr [ebp + 8]
00486011  mov        esi, eax
00486013  add        esi, 0x9c
00486019  test       ebx, ebx
0048601b  jne        0x486029

// block 0048601d
0048601d  or         dword ptr [esi + 8], 0x104
00486024  jmp        0x4860e6

// block 00486029
00486029  lea        eax, [ebx + 0x40]
0048602c  lea        edi, [esi + 4]
0048602f  mov        dword ptr [esi], ebx
00486031  mov        dword ptr [edi], eax
00486033  test       eax, eax
00486035  je         0x48604c

// block 00486037
00486037  cmp        byte ptr [eax], 0
0048603a  je         0x48604c

// block 0048603c
0048603c  push       edi
0048603d  push       0x16
0048603f  push       0x49f220
00486044  call       0x485a06

// block 00486049
00486049  add        esp, 0xc

// block 0048604c
0048604c  mov        eax, dword ptr [esi]
0048604e  and        dword ptr [esi + 8], 0
00486052  test       eax, eax
00486054  je         0x4860a9

// block 00486056
00486056  cmp        byte ptr [eax], 0
00486059  je         0x4860a9

// block 0048605b
0048605b  mov        eax, dword ptr [edi]
0048605d  test       eax, eax
0048605f  je         0x48606d

// block 00486061
00486061  cmp        byte ptr [eax], 0
00486064  je         0x48606d

// block 00486066
00486066  call       0x485f5e

// block 0048606b
0048606b  jmp        0x486072

// block 0048606d
0048606d  call       0x485fc5

// block 00486072
00486072  cmp        dword ptr [esi + 8], 0
00486076  jne        0x4860fc

// block 0048607c
0048607c  push       esi
0048607d  push       0x40
0048607f  push       0x49f018
00486084  call       0x485a06

// block 00486089
00486089  add        esp, 0xc
0048608c  test       eax, eax
0048608e  je         0x4860f2

// block 00486090
00486090  mov        edi, dword ptr [edi]
00486092  test       edi, edi
00486094  je         0x4860a2

// block 00486096
00486096  cmp        byte ptr [edi], 0
00486099  je         0x4860a2

// block 0048609b
0048609b  call       0x485f5e

// block 004860a0
004860a0  jmp        0x4860f2

// block 004860a2
004860a2  call       0x485fc5

// block 004860a7
004860a7  jmp        0x4860f2

// block 004860a9
004860a9  mov        edi, dword ptr [edi]
004860ab  test       edi, edi
004860ad  je         0x4860df

// block 004860af
004860af  cmp        byte ptr [edi], 0
004860b2  je         0x4860df

// block 004860b4
004860b4  push       edi
004860b5  call       0x475860

// block 004860ba
004860ba  sub        eax, 3
004860bd  neg        eax
004860bf  pop        ecx
004860c0  sbb        eax, eax
004860c2  push       1
004860c4  inc        eax
004860c5  push       0x485b93
004860ca  mov        dword ptr [esi + 0x14], eax
004860cd  call       dword ptr [0x498194]

// block 004860d3
004860d3  test       byte ptr [esi + 8], 4
004860d7  jne        0x4860f2

// block 004860d9
004860d9  and        dword ptr [esi + 8], 0
004860dd  jmp        0x4860f2

// block 004860df
004860df  mov        dword ptr [esi + 8], 0x104

// block 004860e6
004860e6  call       dword ptr [0x49803c]

// block 004860ec
004860ec  mov        dword ptr [esi + 0x18], eax
004860ef  mov        dword ptr [esi + 0x1c], eax

// block 004860f2
004860f2  cmp        dword ptr [esi + 8], 0
004860f6  je         0x4861eb

// block 004860fc
004860fc  mov        ecx, ebx
004860fe  sub        ebx, -0x80
00486101  neg        ecx
00486103  sbb        ecx, ecx
00486105  and        ecx, ebx
00486107  mov        edi, esi
00486109  call       0x485a7c

// block 0048610e
0048610e  mov        edi, eax
00486110  mov        dword ptr [ebp + 8], edi
00486113  test       edi, edi
00486115  je         0x4861eb

// block 0048611b
0048611b  cmp        edi, 0xfde8
00486121  je         0x4861eb

// block 00486127
00486127  cmp        edi, 0xfde9
0048612d  je         0x4861eb

// block 00486133
00486133  movzx      eax, di
00486136  push       eax
00486137  call       dword ptr [0x498150]

// block 0048613d
0048613d  test       eax, eax
0048613f  je         0x4861eb

// block 00486145
00486145  push       1
00486147  push       dword ptr [esi + 0x18]
0048614a  call       dword ptr [0x498198]

// block 00486150
00486150  test       eax, eax
00486152  je         0x4861eb

// block 00486158
00486158  mov        eax, dword ptr [ebp + 0xc]
0048615b  test       eax, eax
0048615d  je         0x486172

// block 0048615f
0048615f  mov        cx, word ptr [esi + 0x18]
00486163  mov        word ptr [eax], cx
00486166  mov        cx, word ptr [esi + 0x1c]
0048616a  mov        word ptr [eax + 2], cx
0048616e  mov        word ptr [eax + 4], di

// block 00486172
00486172  mov        ebx, dword ptr [ebp + 0x10]
00486175  test       ebx, ebx
00486177  je         0x4861e6

// block 00486179
00486179  mov        edi, dword ptr [0x4980e0]
0048617f  mov        ecx, 0x814
00486184  cmp        word ptr [eax], cx
00486187  jne        0x4861ae

// block 00486189
00486189  push       0x49f2f8
0048618e  push       0x40
00486190  push       ebx
00486191  call       0x47a2b9

// block 00486196
00486196  add        esp, 0xc
00486199  test       eax, eax
0048619b  je         0x4861bf

// block 0048619d
0048619d  xor        eax, eax
0048619f  push       eax
004861a0  push       eax
004861a1  push       eax
004861a2  push       eax
004861a3  push       eax
004861a4  call       0x470dc6

// block 004861a9
004861a9  add        esp, 0x14
004861ac  jmp        0x4861bf

// block 004861ae
004861ae  push       0x40
004861b0  push       ebx
004861b1  push       0x1001
004861b6  push       dword ptr [esi + 0x18]
004861b9  call       edi

// block 004861bb
004861bb  test       eax, eax
004861bd  je         0x4861eb

// block 004861bf
004861bf  push       0x40
004861c1  lea        eax, [ebx + 0x40]
004861c4  push       eax
004861c5  push       0x1002
004861ca  push       dword ptr [esi + 0x1c]
004861cd  call       edi

// block 004861cf
004861cf  test       eax, eax
004861d1  je         0x4861eb

// block 004861d3
004861d3  push       0xa
004861d5  push       0x10
004861d7  sub        ebx, -0x80
004861da  push       ebx
004861db  push       dword ptr [ebp + 8]
004861de  call       0x48da89

// block 004861e3
004861e3  add        esp, 0x10

// block 004861e6
004861e6  xor        eax, eax
004861e8  inc        eax
004861e9  jmp        0x4861ed

// block 004861eb
004861eb  xor        eax, eax

// block 004861ed
004861ed  pop        edi
004861ee  pop        esi
004861ef  pop        ebx
004861f0  pop        ebp
004861f1  ret        
