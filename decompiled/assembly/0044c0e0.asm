
// block 0044c0e0
0044c0e0  test       byte ptr [esp + 4], 1
0044c0e5  push       esi
0044c0e6  mov        esi, ecx
0044c0e8  mov        dword ptr [esi], 0x4a2304
0044c0ee  je         0x44c0f9

// block 0044c0f0
0044c0f0  push       esi
0044c0f1  call       0x46ca4f

// block 0044c0f6
0044c0f6  add        esp, 4

// block 0044c0f9
0044c0f9  mov        eax, esi
0044c0fb  pop        esi
0044c0fc  ret        4
