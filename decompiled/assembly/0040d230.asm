
// block 0040d230
0040d230  push       ecx
0040d231  mov        eax, dword ptr [0x4b43cc]
0040d236  push       esi
0040d237  mov        esi, dword ptr [0x4b43c8]
0040d23d  add        esi, 0x64
0040d240  test       byte ptr [eax + 0x7c], 1
0040d244  push       edi
0040d245  je         0x40d254

// block 0040d247
0040d247  mov        eax, dword ptr [eax + 0x78]
0040d24a  cmp        eax, 0x60
0040d24d  jl         0x40d254

// block 0040d24f
0040d24f  cmp        eax, 0x63
0040d252  jle        0x40d28c

// block 0040d254
0040d254  mov        edi, 0x7d0
0040d259  lea        esp, [esp]

// block 0040d260
0040d260  movzx      eax, word ptr [esi + 0x532]
0040d267  test       ax, ax
0040d26a  je         0x40d281

// block 0040d26c
0040d26c  cmp        ax, 3
0040d270  je         0x40d281

// block 0040d272
0040d272  test       ebx, ebx
0040d274  je         0x40d27c

// block 0040d276
0040d276  cmp        dword ptr [esi + 4], 0
0040d27a  jne        0x40d281

// block 0040d27c
0040d27c  call       0x40c8b0

// block 0040d281
0040d281  add        esi, 0x9f8
0040d287  sub        edi, 1
0040d28a  jne        0x40d260

// block 0040d28c
0040d28c  pop        edi
0040d28d  pop        esi
0040d28e  pop        ecx
0040d28f  ret        
