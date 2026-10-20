
// block 00408750
00408750  fld        dword ptr [eax]
00408752  push       ecx
00408753  fadd       dword ptr [esp + 8]
00408757  fstp       dword ptr [esp + 8]
0040875b  fld        dword ptr [esp + 8]
0040875f  fstp       dword ptr [esp]
00408762  call       0x4646e0

// block 00408767
00408767  fstp       dword ptr [esi]
00408769  mov        eax, esi
0040876b  ret        4
