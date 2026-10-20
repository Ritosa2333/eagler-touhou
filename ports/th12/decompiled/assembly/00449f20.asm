
// block 00449f20
00449f20  mov        ecx, dword ptr [0x4b43dc]
00449f26  xor        eax, eax
00449f28  add        ecx, 0x1c
00449f2b  jmp        0x449f30

// block 00449f30
00449f30  cmp        dword ptr [ecx], 0
00449f33  jne        0x449f41

// block 00449f35
00449f35  inc        eax
00449f36  add        ecx, 4
00449f39  cmp        eax, 8
00449f3c  jb         0x449f30

// block 00449f3e
00449f3e  xor        eax, eax
00449f40  ret        

// block 00449f41
00449f41  mov        eax, 1
00449f46  ret        
