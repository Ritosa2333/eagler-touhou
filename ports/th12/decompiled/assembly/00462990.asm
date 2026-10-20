
// block 00462990
00462990  push       esi
00462991  push       eax
00462992  call       0x4627e0

// block 00462997
00462997  mov        ecx, dword ptr [esp + 8]
0046299b  mov        esi, eax
0046299d  and        dword ptr [esi + 4], 0xfffffffd
004629a1  mov        dword ptr [esi + 0x20], ecx
004629a4  call       0x462420

// block 004629a9
004629a9  mov        eax, esi
004629ab  pop        esi
004629ac  ret        4
