
// block 00406050
00406050  push       ecx
00406051  fld        dword ptr [eax + 0x78]
00406054  fidiv      dword ptr [eax + 0x84]
0040605a  mov        eax, dword ptr [eax + 0x88]
00406060  dec        eax
00406061  fstp       dword ptr [esp]
00406064  fld        dword ptr [esp]
00406067  cmp        eax, 0xf
0040606a  ja         0x406075

// block 0040606c
0040606c  push       ecx
0040606d  fstp       dword ptr [esp]
00406070  call       0x464f80

// block 00406075
00406075  fstp       dword ptr [esp]
00406078  fld        dword ptr [esp]
0040607b  pop        ecx
0040607c  ret        
