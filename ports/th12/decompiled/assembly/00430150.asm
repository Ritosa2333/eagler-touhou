
// block 00430150
00430150  test       byte ptr [0x4ceae8], 0x10
00430157  push       edi
00430158  je         0x43016d

// block 0043015a
0043015a  push       0
0043015c  push       4
0043015e  mov        edi, 0x4a0a20
00430163  mov        eax, 0x4cf4e8
00430168  call       0x454960

// block 0043016d
0043016d  mov        eax, dword ptr [esp + 8]
00430171  push       eax
00430172  push       2
00430174  mov        edi, 0x4a0a20
00430179  mov        eax, 0x4cf4e8
0043017e  call       0x454960

// block 00430183
00430183  mov        ecx, dword ptr [0x4b451c]
00430189  mov        edx, dword ptr [esp + 0xc]
0043018d  mov        byte ptr [ecx + edx + 0x1e9da], 1
00430195  xor        eax, eax
00430197  pop        edi
00430198  ret        8
