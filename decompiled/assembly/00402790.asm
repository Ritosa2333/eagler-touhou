
// block 00402790
00402790  push       ebp
00402791  mov        ebp, dword ptr [esp + 8]
00402795  push       esi
00402796  push       edi
00402797  mov        edi, eax
00402799  sub        edi, 1
0040279c  mov        esi, ecx
0040279e  js         0x4027ab

// block 004027a0
004027a0  mov        ecx, esi
004027a2  call       ebx

// block 004027a4
004027a4  add        esi, ebp
004027a6  sub        edi, 1
004027a9  jns        0x4027a0

// block 004027ab
004027ab  pop        edi
004027ac  pop        esi
004027ad  pop        ebp
004027ae  ret        4
