
// block 0049132c
0049132c  mov        edi, edi
0049132e  push       ebp
0049132f  mov        ebp, esp
00491331  push       ebx
00491332  xor        ebx, ebx
00491334  cmp        dword ptr [ebp + 8], ebx
00491337  jne        0x49133d

// block 00491339
00491339  xor        eax, eax
0049133b  jmp        0x49137e

// block 0049133d
0049133d  push       esi
0049133e  push       edi
0049133f  push       dword ptr [ebp + 8]
00491342  call       0x475860

// block 00491347
00491347  mov        esi, eax
00491349  inc        esi
0049134a  push       esi
0049134b  call       0x46d04a

// block 00491350
00491350  mov        edi, eax
00491352  pop        ecx
00491353  pop        ecx
00491354  cmp        edi, ebx
00491356  je         0x49137a

// block 00491358
00491358  push       dword ptr [ebp + 8]
0049135b  push       esi
0049135c  push       edi
0049135d  call       0x47a2b9

// block 00491362
00491362  add        esp, 0xc
00491365  test       eax, eax
00491367  je         0x491376

// block 00491369
00491369  push       ebx
0049136a  push       ebx
0049136b  push       ebx
0049136c  push       ebx
0049136d  push       ebx
0049136e  call       0x470dc6

// block 00491373
00491373  add        esp, 0x14

// block 00491376
00491376  mov        eax, edi
00491378  jmp        0x49137c

// block 0049137a
0049137a  xor        eax, eax

// block 0049137c
0049137c  pop        edi
0049137d  pop        esi

// block 0049137e
0049137e  pop        ebx
0049137f  pop        ebp
00491380  ret        
