
// block 00406650
00406650  fld        dword ptr [esi]
00406652  push       ecx
00406653  fadd       dword ptr [esp + 8]
00406657  fstp       dword ptr [esp + 8]
0040665b  fld        dword ptr [esp + 8]
0040665f  fstp       dword ptr [esp]
00406662  call       0x4646e0

// block 00406667
00406667  fstp       dword ptr [esi]
00406669  mov        eax, esi
0040666b  ret        4
