
// block 004130e0
004130e0  push       esi
004130e1  push       0x7c
004130e3  call       0x46c9ea

// block 004130e8
004130e8  mov        esi, eax
004130ea  add        esp, 4
004130ed  test       esi, esi
004130ef  je         0x41310d

// block 004130f1
004130f1  and        dword ptr [esi + 0x60], 0xfffffffe
004130f5  push       0x7c
004130f7  push       0
004130f9  push       esi
004130fa  call       0x477420

// block 004130ff
004130ff  add        esp, 0xc
00413102  or         dword ptr [esi], 2
00413105  mov        dword ptr [0x4b43dc], esi
0041310b  jmp        0x41310f

// block 0041310d
0041310d  xor        esi, esi

// block 0041310f
0041310f  mov        eax, dword ptr [esp + 8]
00413113  push       edi
00413114  push       eax
00413115  mov        edi, esi
00413117  call       0x412c60

// block 0041311c
0041311c  pop        edi
0041311d  test       eax, eax
0041311f  je         0x41313b

// block 00413121
00413121  test       esi, esi
00413123  je         0x413135

// block 00413125
00413125  mov        eax, esi
00413127  call       0x412f10

// block 0041312c
0041312c  push       esi
0041312d  call       0x46ca4f

// block 00413132
00413132  add        esp, 4

// block 00413135
00413135  xor        eax, eax
00413137  pop        esi
00413138  ret        4

// block 0041313b
0041313b  mov        eax, esi
0041313d  pop        esi
0041313e  ret        4
