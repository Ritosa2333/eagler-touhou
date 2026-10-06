
// block 00412500
00412500  test       ecx, ecx
00412502  je         0x412521

// block 00412504
00412504  mov        eax, dword ptr [0x4b43dc]
00412509  mov        eax, dword ptr [eax + 0x68]
0041250c  test       eax, eax
0041250e  je         0x412521

// block 00412510
00412510  mov        edx, dword ptr [eax]
00412512  cmp        dword ptr [edx + 0x27b4], ecx
00412518  je         0x412524

// block 0041251a
0041251a  mov        eax, dword ptr [eax + 4]
0041251d  test       eax, eax
0041251f  jne        0x412510

// block 00412521
00412521  xor        eax, eax
00412523  ret        

// block 00412524
00412524  mov        eax, 1
00412529  ret        
