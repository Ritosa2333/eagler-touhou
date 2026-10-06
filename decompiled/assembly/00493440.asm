
// block 00493440
00493440  cmp        dword ptr [0x4d52d4], 0
00493447  je         0x49347b

// block 00493449
00493449  sub        esp, 8
0049344c  stmxcsr    dword ptr [esp + 4]
00493451  mov        eax, dword ptr [esp + 4]
00493455  and        eax, 0x1f80
0049345a  cmp        eax, 0x1f80
0049345f  jne        0x493470

// block 00493461
00493461  fnstcw     word ptr [esp]
00493464  mov        ax, word ptr [esp]
00493468  and        ax, 0x7f
0049346c  cmp        ax, 0x7f

// block 00493470
00493470  lea        esp, [esp + 8]
00493474  jne        0x49347b

// block 00493476
00493476  jmp        0x4950a0

// block 0049347b
0049347b  sub        esp, 0xc
0049347e  fst        qword ptr [esp]
00493481  call       0x4956e8

// block 00493486
00493486  call       0x493498

// block 0049348b
0049348b  add        esp, 0xc
0049348e  ret        

// block 004950a0
004950a0  push       ebp
004950a1  mov        ebp, esp
004950a3  sub        esp, 8
004950a6  and        esp, 0xfffffff0
004950a9  fstp       qword ptr [esp]
004950ac  movq       xmm0, qword ptr [esp]
004950b1  call       0x4950be

// block 004950b6
004950b6  leave      
004950b7  ret        
