
// block 00401610
00401610  sub        esp, 0x104
00401616  mov        eax, dword ptr [0x4ad138]
0040161b  xor        eax, esp
0040161d  mov        dword ptr [esp + 0x100], eax
00401624  cmp        ecx, 0x3e8
0040162a  push       esi
0040162b  mov        esi, dword ptr [0x4b43b8]
00401631  jge        0x40164b

// block 00401633
00401633  push       ecx
00401634  lea        eax, [esp + 8]
00401638  push       0x49f484
0040163d  push       eax
0040163e  call       0x46cbbb

// block 00401643
00401643  add        esp, 0xc
00401646  jmp        0x4016f3

// block 0040164b
0040164b  mov        eax, 0x10624dd3
00401650  imul       ecx
00401652  sar        edx, 6
00401655  mov        eax, edx
00401657  shr        eax, 0x1f
0040165a  add        eax, edx
0040165c  mov        edx, eax
0040165e  imul       edx, edx, 0x3e8
00401664  cmp        ecx, 0xf4240
0040166a  jge        0x401684

// block 0040166c
0040166c  sub        ecx, edx
0040166e  push       ecx
0040166f  push       eax
00401670  lea        eax, [esp + 0xc]
00401674  push       0x49f488
00401679  push       eax
0040167a  call       0x46cbbb

// block 0040167f
0040167f  add        esp, 0x10
00401682  jmp        0x4016f3

// block 00401684
00401684  push       edi
00401685  mov        edi, ecx
00401687  sub        edi, edx
00401689  cdq        
0040168a  push       edi
0040168b  mov        edi, 0x3e8
00401690  idiv       edi
00401692  mov        eax, 0x431bde83
00401697  push       edx
00401698  imul       ecx
0040169a  sar        edx, 0x12
0040169d  mov        eax, edx
0040169f  shr        eax, 0x1f
004016a2  add        eax, edx
004016a4  cmp        ecx, 0x3b9aca00
004016aa  cdq        
004016ab  jge        0x4016c6

// block 004016ad
004016ad  mov        ecx, edi
004016af  idiv       ecx
004016b1  push       edx
004016b2  lea        edx, [esp + 0x14]
004016b6  push       0x49f490
004016bb  push       edx
004016bc  call       0x46cbbb

// block 004016c1
004016c1  add        esp, 0x14
004016c4  jmp        0x4016f2

// block 004016c6
004016c6  idiv       edi
004016c8  mov        eax, 0x44b82fa1
004016cd  push       edx
004016ce  imul       ecx
004016d0  sar        edx, 0x1c
004016d3  mov        eax, edx
004016d5  shr        eax, 0x1f
004016d8  add        eax, edx
004016da  cdq        
004016db  mov        ecx, edi
004016dd  idiv       ecx
004016df  push       edx
004016e0  lea        edx, [esp + 0x18]
004016e4  push       0x49f4a0
004016e9  push       edx
004016ea  call       0x46cbbb

// block 004016ef
004016ef  add        esp, 0x18

// block 004016f2
004016f2  pop        edi

// block 004016f3
004016f3  lea        eax, [esp + 4]
004016f7  mov        ecx, esi
004016f9  call       0x4014f0

// block 004016fe
004016fe  mov        ecx, dword ptr [esp + 0x104]
00401705  pop        esi
00401706  xor        ecx, esp
00401708  call       0x46c932

// block 0040170d
0040170d  add        esp, 0x104
00401713  ret        
