
// block 0044bd70
0044bd70  push       esi
0044bd71  mov        esi, ecx
0044bd73  mov        eax, dword ptr [esi + 4]
0044bd76  mov        dword ptr [esi], 0x4a22dc
0044bd7c  cmp        eax, -1
0044bd7f  je         0x44bd96

// block 0044bd81
0044bd81  push       eax
0044bd82  call       dword ptr [0x4980a4]

// block 0044bd88
0044bd88  mov        dword ptr [esi + 4], 0xffffffff
0044bd8f  mov        dword ptr [esi + 8], 0

// block 0044bd96
0044bd96  mov        dword ptr [esi], 0x4a2304
0044bd9c  pop        esi
0044bd9d  ret        
