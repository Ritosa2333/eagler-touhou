
// block 00472b94
00472b94  push       8
00472b96  push       0x4aac00
00472b9b  call       0x46fcec

// block 00472ba0
00472ba0  push       dword ptr [0x4b3d6c]
00472ba6  call       0x4751de

// block 00472bab
00472bab  pop        ecx
00472bac  test       eax, eax
00472bae  je         0x472bc6

// block 00472bb0
00472bb0  and        dword ptr [ebp - 4], 0
00472bb4  call       eax

// block 00472bb6
00472bb6  jmp        0x472bbf

// block 00472bbf
00472bbf  mov        dword ptr [ebp - 4], 0xfffffffe

// block 00472bc6
00472bc6  call       0x472b48
