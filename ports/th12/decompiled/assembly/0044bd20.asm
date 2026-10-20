
// block 0044bd20
0044bd20  push       esi
0044bd21  mov        esi, ecx
0044bd23  mov        eax, dword ptr [esi + 4]
0044bd26  mov        dword ptr [esi], 0x4a22dc
0044bd2c  cmp        eax, -1
0044bd2f  je         0x44bd46

// block 0044bd31
0044bd31  push       eax
0044bd32  call       dword ptr [0x4980a4]

// block 0044bd38
0044bd38  mov        dword ptr [esi + 4], 0xffffffff
0044bd3f  mov        dword ptr [esi + 8], 0

// block 0044bd46
0044bd46  test       byte ptr [esp + 8], 1
0044bd4b  mov        dword ptr [esi], 0x4a2304
0044bd51  je         0x44bd5c

// block 0044bd53
0044bd53  push       esi
0044bd54  call       0x46ca4f

// block 0044bd59
0044bd59  add        esp, 4

// block 0044bd5c
0044bd5c  mov        eax, esi
0044bd5e  pop        esi
0044bd5f  ret        4
