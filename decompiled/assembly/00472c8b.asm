
// block 00472c8b
00472c8b  mov        edi, edi
00472c8d  push       ebp
00472c8e  mov        ebp, esp
00472c90  push       esi
00472c91  mov        esi, eax
00472c93  jmp        0x472ca0

// block 00472c95
00472c95  mov        eax, dword ptr [esi]
00472c97  test       eax, eax
00472c99  je         0x472c9d

// block 00472c9b
00472c9b  call       eax

// block 00472c9d
00472c9d  add        esi, 4

// block 00472ca0
00472ca0  cmp        esi, dword ptr [ebp + 8]
00472ca3  jb         0x472c95

// block 00472ca5
00472ca5  pop        esi
00472ca6  pop        ebp
00472ca7  ret        
