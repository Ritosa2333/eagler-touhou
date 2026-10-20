
// block 00461b00
00461b00  mov        eax, dword ptr [esp + 4]
00461b04  mov        edx, dword ptr [0x4ce8cc]
00461b0a  push       eax
00461b0b  call       0x461920

// block 00461b10
00461b10  test       eax, eax
00461b12  je         0x461b2e

// block 00461b14
00461b14  mov        ecx, dword ptr [esi]
00461b16  mov        dword ptr [eax + 0x430], ecx
00461b1c  mov        edx, dword ptr [esi + 4]
00461b1f  mov        dword ptr [eax + 0x434], edx
00461b25  mov        ecx, dword ptr [esi + 8]
00461b28  mov        dword ptr [eax + 0x438], ecx

// block 00461b2e
00461b2e  ret        4
