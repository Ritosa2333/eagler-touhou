
// block 00454a40
00454a40  push       esi
00454a41  mov        esi, dword ptr [0x4d4754]
00454a47  test       esi, esi
00454a49  je         0x454a67

// block 00454a4b
00454a4b  fld        dword ptr [esp + 8]
00454a4f  mov        dword ptr [esi + 0x1c], 1
00454a56  fmul       qword ptr [0x4a3d20]
00454a5c  call       0x4931e0

// block 00454a61
00454a61  mov        dword ptr [esi + 0x14], eax
00454a64  mov        dword ptr [esi + 0x18], eax

// block 00454a67
00454a67  pop        esi
00454a68  ret        4
