
// block 00406de0
00406de0  push       ecx
00406de1  mov        ecx, dword ptr [0x4b43c4]
00406de7  xor        eax, eax
00406de9  push       esi
00406dea  cmp        dword ptr [ecx + 0x3c], eax
00406ded  je         0x406e35

// block 00406def
00406def  mov        edx, dword ptr [0x4b0c94]
00406df5  mov        esi, dword ptr [0x4b0c90]
00406dfb  lea        edx, [edx + esi*2]
00406dfe  cmp        edx, 5
00406e01  ja         0x406e35

// block 00406e03
00406e03  jmp        dword ptr [edx*4 + 0x406e3c]

// block 00406e0a
00406e0a  mov        esi, dword ptr [esp + 0xc]
00406e0e  mov        eax, edi
00406e10  call       0x4073b0

// block 00406e15
00406e15  pop        esi
00406e16  pop        ecx
00406e17  ret        4

// block 00406e1a
00406e1a  push       edi
00406e1b  mov        edx, ecx
00406e1d  call       0x4079d0

// block 00406e22
00406e22  pop        esi
00406e23  pop        ecx
00406e24  ret        4

// block 00406e27
00406e27  mov        eax, edi
00406e29  call       0x408c10

// block 00406e2e
00406e2e  pop        esi
00406e2f  pop        ecx
00406e30  ret        4

// block 00406e33
00406e33  xor        eax, eax

// block 00406e35
00406e35  pop        esi
00406e36  pop        ecx
00406e37  ret        4
