
// block 00460f50
00460f50  push       esi
00460f51  push       edi
00460f52  mov        edi, 0x4ceaec
00460f57  mov        esi, ecx
00460f59  mov        dword ptr [0x4cee34], edi
00460f5f  call       0x430910

// block 00460f64
00460f64  mov        edx, dword ptr [0x4cee34]
00460f6a  mov        eax, dword ptr [0x4ce8f0]
00460f6f  mov        ecx, dword ptr [eax]
00460f71  add        edx, 0xcc
