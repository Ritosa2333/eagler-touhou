
// block 00452470
00452470  sub        esp, 0x10
00452473  fld        dword ptr [0x4a3d78]
00452479  push       ebx
0045247a  mov        ebx, dword ptr [ecx + 0x18]
0045247d  fstp       dword ptr [esp + 4]
00452481  fld        dword ptr [0x4a3e68]
00452487  shl        ebx, 0x18
0045248a  or         ebx, dword ptr [ecx + 0x20]
0045248d  fstp       dword ptr [esp + 8]
00452491  fld        dword ptr [0x4a3ebc]
00452497  push       edi
00452498  fstp       dword ptr [esp + 0x10]
0045249c  lea        edi, [esp + 8]
004524a0  fld        dword ptr [0x4a3eb8]
004524a6  fstp       dword ptr [esp + 0x14]
004524aa  call       0x451f30

// block 004524af
004524af  pop        edi
004524b0  mov        eax, 1
004524b5  pop        ebx
004524b6  add        esp, 0x10
004524b9  ret        
