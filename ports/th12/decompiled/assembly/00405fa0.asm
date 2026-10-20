
// block 00405fa0
00405fa0  push       esi
00405fa1  push       edi
00405fa2  mov        edi, eax
00405fa4  add        edi, 0x1c
00405fa7  mov        ecx, 7
00405fac  mov        esi, edx

// block 00405fae
00405fae  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405fb0
00405fb0  pop        edi
00405fb1  pop        esi
00405fb2  ret        
