
// block 00460f00
00460f00  push       esi
00460f01  push       edi
00460f02  mov        edi, 0x4cec04
00460f07  mov        esi, ecx
00460f09  mov        dword ptr [0x4cee34], edi
00460f0f  call       0x430910

// block 00460f14
00460f14  mov        edx, dword ptr [0x4cee34]
00460f1a  mov        eax, dword ptr [0x4ce8f0]
00460f1f  mov        ecx, dword ptr [eax]
00460f21  add        edx, 0xcc
