
// block 004755b0
004755b0  mov        edi, edi
004755b2  push       ebp
004755b3  mov        ebp, esp
004755b5  cmp        dword ptr [0x4adac8], -1
004755bc  je         0x475609

// block 004755be
004755be  cmp        dword ptr [ebp + 8], 0
004755c2  jne        0x4755eb

// block 004755c4
004755c4  push       esi
004755c5  push       dword ptr [0x4adacc]
004755cb  mov        esi, dword ptr [0x49814c]
004755d1  call       esi

// block 004755d3
004755d3  test       eax, eax
004755d5  je         0x4755ea

// block 004755d7
004755d7  push       dword ptr [0x4adac8]
004755dd  push       dword ptr [0x4adacc]
004755e3  call       esi

// block 004755e5
004755e5  call       eax

// block 004755e7
004755e7  mov        dword ptr [ebp + 8], eax

// block 004755ea
004755ea  pop        esi

// block 004755eb
004755eb  push       0
004755ed  push       dword ptr [0x4adac8]
004755f3  push       dword ptr [0x4b4108]
004755f9  call       0x4751de

// block 004755fe
004755fe  pop        ecx
004755ff  call       eax

// block 00475601
00475601  push       dword ptr [ebp + 8]
00475604  call       0x475481

// block 00475609
00475609  mov        eax, dword ptr [0x4adacc]
0047560e  cmp        eax, -1
00475611  je         0x47561c

// block 00475613
00475613  push       0
00475615  push       eax
00475616  call       dword ptr [0x498144]

// block 0047561c
0047561c  pop        ebp
0047561d  ret        
