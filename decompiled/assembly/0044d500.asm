
// block 0044d500
0044d500  mov        eax, dword ptr [esi]
0044d502  test       eax, eax
0044d504  je         0x44d513

// block 0044d506
0044d506  push       eax
0044d507  call       dword ptr [0x498030]

// block 0044d50d
0044d50d  mov        dword ptr [esi], 0

// block 0044d513
0044d513  cmp        dword ptr [esi + 0x24], 0x10
0044d517  jb         0x44d51e

// block 0044d519
0044d519  mov        eax, dword ptr [esi + 0x10]
0044d51c  jmp        0x44d521

// block 0044d51e
0044d51e  lea        eax, [esi + 0x10]
