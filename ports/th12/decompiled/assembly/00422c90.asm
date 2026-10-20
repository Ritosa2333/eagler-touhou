
// block 00422c90
00422c90  mov        byte ptr [0x4d4f38], 0
00422c97  mov        ecx, eax
00422c99  lea        esp, [esp]

// block 00422ca0
00422ca0  mov        dl, byte ptr [eax]
00422ca2  inc        eax
00422ca3  test       dl, dl
00422ca5  jne        0x422ca0

// block 00422ca7
00422ca7  push       esi
00422ca8  push       edi
00422ca9  mov        edi, 0x4d4f38
00422cae  sub        eax, ecx
00422cb0  mov        esi, ecx
00422cb2  dec        edi

// block 00422cb3
00422cb3  mov        cl, byte ptr [edi + 1]
00422cb6  inc        edi
00422cb7  test       cl, cl
00422cb9  jne        0x422cb3

// block 00422cbb
00422cbb  mov        ecx, eax
00422cbd  shr        ecx, 2

// block 00422cc0
00422cc0  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00422cc2
00422cc2  mov        ecx, eax
00422cc4  and        ecx, 3

// block 00422cc7
00422cc7  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 00422cc9
00422cc9  pop        edi
00422cca  mov        eax, 0x4d4f38
00422ccf  pop        esi
00422cd0  ret        
