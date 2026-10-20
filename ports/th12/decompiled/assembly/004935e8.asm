
// block 004935e8
004935e8  push       edx
004935e9  wait       
004935ea  fnstcw     word ptr [esp]
004935ed  je         0x493625

// block 004935ef
004935ef  cmp        word ptr [esp], 0x27f
004935f5  je         0x4935fd

// block 004935f7
004935f7  fldcw      word ptr [0x4a5dd8]

// block 004935fd
004935fd  fld1       
004935ff  fpatan     

// block 00493601
00493601  cmp        dword ptr [0x4b4110], 0
00493608  jne        0x4956fe

// block 0049360e
0049360e  mov        edx, 0xf
00493613  lea        ecx, [0x4b3570]
00493619  jmp        0x49570b

// block 0049361e
0049361e  call       0x49568c

// block 00493623
00493623  jmp        0x49364b

// block 00493625
00493625  test       eax, 0xfffff
0049362a  jne        0x49361e

// block 0049362c
0049362c  cmp        dword ptr [esp + 8], 0
00493631  jne        0x49361e

// block 00493633
00493633  fstp       st(0)
00493635  fld        xword ptr [0x4b35ca]
0049363b  test       eax, 0x80000000
00493640  je         0x493601

// block 00493642
00493642  fchs       
00493644  jmp        0x493601

// block 0049364b
0049364b  cmp        dword ptr [0x4b4110], 0
00493652  jne        0x4956fe

// block 00493658
00493658  mov        edx, 0xf
0049365d  lea        ecx, [0x4b3570]
00493663  call       0x495617

// block 00493668
00493668  pop        edx
00493669  ret        
