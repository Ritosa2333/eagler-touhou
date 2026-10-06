
// block 00408950
00408950  fld        dword ptr [esi + 0x1c]
00408953  push       ecx
00408954  fadd       dword ptr [esp + 8]
00408958  fstp       dword ptr [esp + 8]
0040895c  fld        dword ptr [esp + 8]
00408960  fstp       dword ptr [esp]
00408963  call       0x4646e0

// block 00408968
00408968  fstp       dword ptr [esi + 0x1c]
0040896b  ret        4
