
// block 00493590
00493590  cmp        dword ptr [0x4d52d4], 0
00493597  je         0x4935cb

// block 00493599
00493599  sub        esp, 8
0049359c  stmxcsr    dword ptr [esp + 4]
004935a1  mov        eax, dword ptr [esp + 4]
004935a5  and        eax, 0x1f80
004935aa  cmp        eax, 0x1f80
004935af  jne        0x4935c0

// block 004935b1
004935b1  fnstcw     word ptr [esp]
004935b4  mov        ax, word ptr [esp]
004935b8  and        ax, 0x7f
004935bc  cmp        ax, 0x7f

// block 004935c0
004935c0  lea        esp, [esp + 8]
004935c4  jne        0x4935cb

// block 004935c6
004935c6  jmp        0x4957f0

// block 004935cb
004935cb  sub        esp, 0xc
004935ce  fst        qword ptr [esp]
004935d1  call       0x4956e8

// block 004935d6
004935d6  call       0x4935e8

// block 004935db
004935db  add        esp, 0xc
004935de  ret        

// block 004957f0
004957f0  push       ebp
004957f1  mov        ebp, esp
004957f3  sub        esp, 8
004957f6  and        esp, 0xfffffff0
004957f9  fstp       qword ptr [esp]
004957fc  movq       xmm7, qword ptr [esp]
00495801  call       0x49580e

// block 00495806
00495806  leave      
00495807  ret        
