
// block 00472b48
00472b48  push       8
00472b4a  push       0x4aabe0
00472b4f  call       0x46fcec

// block 00472b54
00472b54  call       0x475467

// block 00472b59
00472b59  mov        eax, dword ptr [eax + 0x78]
00472b5c  test       eax, eax
00472b5e  je         0x472b76

// block 00472b60
00472b60  and        dword ptr [ebp - 4], 0
00472b64  call       eax

// block 00472b66
00472b66  jmp        0x472b6f

// block 00472b6f
00472b6f  mov        dword ptr [ebp - 4], 0xfffffffe

// block 00472b76
00472b76  call       0x479ff3
