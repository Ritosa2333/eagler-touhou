
// block 00411f30
00411f30  xor        eax, eax
00411f32  push       0x1098
00411f37  push       eax
00411f38  mov        dword ptr [esi], 0x49fc28
00411f3e  push       esi
00411f3f  mov        dword ptr [esi + 0x1090], eax
00411f45  mov        dword ptr [esi + 0x1094], eax
00411f4b  call       0x477420

// block 00411f50
00411f50  add        esp, 0xc
00411f53  mov        eax, esi
00411f55  ret        
