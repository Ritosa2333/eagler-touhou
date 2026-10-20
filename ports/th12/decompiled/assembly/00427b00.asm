
// block 00427b00
00427b00  push       ebx
00427b01  push       esi
00427b02  mov        esi, eax
00427b04  mov        eax, dword ptr [0x4b43c8]
00427b09  push       edi
00427b0a  mov        edi, dword ptr [esi + 0x9b4]
00427b10  mov        dword ptr [esi + 0x9b0], 2
00427b1a  mov        ebx, dword ptr [eax + 0x4debdc]
00427b20  add        edi, 0xa5
00427b26  call       0x402520

// block 00427b2b
00427b2b  mov        al, 0x10
00427b2d  push       edi
00427b2e  push       esi
00427b2f  mov        ecx, ebx
00427b31  mov        byte ptr [esi + 0x49d], al
00427b37  mov        byte ptr [esi + 0x49c], al
00427b3d  call       0x454d10

// block 00427b42
00427b42  pop        edi
00427b43  pop        esi
00427b44  xor        eax, eax
00427b46  pop        ebx
00427b47  ret        
