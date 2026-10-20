
// block 00406630
00406630  fld        dword ptr [esi]
00406632  push       ecx
00406633  fsub       dword ptr [esp + 8]
00406637  fstp       dword ptr [esp + 8]
0040663b  fld        dword ptr [esp + 8]
0040663f  fstp       dword ptr [esp]
00406642  call       0x4646e0

// block 00406647
00406647  fstp       dword ptr [esi]
00406649  mov        eax, esi
0040664b  ret        4
