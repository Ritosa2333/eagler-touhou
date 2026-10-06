
// block 004021a0
004021a0  sub        esp, 0x110
004021a6  mov        eax, dword ptr [0x4ad138]
004021ab  xor        eax, esp
004021ad  mov        dword ptr [esp + 0x108], eax
004021b4  cmp        ecx, 0x3e8
004021ba  push       ebx
004021bb  push       esi
004021bc  mov        esi, dword ptr [0x4b43b8]
004021c2  push       edi
004021c3  mov        edi, edx
004021c5  jge        0x4021df

// block 004021c7
004021c7  push       ecx
004021c8  lea        eax, [esp + 0x14]
004021cc  push       0x49f484
004021d1  push       eax
004021d2  call       0x46cbbb

// block 004021d7
004021d7  add        esp, 0xc
004021da  jmp        0x402270

// block 004021df
004021df  cmp        ecx, 0xf4240
004021e5  jge        0x402218

// block 004021e7
004021e7  mov        eax, 0x10624dd3
004021ec  imul       ecx
004021ee  sar        edx, 6
004021f1  mov        eax, edx
004021f3  shr        eax, 0x1f
004021f6  add        eax, edx
004021f8  mov        edx, eax
004021fa  imul       edx, edx, 0x3e8
00402200  sub        ecx, edx
00402202  push       ecx
00402203  push       eax
00402204  lea        eax, [esp + 0x18]
00402208  push       0x49f488
0040220d  push       eax
0040220e  call       0x46cbbb

// block 00402213
00402213  add        esp, 0x10
00402216  jmp        0x402270

// block 00402218
00402218  cmp        ecx, 0x3b9aca00
0040221e  jge        0x402270

// block 00402220
00402220  mov        eax, 0x10624dd3
00402225  imul       ecx
00402227  sar        edx, 6
0040222a  mov        eax, edx
0040222c  shr        eax, 0x1f
0040222f  add        eax, edx
00402231  mov        edx, eax
00402233  imul       edx, edx, 0x3e8
00402239  mov        ebx, ecx
0040223b  sub        ebx, edx
0040223d  cdq        
0040223e  push       ebx
0040223f  mov        ebx, 0x3e8
00402244  idiv       ebx
00402246  mov        eax, 0x431bde83
0040224b  push       edx
0040224c  imul       ecx
0040224e  sar        edx, 0x12
00402251  mov        eax, edx
00402253  shr        eax, 0x1f
00402256  add        eax, edx
00402258  cdq        
00402259  mov        ecx, ebx
0040225b  idiv       ecx
0040225d  push       edx
0040225e  lea        edx, [esp + 0x1c]
00402262  push       0x49f490
00402267  push       edx
00402268  call       0x46cbbb

// block 0040226d
0040226d  add        esp, 0x14

// block 00402270
00402270  lea        eax, [esp + 0x10]
00402274  push       eax
00402275  call       0x4020a0

// block 0040227a
0040227a  mov        ecx, dword ptr [esp + 0x118]
00402281  add        esp, 4
00402284  pop        edi
00402285  pop        esi
00402286  pop        ebx
00402287  xor        ecx, esp
00402289  call       0x46c932

// block 0040228e
0040228e  add        esp, 0x110
00402294  ret        
