
// block 0047411d
0047411d  mov        edi, edi
0047411f  push       esi
00474120  mov        esi, ecx
00474122  test       esi, esi
00474124  je         0x474141

// block 00474126
00474126  test       eax, eax
00474128  je         0x474141

// block 0047412a
0047412a  cmp        eax, esi
0047412c  je         0x474141

// block 0047412e
0047412e  push       edi
0047412f  push       0x36
00474131  pop        ecx
00474132  mov        edi, eax

// block 00474134
00474134  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00474136
00474136  and        dword ptr [eax], 0
00474139  push       eax
0047413a  call       0x473ff5

// block 0047413f
0047413f  pop        ecx
00474140  pop        edi

// block 00474141
00474141  pop        esi
00474142  ret        
