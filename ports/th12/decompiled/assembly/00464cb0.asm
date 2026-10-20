
// block 00464cb0
00464cb0  push       esi
00464cb1  mov        esi, eax
00464cb3  call       0x464c40

// block 00464cb8
00464cb8  mov        ecx, dword ptr [esp + 8]
00464cbc  lea        eax, [esi + 8]
00464cbf  push       eax
00464cc0  push       0
00464cc2  push       ecx
00464cc3  push       edi
00464cc4  push       0
00464cc6  push       0
00464cc8  mov        dword ptr [esi + 0x18], edi
00464ccb  mov        dword ptr [esi + 0x10], 1
00464cd2  mov        dword ptr [esi + 0xc], 0
00464cd9  call       0x46e54b

// block 00464cde
00464cde  add        esp, 0x18
00464ce1  mov        dword ptr [esi + 4], eax
00464ce4  pop        esi
00464ce5  ret        4
