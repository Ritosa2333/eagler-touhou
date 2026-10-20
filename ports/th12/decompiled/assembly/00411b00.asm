
// block 00411b00
00411b00  push       ebx
00411b01  push       esi
00411b02  mov        esi, dword ptr [0x4b43b8]
00411b08  mov        eax, dword ptr [esi + 0x18fbc]
00411b0e  push       eax
00411b0f  mov        ebx, 1
00411b14  call       0x461970

// block 00411b19
00411b19  mov        dword ptr [esi + 0x18fbc], 0
00411b23  pop        esi
00411b24  pop        ebx
00411b25  ret        
