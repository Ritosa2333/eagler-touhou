
// block 00493400
00493400  cmp        dword ptr [0x4d52d4], 0
00493407  je         0x49348f

// block 0049340d
0049340d  sub        esp, 8
00493410  stmxcsr    dword ptr [esp + 4]
00493415  mov        eax, dword ptr [esp + 4]
00493419  and        eax, 0x1f80
0049341e  cmp        eax, 0x1f80
00493423  jne        0x493434

// block 00493425
00493425  fnstcw     word ptr [esp]
00493428  mov        ax, word ptr [esp]
0049342c  and        ax, 0x7f
00493430  cmp        ax, 0x7f

// block 00493434
00493434  lea        esp, [esp + 8]
00493438  jne        0x49348f

// block 0049343a
0049343a  jmp        0x4950b8

// block 0049348f
0049348f  lea        edx, [esp + 4]
00493493  call       0x4956a5

// block 004950b8
004950b8  movlpd     xmm0, qword ptr [esp + 4]
