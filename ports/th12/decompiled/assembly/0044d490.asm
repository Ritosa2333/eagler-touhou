
// block 0044d490
0044d490  mov        eax, dword ptr [esi]
0044d492  test       eax, eax
0044d494  je         0x44d4a3

// block 0044d496
0044d496  push       eax
0044d497  call       dword ptr [0x498030]

// block 0044d49d
0044d49d  mov        dword ptr [esi], 0

// block 0044d4a3
0044d4a3  cmp        dword ptr [esi + 0x24], 0x10
0044d4a7  jb         0x44d4b5

// block 0044d4a9
0044d4a9  mov        eax, dword ptr [esi + 0x10]
0044d4ac  push       eax
0044d4ad  call       0x46ca4f

// block 0044d4b2
0044d4b2  add        esp, 4

// block 0044d4b5
0044d4b5  mov        dword ptr [esi + 0x24], 0xf
0044d4bc  mov        dword ptr [esi + 0x20], 0
0044d4c3  mov        byte ptr [esi + 0x10], 0
0044d4c7  ret        
