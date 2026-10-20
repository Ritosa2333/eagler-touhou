
// block 00473e82
00473e82  cmp        dword ptr [0x4d6434], 0
00473e89  jne        0x473e9d

// block 00473e8b
00473e8b  push       -3
00473e8d  call       0x473ce8

// block 00473e92
00473e92  pop        ecx
00473e93  mov        dword ptr [0x4d6434], 1

// block 00473e9d
00473e9d  xor        eax, eax
00473e9f  ret        
