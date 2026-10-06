
// block 00431b30
00431b30  mov        eax, dword ptr [0x4b43e4]
00431b35  mov        ecx, dword ptr [0x4b0c44]
00431b3b  mov        dword ptr [eax + 0x6cdc], ecx
00431b41  mov        eax, ecx
00431b43  cmp        dword ptr [0x4b0c40], eax
00431b49  jge        0x431b50

// block 00431b4b
00431b4b  mov        dword ptr [0x4b0c40], eax

// block 00431b50
00431b50  ret        
