
// block 00461dd0
00461dd0  mov        eax, dword ptr [eax]
00461dd2  mov        edx, dword ptr [0x4ce8cc]
00461dd8  push       eax
00461dd9  call       0x461920

// block 00461dde
00461dde  test       eax, eax
00461de0  je         0x461dfc

// block 00461de2
00461de2  mov        ecx, dword ptr [esi]
00461de4  mov        dword ptr [eax + 0x430], ecx
00461dea  mov        edx, dword ptr [esi + 4]
00461ded  mov        dword ptr [eax + 0x434], edx
00461df3  mov        ecx, dword ptr [esi + 8]
00461df6  mov        dword ptr [eax + 0x438], ecx

// block 00461dfc
00461dfc  ret        
