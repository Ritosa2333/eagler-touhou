
// block 00427f70
00427f70  dec        dword ptr [ecx + 0x468]
00427f76  mov        edx, dword ptr [eax + 4]
00427f79  push       esi
00427f7a  mov        esi, dword ptr [eax + 8]
00427f7d  mov        dword ptr [edx + 8], esi
00427f80  mov        edx, dword ptr [eax + 8]
00427f83  test       edx, edx
00427f85  je         0x427f8d

// block 00427f87
00427f87  mov        esi, dword ptr [eax + 4]
00427f8a  mov        dword ptr [edx + 4], esi

// block 00427f8d
00427f8d  pop        esi
00427f8e  cmp        dword ptr [ecx + 0x464], eax
00427f94  jne        0x427f9f

// block 00427f96
00427f96  mov        eax, dword ptr [eax + 4]
00427f99  mov        dword ptr [ecx + 0x464], eax

// block 00427f9f
00427f9f  ret        
