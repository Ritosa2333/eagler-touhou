
// block 00422700
00422700  push       edi
00422701  push       0x78
00422703  call       0x46c9ea

// block 00422708
00422708  mov        edi, eax
0042270a  add        esp, 4
0042270d  test       edi, edi
0042270f  je         0x42272e

// block 00422711
00422711  and        dword ptr [edi + 0x20], 0xfffffffe
00422715  push       esi
00422716  lea        esi, [edi + 0x24]
00422719  call       0x423250

// block 0042271e
0042271e  push       0x78
00422720  push       0
00422722  push       edi
00422723  call       0x477420

// block 00422728
00422728  add        esp, 0xc
0042272b  pop        esi
0042272c  jmp        0x422730

// block 0042272e
0042272e  xor        edi, edi

// block 00422730
00422730  mov        eax, dword ptr [0x4ce8f0]
00422735  mov        dword ptr [0x4cf468], 0
0042273f  mov        ecx, dword ptr [eax]
00422741  mov        edx, dword ptr [ecx + 0x14]
00422744  push       eax
00422745  call       edx

// block 00422747
00422747  mov        eax, dword ptr [esp + 8]
0042274b  or         dword ptr [edi + 0x60], 4
0042274f  mov        dword ptr [0x4b44e8], edi
00422755  mov        dword ptr [edi + 0x74], eax
00422758  call       0x430500

// block 0042275d
0042275d  mov        eax, edi
0042275f  pop        edi
00422760  ret        4
