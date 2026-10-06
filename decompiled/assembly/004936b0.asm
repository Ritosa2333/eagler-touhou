
// block 004936b0
004936b0  cmp        dword ptr [0x4d52d4], 0
004936b7  je         0x4936eb

// block 004936b9
004936b9  sub        esp, 8
004936bc  stmxcsr    dword ptr [esp + 4]
004936c1  mov        eax, dword ptr [esp + 4]
004936c5  and        eax, 0x1f80
004936ca  cmp        eax, 0x1f80
004936cf  jne        0x4936e0

// block 004936d1
004936d1  fnstcw     word ptr [esp]
004936d4  mov        ax, word ptr [esp]
004936d8  and        ax, 0x7f
004936dc  cmp        ax, 0x7f

// block 004936e0
004936e0  lea        esp, [esp + 8]
004936e4  jne        0x4936eb

// block 004936e6
004936e6  jmp        0x495ae0

// block 004936eb
004936eb  sub        esp, 0xc
004936ee  fst        qword ptr [esp]
004936f1  call       0x4956e8

// block 004936f6
004936f6  call       0x493708

// block 004936fb
004936fb  add        esp, 0xc
004936fe  ret        

// block 00495ae0
00495ae0  push       ebp
00495ae1  mov        ebp, esp
00495ae3  sub        esp, 8
00495ae6  and        esp, 0xfffffff0
00495ae9  fstp       qword ptr [esp]
00495aec  movq       xmm0, qword ptr [esp]
00495af1  call       0x495afe

// block 00495af6
00495af6  leave      
00495af7  ret        
