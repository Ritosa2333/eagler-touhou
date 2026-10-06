
// block 00410ba0
00410ba0  mov        eax, dword ptr [0x4b43d8]
00410ba5  push       ebx
00410ba6  push       esi
00410ba7  mov        esi, dword ptr [eax + 0x18]
00410baa  mov        ecx, dword ptr [esi + 0xec]
00410bb0  mov        ebx, dword ptr [esi + 0x70]
00410bb3  add        ecx, 0x1c
00410bb6  call       0x45fe60

// block 00410bbb
00410bbb  mov        ecx, dword ptr [esi + 0xec]
00410bc1  mov        dword ptr [esi + ecx*4 + 0x80], eax
00410bc8  and        dword ptr [esi + 0x74], 0xfffffffb
00410bcc  mov        esi, dword ptr [0x4b43b8]
00410bd2  mov        edx, dword ptr [esi + 0x18fbc]
00410bd8  add        esi, 0x18fbc
00410bde  push       edx
00410bdf  mov        ebx, 1
00410be4  call       0x461970

// block 00410be9
00410be9  mov        dword ptr [esi], 0
00410bef  pop        esi
00410bf0  xor        eax, eax
00410bf2  pop        ebx
00410bf3  ret        
