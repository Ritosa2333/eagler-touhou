
// block 00427b90
00427b90  push       esi
00427b91  fldz       
00427b93  mov        esi, dword ptr [esp + 8]
00427b97  mov        dword ptr [eax + 0xe0], esi
00427b9d  mov        esi, dword ptr [0x4ce8d0]
00427ba3  mov        dword ptr [eax + 0xb4], esi
00427ba9  mov        esi, dword ptr [0x4ce8d4]
00427baf  mov        dword ptr [eax + 0xb8], esi
00427bb5  mov        esi, dword ptr [0x4ce8d8]
00427bbb  mov        dword ptr [eax + 0xbc], esi
00427bc1  mov        esi, dword ptr [0x4ce8d0]
00427bc7  mov        dword ptr [eax + 0xc0], esi
00427bcd  mov        esi, dword ptr [0x4ce8d4]
00427bd3  mov        dword ptr [eax + 0xc4], esi
00427bd9  mov        esi, dword ptr [0x4ce8d8]
00427bdf  mov        dword ptr [eax + 0xc8], esi
00427be5  movzx      esi, byte ptr [esp + 0xc]
00427bea  mov        dword ptr [eax + 0xe4], esi
00427bf0  mov        esi, dword ptr [edx]
00427bf2  mov        dword ptr [eax + 0x9c], esi
00427bf8  mov        esi, dword ptr [edx + 4]
00427bfb  mov        dword ptr [eax + 0xa0], esi
00427c01  mov        edx, dword ptr [edx + 8]
00427c04  mov        dword ptr [eax + 0xa4], edx
00427c0a  mov        edx, dword ptr [ecx]
00427c0c  mov        dword ptr [eax + 0xa8], edx
00427c12  mov        edx, dword ptr [ecx + 4]
00427c15  mov        dword ptr [eax + 0xac], edx
00427c1b  mov        ecx, dword ptr [ecx + 8]
00427c1e  mov        dword ptr [eax + 0xb0], ecx
00427c24  mov        ecx, dword ptr [eax + 0xdc]
00427c2a  xor        edx, edx
00427c2c  pop        esi
00427c2d  test       cl, 1
00427c30  jne        0x427c5b

// block 00427c32
00427c32  or         ecx, 1
00427c35  fst        dword ptr [eax + 0xd4]
00427c3b  mov        dword ptr [eax + 0xd0], edx
00427c41  mov        dword ptr [eax + 0xcc], 0xfff0bdc1
00427c4b  mov        dword ptr [eax + 0xd8], 0x4b2ed0
00427c55  mov        dword ptr [eax + 0xdc], ecx

// block 00427c5b
00427c5b  fstp       dword ptr [eax + 0xd4]
00427c61  mov        dword ptr [eax + 0xd0], edx
00427c67  mov        dword ptr [eax + 0xcc], 0xffffffff
00427c71  ret        8
