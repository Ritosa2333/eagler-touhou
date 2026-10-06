
// block 004442b0
004442b0  push       edi
004442b1  mov        edi, edx
004442b3  movsx      edx, word ptr [edi + eax*2 + 0x5a68]
004442bb  cmp        edx, ecx
004442bd  je         0x444375

// block 004442c3
004442c3  test       eax, eax
004442c5  je         0x4442e1

// block 004442c7
004442c7  movsx      edx, word ptr [edi + 0x5a68]
004442ce  cmp        edx, ecx
004442d0  jne        0x4442e1

// block 004442d2
004442d2  mov        dx, word ptr [edi + eax*2 + 0x5a68]
004442da  mov        word ptr [edi + 0x5a68], dx

// block 004442e1
004442e1  cmp        eax, 1
004442e4  je         0x444300

// block 004442e6
004442e6  movsx      edx, word ptr [edi + 0x5a6a]
004442ed  cmp        edx, ecx
004442ef  jne        0x444300

// block 004442f1
004442f1  mov        dx, word ptr [edi + eax*2 + 0x5a68]
004442f9  mov        word ptr [edi + 0x5a6a], dx

// block 00444300
00444300  cmp        eax, 2
00444303  je         0x44431f

// block 00444305
00444305  movsx      edx, word ptr [edi + 0x5a6c]
0044430c  cmp        edx, ecx
0044430e  jne        0x44431f

// block 00444310
00444310  mov        dx, word ptr [edi + eax*2 + 0x5a68]
00444318  mov        word ptr [edi + 0x5a6c], dx

// block 0044431f
0044431f  cmp        eax, 3
00444322  je         0x44433e

// block 00444324
00444324  movsx      edx, word ptr [edi + 0x5a6e]
0044432b  cmp        edx, ecx
0044432d  jne        0x44433e

// block 0044432f
0044432f  mov        dx, word ptr [edi + eax*2 + 0x5a68]
00444337  mov        word ptr [edi + 0x5a6e], dx

// block 0044433e
0044433e  cmp        eax, 4
00444341  je         0x44435d

// block 00444343
00444343  movsx      edx, word ptr [edi + 0x5a70]
0044434a  cmp        edx, ecx
0044434c  jne        0x44435d

// block 0044434e
0044434e  mov        dx, word ptr [edi + eax*2 + 0x5a68]
00444356  mov        word ptr [edi + 0x5a70], dx

// block 0044435d
0044435d  mov        word ptr [edi + eax*2 + 0x5a68], cx
00444365  call       0x443920

// block 0044436a
0044436a  mov        edx, 7
0044436f  pop        edi
00444370  jmp        0x453d90

// block 00444375
00444375  pop        edi
00444376  ret        
