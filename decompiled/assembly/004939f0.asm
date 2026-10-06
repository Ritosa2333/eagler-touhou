
// block 004939f0
004939f0  cmp        dword ptr [0x4d52d4], 0
004939f7  je         0x493a2b

// block 004939f9
004939f9  sub        esp, 8
004939fc  stmxcsr    dword ptr [esp + 4]
00493a01  mov        eax, dword ptr [esp + 4]
00493a05  and        eax, 0x1f80
00493a0a  cmp        eax, 0x1f80
00493a0f  jne        0x493a20

// block 00493a11
00493a11  fnstcw     word ptr [esp]
00493a14  mov        ax, word ptr [esp]
00493a18  and        ax, 0x7f
00493a1c  cmp        ax, 0x7f

// block 00493a20
00493a20  lea        esp, [esp + 8]
00493a24  jne        0x493a2b

// block 00493a26
00493a26  jmp        0x495fc0

// block 00493a2b
00493a2b  sub        esp, 0xc
00493a2e  fst        qword ptr [esp]
00493a31  call       0x4956e8

// block 00493a36
00493a36  call       0x493a48

// block 00493a3b
00493a3b  add        esp, 0xc
00493a3e  ret        

// block 00495fc0
00495fc0  push       ebp
00495fc1  mov        ebp, esp
00495fc3  sub        esp, 8
00495fc6  and        esp, 0xfffffff0
00495fc9  fstp       qword ptr [esp]
00495fcc  movq       xmm0, qword ptr [esp]
00495fd1  call       0x495fde

// block 00495fd6
00495fd6  leave      
00495fd7  ret        
