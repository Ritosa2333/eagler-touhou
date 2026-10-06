
// block 00493550
00493550  cmp        dword ptr [0x4d52d4], 0
00493557  je         0x4935df

// block 0049355d
0049355d  sub        esp, 8
00493560  stmxcsr    dword ptr [esp + 4]
00493565  mov        eax, dword ptr [esp + 4]
00493569  and        eax, 0x1f80
0049356e  cmp        eax, 0x1f80
00493573  jne        0x493584

// block 00493575
00493575  fnstcw     word ptr [esp]
00493578  mov        ax, word ptr [esp]
0049357c  and        ax, 0x7f
00493580  cmp        ax, 0x7f

// block 00493584
00493584  lea        esp, [esp + 8]
00493588  jne        0x4935df

// block 0049358a
0049358a  jmp        0x495808

// block 004935df
004935df  lea        edx, [esp + 4]
004935e3  call       0x4956a5

// block 00495808
00495808  movq       xmm7, qword ptr [esp + 4]
