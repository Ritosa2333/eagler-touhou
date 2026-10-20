
// block 00427f40
00427f40  push       esi
00427f41  push       edi
00427f42  mov        ecx, 0x7a
00427f47  mov        esi, edx
00427f49  mov        edi, eax

// block 00427f4b
00427f4b  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00427f4d
00427f4d  pop        edi
00427f4e  pop        esi
00427f4f  ret        
