
// block 00464cf0
00464cf0  push       esi
00464cf1  mov        esi, eax
00464cf3  call       0x464c40

// block 00464cf8
00464cf8  mov        ecx, dword ptr [esp + 8]
00464cfc  lea        eax, [esi + 8]
00464cff  push       eax
00464d00  push       4
00464d02  push       ecx
00464d03  push       edi
00464d04  push       0
00464d06  push       0
00464d08  mov        dword ptr [esi + 0x18], edi
00464d0b  mov        dword ptr [esi + 0x10], 1
00464d12  mov        dword ptr [esi + 0xc], 0
00464d19  call       0x46e54b

// block 00464d1e
00464d1e  add        esp, 0x18
00464d21  mov        dword ptr [esi + 4], eax
00464d24  pop        esi
00464d25  ret        4
