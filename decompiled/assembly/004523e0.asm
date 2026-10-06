
// block 004523e0
004523e0  push       ecx
004523e1  cmp        dword ptr [0x4ce55c], 0
004523e8  push       esi
004523e9  mov        esi, ecx
004523eb  je         0x4523f5

// block 004523ed
004523ed  mov        eax, 7
004523f2  pop        esi
004523f3  pop        ecx
004523f4  ret        

// block 004523f5
004523f5  push       edi
004523f6  mov        edi, dword ptr [esi + 0x1c]
004523f9  mov        dword ptr [esp + 8], edi
004523fd  test       edi, edi
004523ff  je         0x452431

// block 00452401
00452401  cmp        dword ptr [esi + 0x34], edi
00452404  jge        0x45241d

// block 00452406
00452406  fld        dword ptr [esi + 0x38]
00452409  fmul       qword ptr [0x4a3ed0]
0045240f  fidiv      dword ptr [esp + 8]
00452413  call       0x4931e0

// block 00452418
00452418  mov        dword ptr [esi + 0x18], eax
0045241b  jmp        0x452424

// block 0045241d
0045241d  mov        dword ptr [esi + 0x18], 0xff

// block 00452424
00452424  cmp        dword ptr [esi + 0x18], 0
00452428  jge        0x452431

// block 0045242a
0045242a  mov        dword ptr [esi + 0x18], 0

// block 00452431
00452431  add        edi, 2
00452434  cmp        dword ptr [esi + 0x34], edi
00452437  pop        edi
00452438  jge        0x4523ed

// block 0045243a
0045243a  mov        eax, dword ptr [0x4b44e8]
0045243f  test       eax, eax
00452441  je         0x452452

// block 00452443
00452443  mov        eax, dword ptr [eax + 0x60]
00452446  mov        ecx, eax
00452448  shr        ecx, 2
0045244b  or         ecx, eax
0045244d  test       cl, 1
00452450  jne        0x45245a

// block 00452452
00452452  add        esi, 0x30
00452455  call       0x464a80

// block 0045245a
0045245a  mov        eax, 1
0045245f  pop        esi
00452460  pop        ecx
00452461  ret        
