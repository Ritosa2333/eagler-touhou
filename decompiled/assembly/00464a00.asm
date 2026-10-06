
// block 00464a00
00464a00  mov        eax, dword ptr [ecx + 0xc]
00464a03  test       eax, eax
00464a05  je         0x464a0a

// block 00464a07
00464a07  dec        dword ptr [eax + 4]

// block 00464a0a
00464a0a  mov        dword ptr [ecx + 0xc], 0x4b2ed0
00464a11  ret        
