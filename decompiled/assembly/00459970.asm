
// block 00459970
00459970  mov        ecx, dword ptr [eax + 0x20]
00459973  fldz       
00459975  xor        edx, edx
00459977  test       cl, 1
0045997a  jne        0x459996

// block 0045997c
0045997c  or         ecx, 1
0045997f  fst        dword ptr [eax + 0x18]
00459982  mov        dword ptr [eax + 0x14], edx
00459985  mov        dword ptr [eax + 0x10], 0xfff0bdc1
0045998c  mov        dword ptr [eax + 0x1c], 0x4b2ed0
00459993  mov        dword ptr [eax + 0x20], ecx

// block 00459996
00459996  fstp       dword ptr [eax + 0x18]
00459999  mov        dword ptr [eax + 0x14], edx
0045999c  mov        dword ptr [eax + 0x10], 0xffffffff
004599a3  ret        
