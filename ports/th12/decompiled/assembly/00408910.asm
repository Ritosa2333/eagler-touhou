
// block 00408910
00408910  fld        dword ptr [esp + 8]
00408914  push       ecx
00408915  fstp       dword ptr [esp]
00408918  call       0x4646e0

// block 0040891d
0040891d  push       ecx
0040891e  fstp       dword ptr [esp]
00408921  call       0x4646e0

// block 00408926
00408926  mov        eax, dword ptr [esp + 4]
0040892a  fstp       dword ptr [eax + 0x1c]
0040892d  ret        8
