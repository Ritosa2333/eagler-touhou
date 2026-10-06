
// block 00412da0
00412da0  xor        eax, eax
00412da2  push       0x1098
00412da7  push       eax
00412da8  push       esi
00412da9  mov        dword ptr [esi + 0x1090], eax
00412daf  mov        dword ptr [esi + 0x1094], eax
00412db5  call       0x477420

// block 00412dba
00412dba  add        esp, 0xc
00412dbd  mov        dword ptr [esi], 0x49fc6c
00412dc3  mov        eax, esi
00412dc5  ret        
