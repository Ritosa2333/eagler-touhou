
// block 0047b2cf
0047b2cf  mov        edi, edi
0047b2d1  push       esi
0047b2d2  push       edi
0047b2d3  mov        esi, 0x4d6320

// block 0047b2d8
0047b2d8  mov        edi, dword ptr [esi]
0047b2da  test       edi, edi
0047b2dc  je         0x47b30f

// block 0047b2de
0047b2de  lea        eax, [edi + 0x800]
0047b2e4  jmp        0x47b300

// block 0047b2e6
0047b2e6  cmp        dword ptr [edi + 8], 0
0047b2ea  je         0x47b2f6

// block 0047b2ec
0047b2ec  lea        eax, [edi + 0xc]
0047b2ef  push       eax
0047b2f0  call       dword ptr [0x498094]

// block 0047b2f6
0047b2f6  mov        eax, dword ptr [esi]
0047b2f8  add        edi, 0x40
0047b2fb  add        eax, 0x800

// block 0047b300
0047b300  cmp        edi, eax
0047b302  jb         0x47b2e6

// block 0047b304
0047b304  push       dword ptr [esi]
0047b306  call       0x46c941

// block 0047b30b
0047b30b  and        dword ptr [esi], 0
0047b30e  pop        ecx

// block 0047b30f
0047b30f  add        esi, 4
0047b312  cmp        esi, 0x4d6420
0047b318  jl         0x47b2d8

// block 0047b31a
0047b31a  pop        edi
0047b31b  pop        esi
0047b31c  ret        
