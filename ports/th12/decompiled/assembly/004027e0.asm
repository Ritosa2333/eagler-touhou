
// block 004027e0
004027e0  mov        eax, 0xfffffffe
004027e5  push       esi
004027e6  mov        esi, ecx
004027e8  and        dword ptr [esi + 0x78], eax
004027eb  and        dword ptr [esi + 0xdc], eax
004027f1  and        dword ptr [esi + 0x128], eax
004027f7  and        dword ptr [esi + 0x154], eax
004027fd  and        dword ptr [esi + 0x1a0], eax
00402803  and        dword ptr [esi + 0x1dc], eax
00402809  and        dword ptr [esi + 0x218], eax
0040280f  and        dword ptr [esi + 0x264], eax
00402815  and        dword ptr [esi + 0x290], eax
0040281b  and        dword ptr [esi + 0x2bc], eax
00402821  and        dword ptr [esi + 0x2e8], eax
00402827  and        dword ptr [esi + 0x3d8], eax
0040282d  push       0x4b4
00402832  push       0
00402834  push       esi
00402835  call       0x477420

// block 0040283a
0040283a  or         eax, 0xffffffff
0040283d  mov        word ptr [esi + 0x3e4], ax
00402844  add        esp, 0xc
00402847  mov        eax, esi
00402849  pop        esi
0040284a  ret        
