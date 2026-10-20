
// block 0049124d
0049124d  mov        eax, dword ptr [ebp - 0x14]
00491250  mov        eax, dword ptr [eax]
00491252  mov        eax, dword ptr [eax]
00491254  cmp        eax, 0xc0000005
00491259  je         0x491265

// block 0049125b
0049125b  cmp        eax, 0xc000001d
00491260  je         0x491265

// block 00491262
00491262  xor        eax, eax
00491264  ret        

// block 00491265
00491265  xor        eax, eax
00491267  inc        eax
00491268  ret        
