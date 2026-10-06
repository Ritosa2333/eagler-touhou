
// block 00427ca0
00427ca0  fld        dword ptr [esp + 4]
00427ca4  fmul       qword ptr [0x4a3ce8]
00427caa  call       0x4931e0

// block 00427caf
00427caf  add        dword ptr [esi + 0x38], eax
00427cb2  mov        ecx, dword ptr [esi + 0x38]
00427cb5  mov        eax, dword ptr [esi + 0xb4]
00427cbb  cmp        ecx, eax
00427cbd  jle        0x427cc2

// block 00427cbf
00427cbf  mov        dword ptr [esi + 0x38], eax

// block 00427cc2
00427cc2  ret        4
