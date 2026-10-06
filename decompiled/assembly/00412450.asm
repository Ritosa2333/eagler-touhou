
// block 00412450
00412450  mov        eax, dword ptr [esp + 4]
00412454  push       ebx
00412455  push       esi
00412456  mov        ebx, ecx
00412458  push       edi
00412459  mov        byte ptr [0x4d4f38], 0
00412460  mov        ecx, eax

// block 00412462
00412462  mov        dl, byte ptr [eax]
00412464  inc        eax
00412465  test       dl, dl
00412467  jne        0x412462

// block 00412469
00412469  mov        edi, 0x4d4f38
0041246e  sub        eax, ecx
00412470  mov        esi, ecx
00412472  dec        edi

// block 00412473
00412473  mov        cl, byte ptr [edi + 1]
00412476  inc        edi
00412477  test       cl, cl
00412479  jne        0x412473

// block 0041247b
0041247b  mov        ecx, eax
0041247d  shr        ecx, 2

// block 00412480
00412480  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00412482
00412482  mov        ecx, eax
00412484  push       0
00412486  and        ecx, 3
00412489  push       0
0041248b  mov        eax, 0x4d4f38

// block 00412490
00412490  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 00412492
00412492  call       0x463c10

// block 00412497
00412497  push       eax
00412498  mov        ecx, ebx
0041249a  call       0x4692e0

// block 0041249f
0041249f  xor        ecx, ecx
004124a1  test       eax, eax
004124a3  setge      cl
004124a6  pop        edi
004124a7  pop        esi
004124a8  pop        ebx
004124a9  dec        ecx
004124aa  mov        eax, ecx
004124ac  ret        4
