
// block 004db449
004db449  mov        ecx, 0xddde7560
004db44e  mov        esp, 0xd43f2f1a
004db453  das        
004db454  jne        0x4db439

// block 004db456
004db456  push       ss
004db457  imul       ebp, dword ptr [esi], 0x8deb4df
004db45d  int        0x98

// block 004db45f
004db45f  inc        ebx
004db460  xchg       edi, eax
004db461  cmp        esp, dword ptr [ecx - 0x759212c8]
004db467  cmp        bl, byte ptr [ebp + esi*2 - 0x1b]
004db46b  hlt        
