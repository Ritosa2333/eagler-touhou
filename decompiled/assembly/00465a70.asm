
// block 00465a70
00465a70  cmp        dword ptr [esi + 0x78], 1
00465a74  jne        0x465a8d

// block 00465a76
00465a76  mov        eax, dword ptr [esi + 0x8c]
00465a7c  push       eax
00465a7d  call       dword ptr [0x4980a4]

// block 00465a83
00465a83  mov        dword ptr [esi + 0x8c], 0xffffffff

// block 00465a8d
00465a8d  push       esi
00465a8e  call       0x46ca4f

// block 00465a93
00465a93  add        esp, 4
00465a96  mov        eax, esi
00465a98  ret        
