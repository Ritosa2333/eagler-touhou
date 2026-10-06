
// block 0048326a
0048326a  mov        edi, edi
0048326c  push       ebp
0048326d  mov        ebp, esp
0048326f  mov        eax, dword ptr [ebp + 8]
00483272  test       eax, eax
00483274  je         0x483288

// block 00483276
00483276  sub        eax, 8
00483279  cmp        dword ptr [eax], 0xdddd
0048327f  jne        0x483288

// block 00483281
00483281  push       eax
00483282  call       0x46c941

// block 00483287
00483287  pop        ecx

// block 00483288
00483288  pop        ebp
00483289  ret        
