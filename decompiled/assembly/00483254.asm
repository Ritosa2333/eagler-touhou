
// block 00483254
00483254  mov        edi, edi
00483256  push       ebp
00483257  mov        ebp, esp
00483259  mov        eax, dword ptr [ebp + 8]
0048325c  test       eax, eax
0048325e  je         0x483268

// block 00483260
00483260  mov        ecx, dword ptr [ebp + 0xc]
00483263  mov        dword ptr [eax], ecx
00483265  add        eax, 8

// block 00483268
00483268  pop        ebp
00483269  ret        
