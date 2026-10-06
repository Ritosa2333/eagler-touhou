
// block 00422fe0
00422fe0  push       ecx
00422fe1  push       esi
00422fe2  mov        esi, eax
00422fe4  mov        eax, dword ptr [esi]
00422fe6  mov        dword ptr [esi + 0x14], 0
00422fed  test       eax, eax
00422fef  je         0x423012

// block 00422ff1
00422ff1  mov        ecx, dword ptr [eax]
00422ff3  lea        edx, [esp + 4]
00422ff7  push       edx
00422ff8  push       eax
00422ff9  mov        eax, dword ptr [ecx + 0x24]
00422ffc  call       eax

// block 00422ffe
00422ffe  mov        ecx, dword ptr [esp + 4]
00423002  and        ecx, 1
00423005  mov        dword ptr [esi + 0x14], ecx
00423008  mov        esi, dword ptr [esi]
0042300a  mov        edx, dword ptr [esi]
0042300c  mov        eax, dword ptr [edx + 0x48]
0042300f  push       esi
00423010  call       eax

// block 00423012
00423012  pop        esi
00423013  pop        ecx
00423014  ret        
