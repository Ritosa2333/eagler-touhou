
// block 00461bc0
00461bc0  mov        eax, dword ptr [esp + 4]
00461bc4  push       eax
00461bc5  call       0x461920

// block 00461bca
00461bca  test       eax, eax
00461bcc  je         0x461bd6

// block 00461bce
00461bce  add        eax, 0x430
00461bd3  ret        4

// block 00461bd6
00461bd6  xor        eax, eax
00461bd8  ret        4
