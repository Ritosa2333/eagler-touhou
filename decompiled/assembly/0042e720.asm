
// block 0042e720
0042e720  fld        dword ptr [eax]
0042e722  push       ecx
0042e723  fdiv       dword ptr [esp + 8]
0042e727  fstp       dword ptr [esp + 8]
0042e72b  fld        dword ptr [esp + 8]
0042e72f  fstp       dword ptr [esp]
0042e732  call       0x4646e0

// block 0042e737
0042e737  fstp       dword ptr [esi]
0042e739  mov        eax, esi
0042e73b  ret        4
