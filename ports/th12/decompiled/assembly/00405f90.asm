
// block 00405f90
00405f90  push       esi
00405f91  push       edi
00405f92  mov        ecx, 7
00405f97  mov        esi, eax
00405f99  mov        edi, edx

// block 00405f9b
00405f9b  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405f9d
00405f9d  pop        edi
00405f9e  pop        esi
00405f9f  ret        
