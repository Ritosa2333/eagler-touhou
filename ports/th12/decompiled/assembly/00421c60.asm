
// block 00421c60
00421c60  mov        eax, dword ptr [0x4b43dc]
00421c65  fldz       
00421c67  mov        ecx, dword ptr [eax + 0x60]
00421c6a  xor        edx, edx
00421c6c  test       cl, 1
00421c6f  jne        0x421c8b

// block 00421c71
00421c71  or         ecx, 1
00421c74  fst        dword ptr [eax + 0x58]
00421c77  mov        dword ptr [eax + 0x54], edx
00421c7a  mov        dword ptr [eax + 0x50], 0xfff0bdc1
00421c81  mov        dword ptr [eax + 0x5c], 0x4b2ed0
00421c88  mov        dword ptr [eax + 0x60], ecx

// block 00421c8b
00421c8b  fstp       dword ptr [eax + 0x58]
00421c8e  mov        dword ptr [eax + 0x54], edx
00421c91  mov        dword ptr [eax + 0x50], 0xffffffff
00421c98  mov        dword ptr [eax + 0x68], edx
00421c9b  mov        dword ptr [eax + 0x6c], edx
00421c9e  xor        eax, eax
00421ca0  ret        
