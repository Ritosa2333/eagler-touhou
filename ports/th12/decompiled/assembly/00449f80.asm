
// block 00449f80
00449f80  and        dword ptr [esi + 0x20], 0xfffffffe
00449f84  push       0x6c
00449f86  push       0
00449f88  push       esi
00449f89  call       0x477420

// block 00449f8e
00449f8e  or         dword ptr [esi], 2
00449f91  add        esp, 0xc
00449f94  mov        dword ptr [0x4b4534], esi
00449f9a  mov        eax, esi
00449f9c  ret        
