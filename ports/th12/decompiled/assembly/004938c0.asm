
// block 004938c0
004938c0  cmp        dword ptr [0x4d52d4], 0
004938c7  je         0x4938fb

// block 004938c9
004938c9  sub        esp, 8
004938cc  stmxcsr    dword ptr [esp + 4]
004938d1  mov        eax, dword ptr [esp + 4]
004938d5  and        eax, 0x1f80
004938da  cmp        eax, 0x1f80
004938df  jne        0x4938f0

// block 004938e1
004938e1  fnstcw     word ptr [esp]
004938e4  mov        ax, word ptr [esp]
004938e8  and        ax, 0x7f
004938ec  cmp        ax, 0x7f

// block 004938f0
004938f0  lea        esp, [esp + 8]
004938f4  jne        0x4938fb

// block 004938f6
004938f6  jmp        0x495df0

// block 004938fb
004938fb  sub        esp, 0xc
004938fe  fst        qword ptr [esp]
00493901  call       0x4956e8

// block 00493906
00493906  call       0x493918

// block 0049390b
0049390b  add        esp, 0xc
0049390e  ret        

// block 00495df0
00495df0  push       ebp
00495df1  mov        ebp, esp
00495df3  sub        esp, 8
00495df6  and        esp, 0xfffffff0
00495df9  fstp       qword ptr [esp]
00495dfc  movq       xmm0, qword ptr [esp]
00495e01  call       0x495e0e

// block 00495e06
00495e06  leave      
00495e07  ret        
