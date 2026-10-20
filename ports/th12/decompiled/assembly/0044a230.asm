
// block 0044a230
0044a230  push       esi
0044a231  push       0x6c
0044a233  call       0x46c9ea

// block 0044a238
0044a238  mov        esi, eax
0044a23a  add        esp, 4
0044a23d  test       esi, esi
0044a23f  je         0x44a25d

// block 0044a241
0044a241  and        dword ptr [esi + 0x20], 0xfffffffe
0044a245  push       0x6c
0044a247  push       0
0044a249  push       esi
0044a24a  call       0x477420

// block 0044a24f
0044a24f  add        esp, 0xc
0044a252  or         dword ptr [esi], 2
0044a255  mov        dword ptr [0x4b4534], esi
0044a25b  jmp        0x44a25f

// block 0044a25d
0044a25d  xor        esi, esi

// block 0044a25f
0044a25f  push       edi
0044a260  mov        edi, esi
0044a262  call       0x449fa0

// block 0044a267
0044a267  pop        edi
0044a268  test       eax, eax
0044a26a  je         0x44a286

// block 0044a26c
0044a26c  test       esi, esi
0044a26e  je         0x44a282

// block 0044a270
0044a270  push       ebx
0044a271  mov        ebx, esi
0044a273  call       0x44a120

// block 0044a278
0044a278  push       esi
0044a279  call       0x46ca4f

// block 0044a27e
0044a27e  add        esp, 4
0044a281  pop        ebx

// block 0044a282
0044a282  xor        eax, eax
0044a284  pop        esi
0044a285  ret        

// block 0044a286
0044a286  mov        eax, esi
0044a288  pop        esi
0044a289  ret        
