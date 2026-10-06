
// block 00425890
00425890  push       0x425900
00425895  push       0x4258d0
0042589a  push       0xa68
0042589f  push       0x9d8
004258a4  lea        eax, [esi + 0x14]
004258a7  push       eax
004258a8  call       0x46ce49

// block 004258ad
004258ad  push       0x666fe4
004258b2  push       0
004258b4  push       esi
004258b5  call       0x477420

// block 004258ba
004258ba  or         dword ptr [esi], 2
004258bd  add        esp, 0xc
004258c0  mov        dword ptr [0x4b44f0], esi
004258c6  mov        eax, esi
004258c8  ret        
