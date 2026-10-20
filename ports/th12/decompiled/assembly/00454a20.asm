
// block 00454a20
00454a20  fld        dword ptr [esp + 4]
00454a24  mov        dword ptr [esi + 0x1c], 1
00454a2b  fmul       qword ptr [0x4a3d20]
00454a31  call       0x4931e0

// block 00454a36
00454a36  mov        dword ptr [esi + 0x14], eax
00454a39  mov        dword ptr [esi + 0x18], eax
00454a3c  ret        4
