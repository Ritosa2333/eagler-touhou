
// block 00472b81
00472b81  call       0x475467

// block 00472b86
00472b86  mov        eax, dword ptr [eax + 0x7c]
00472b89  test       eax, eax
00472b8b  je         0x472b8f

// block 00472b8d
00472b8d  call       eax

// block 00472b8f
00472b8f  jmp        0x472b48
