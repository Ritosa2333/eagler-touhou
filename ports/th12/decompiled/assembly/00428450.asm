
// block 00428450
00428450  push       esi
00428451  mov        esi, dword ptr [0x4b44f4]
00428457  cmp        dword ptr [esi + 0x468], 0x100
00428461  jl         0x428469

// block 00428463
00428463  xor        eax, eax
00428465  pop        esi
00428466  ret        4

// block 00428469
00428469  push       ebx
0042846a  mov        ebx, 1
0042846f  add        dword ptr [esi + 0x46c], ebx
00428475  mov        eax, dword ptr [esi + 0x46c]
0042847b  cmp        eax, 0x10000
00428480  jge        0x42848c

// block 00428482
00428482  mov        dword ptr [esi + 0x46c], 0x10000

// block 0042848c
0042848c  mov        eax, dword ptr [esp + 0xc]
00428490  sub        eax, 0
00428493  je         0x4284cd

// block 00428495
00428495  sub        eax, ebx
00428497  je         0x4284b5

// block 00428499
00428499  sub        eax, ebx
0042849b  jne        0x428515

// block 0042849d
0042849d  push       0xfa4
004284a2  call       0x46c9ea

// block 004284a7
004284a7  add        esp, 4
004284aa  test       eax, eax
004284ac  je         0x4284e5

// block 004284ae
004284ae  call       0x4285b0

// block 004284b3
004284b3  jmp        0x4284e7

// block 004284b5
004284b5  push       0xfc8
004284ba  call       0x46c9ea

// block 004284bf
004284bf  add        esp, 4
004284c2  test       eax, eax
004284c4  je         0x4284e5

// block 004284c6
004284c6  call       0x428560

// block 004284cb
004284cb  jmp        0x4284e7

// block 004284cd
004284cd  push       0xfa4
004284d2  call       0x46c9ea

// block 004284d7
004284d7  add        esp, 4
004284da  test       eax, eax
004284dc  je         0x4284e5

// block 004284de
004284de  call       0x428520

// block 004284e3
004284e3  jmp        0x4284e7

// block 004284e5
004284e5  xor        eax, eax

// block 004284e7
004284e7  mov        ecx, dword ptr [esi + 0x46c]
004284ed  mov        dword ptr [eax + 0x80], ecx
004284f3  mov        ecx, dword ptr [esi + 0x464]
004284f9  mov        dword ptr [eax + 4], ecx
004284fc  mov        dword ptr [ecx + 8], eax
004284ff  add        dword ptr [esi + 0x468], ebx
00428505  mov        dword ptr [esi + 0x464], eax
0042850b  mov        edx, dword ptr [eax]
0042850d  mov        ecx, eax
0042850f  mov        eax, dword ptr [edx + 4]
00428512  push       edi
00428513  call       eax

// block 00428515
00428515  mov        eax, dword ptr [esi + 0x46c]
0042851b  pop        ebx
0042851c  pop        esi
0042851d  ret        4
