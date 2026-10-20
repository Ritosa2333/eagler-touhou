
// block 00493670
00493670  cmp        dword ptr [0x4d52d4], 0
00493677  je         0x4936ff

// block 0049367d
0049367d  sub        esp, 8
00493680  stmxcsr    dword ptr [esp + 4]
00493685  mov        eax, dword ptr [esp + 4]
00493689  and        eax, 0x1f80
0049368e  cmp        eax, 0x1f80
00493693  jne        0x4936a4

// block 00493695
00493695  fnstcw     word ptr [esp]
00493698  mov        ax, word ptr [esp]
0049369c  and        ax, 0x7f
004936a0  cmp        ax, 0x7f

// block 004936a4
004936a4  lea        esp, [esp + 8]
004936a8  jne        0x4936ff

// block 004936aa
004936aa  jmp        0x495af8

// block 004936ff
004936ff  lea        edx, [esp + 4]
00493703  call       0x4956a5

// block 00495af8
00495af8  movlpd     xmm0, qword ptr [esp + 4]
