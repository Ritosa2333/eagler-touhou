
// block 00411d60
00411d60  mov        eax, dword ptr [eax + 4]
00411d63  fld        dword ptr [esp + 8]
00411d67  mov        ecx, dword ptr [eax + 4]
00411d6a  movzx      edx, word ptr [ecx + 8]
00411d6e  mov        ecx, dword ptr [esp + 4]
00411d72  push       esi
00411d73  mov        esi, 1
00411d78  shl        esi, cl
00411d7a  test       esi, edx
00411d7c  pop        esi
00411d7d  je         0x411d88

// block 00411d7f
00411d7f  push       ecx
00411d80  fstp       dword ptr [esp]
00411d83  call       0x468fe0

// block 00411d88
00411d88  fstp       dword ptr [esp + 8]
00411d8c  fld        dword ptr [esp + 8]
00411d90  ret        8
