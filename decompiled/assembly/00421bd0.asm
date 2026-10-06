
// block 00421bd0
00421bd0  mov        ecx, dword ptr [eax + 4]
00421bd3  mov        edx, dword ptr [eax + 8]
00421bd6  mov        dword ptr [ecx + 8], edx
00421bd9  mov        ecx, dword ptr [eax + 8]
00421bdc  test       ecx, ecx
00421bde  je         0x421be6

// block 00421be0
00421be0  mov        eax, dword ptr [eax + 4]
00421be3  mov        dword ptr [ecx + 4], eax

// block 00421be6
00421be6  ret        
