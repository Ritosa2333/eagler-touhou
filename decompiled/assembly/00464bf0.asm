
// block 00464bf0
00464bf0  push       esi
00464bf1  mov        esi, ecx
00464bf3  mov        dword ptr [esi], 0x4a3738
00464bf9  call       0x464c40

// block 00464bfe
00464bfe  test       byte ptr [esp + 8], 1
00464c03  je         0x464c0e

// block 00464c05
00464c05  push       esi
00464c06  call       0x46ca4f

// block 00464c0b
00464c0b  add        esp, 4

// block 00464c0e
00464c0e  mov        eax, esi
00464c10  pop        esi
00464c11  ret        4
