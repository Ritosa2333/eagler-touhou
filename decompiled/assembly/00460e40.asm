
// block 00460e40
00460e40  fldz       
00460e42  mov        eax, dword ptr [0x4ce8cc]
00460e47  fst        dword ptr [0x4cee04]
00460e4d  push       edi
00460e4e  fst        dword ptr [0x4cee08]
00460e54  mov        edi, ecx
00460e56  fst        dword ptr [eax + 0xb0]
00460e5c  fstp       dword ptr [eax + 0xb4]
00460e62  mov        eax, 0x14
00460e67  call       0x4611d0

// block 00460e6c
00460e6c  pop        edi
00460e6d  ret        
