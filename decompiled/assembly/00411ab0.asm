
// block 00411ab0
00411ab0  push       ebx
00411ab1  push       esi
00411ab2  mov        esi, eax
00411ab4  mov        ecx, dword ptr [esi + 0xec]
00411aba  mov        ebx, dword ptr [esi + 0x70]
00411abd  add        ecx, 0x1c
00411ac0  call       0x45fe60

// block 00411ac5
00411ac5  mov        ecx, dword ptr [esi + 0xec]
00411acb  mov        dword ptr [esi + ecx*4 + 0x80], eax
00411ad2  and        dword ptr [esi + 0x74], 0xfffffffb
00411ad6  mov        esi, dword ptr [0x4b43b8]
00411adc  mov        edx, dword ptr [esi + 0x18fbc]
00411ae2  add        esi, 0x18fbc
00411ae8  push       edx
00411ae9  mov        ebx, 1
00411aee  call       0x461970

// block 00411af3
00411af3  mov        dword ptr [esi], 0
00411af9  pop        esi
00411afa  xor        eax, eax
00411afc  pop        ebx
00411afd  ret        
