
// block 0047c22d
0047c22d  push       0xc
0047c22f  push       0x4aaeb8
0047c234  call       0x46fcec

// block 0047c239
0047c239  xor        eax, eax
0047c23b  xor        esi, esi
0047c23d  cmp        dword ptr [ebp + 8], esi
0047c240  setne      al
0047c243  cmp        eax, esi
0047c245  jne        0x47c264

// block 0047c247
0047c247  call       0x46e96f

// block 0047c24c
0047c24c  mov        dword ptr [eax], 0x16
0047c252  push       esi
0047c253  push       esi
0047c254  push       esi
0047c255  push       esi
0047c256  push       esi
0047c257  call       0x470f2d

// block 0047c25c
0047c25c  add        esp, 0x14
0047c25f  or         eax, 0xffffffff
0047c262  jmp        0x47c2c3

// block 0047c264
0047c264  call       0x47c025

// block 0047c269
0047c269  push       0x20
0047c26b  pop        ebx
0047c26c  add        eax, ebx
0047c26e  push       eax
0047c26f  push       1
0047c271  call       0x47c13d

// block 0047c276
0047c276  pop        ecx
0047c277  pop        ecx
0047c278  mov        dword ptr [ebp - 4], esi
0047c27b  call       0x47c025

// block 0047c280
0047c280  add        eax, ebx
0047c282  push       eax
0047c283  call       0x48d319

// block 0047c288
0047c288  pop        ecx
0047c289  mov        edi, eax
0047c28b  lea        eax, [ebp + 0xc]
0047c28e  push       eax
0047c28f  push       esi
0047c290  push       dword ptr [ebp + 8]
0047c293  call       0x47c025

// block 0047c298
0047c298  add        eax, ebx
0047c29a  push       eax
0047c29b  call       0x47021f

// block 0047c2a0
0047c2a0  mov        dword ptr [ebp - 0x1c], eax
0047c2a3  call       0x47c025

// block 0047c2a8
0047c2a8  add        eax, ebx
0047c2aa  push       eax
0047c2ab  push       edi
0047c2ac  call       0x48d3b5

// block 0047c2b1
0047c2b1  add        esp, 0x18
0047c2b4  mov        dword ptr [ebp - 4], 0xfffffffe
0047c2bb  call       0x47c2c9

// block 0047c2c0
0047c2c0  mov        eax, dword ptr [ebp - 0x1c]

// block 0047c2c3
0047c2c3  call       0x46fd31

// block 0047c2c8
0047c2c8  ret        
