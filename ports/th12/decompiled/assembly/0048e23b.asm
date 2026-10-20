
// block 0048e23b
0048e23b  mov        edi, edi
0048e23d  push       ebp
0048e23e  mov        ebp, esp
0048e240  sub        esp, 0x10
0048e243  push       ebx
0048e244  push       dword ptr [ebp + 0x14]
0048e247  lea        ecx, [ebp - 0x10]
0048e24a  call       0x46d3b4

// block 0048e24f
0048e24f  mov        ecx, dword ptr [ebp + 0x10]
0048e252  xor        ebx, ebx
0048e254  cmp        ecx, ebx
0048e256  jne        0x48e26b

// block 0048e258
0048e258  cmp        byte ptr [ebp - 4], bl
0048e25b  je         0x48e264

// block 0048e25d
0048e25d  mov        eax, dword ptr [ebp - 8]
0048e260  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048e264
0048e264  xor        eax, eax
0048e266  jmp        0x48e326

// block 0048e26b
0048e26b  cmp        dword ptr [ebp + 8], ebx
0048e26e  jne        0x48e29e

// block 0048e270
0048e270  call       0x46e96f

// block 0048e275
0048e275  push       ebx
0048e276  push       ebx
0048e277  push       ebx
0048e278  push       ebx
0048e279  push       ebx
0048e27a  mov        dword ptr [eax], 0x16
0048e280  call       0x470f2d

// block 0048e285
0048e285  add        esp, 0x14
0048e288  cmp        byte ptr [ebp - 4], bl
0048e28b  je         0x48e294

// block 0048e28d
0048e28d  mov        eax, dword ptr [ebp - 8]
0048e290  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048e294
0048e294  mov        eax, 0x7fffffff
0048e299  jmp        0x48e326

// block 0048e29e
0048e29e  cmp        dword ptr [ebp + 0xc], ebx
0048e2a1  je         0x48e270

// block 0048e2a3
0048e2a3  push       esi
0048e2a4  mov        esi, 0x7fffffff
0048e2a9  cmp        ecx, esi
0048e2ab  jbe        0x48e2c7

// block 0048e2ad
0048e2ad  call       0x46e96f

// block 0048e2b2
0048e2b2  push       ebx
0048e2b3  push       ebx
0048e2b4  push       ebx
0048e2b5  push       ebx
0048e2b6  push       ebx
0048e2b7  mov        dword ptr [eax], 0x16
0048e2bd  call       0x470f2d

// block 0048e2c2
0048e2c2  add        esp, 0x14
0048e2c5  jmp        0x48e306

// block 0048e2c7
0048e2c7  mov        eax, dword ptr [ebp - 0xc]
0048e2ca  cmp        dword ptr [eax + 8], ebx
0048e2cd  jne        0x48e2e3

// block 0048e2cf
0048e2cf  push       dword ptr [ebp + 0x14]
0048e2d2  push       ecx
0048e2d3  push       dword ptr [ebp + 0xc]
0048e2d6  push       dword ptr [ebp + 8]
0048e2d9  call       0x490bac

// block 0048e2de
0048e2de  add        esp, 0x10
0048e2e1  jmp        0x48e319

// block 0048e2e3
0048e2e3  push       dword ptr [eax + 4]
0048e2e6  push       ecx
0048e2e7  push       dword ptr [ebp + 0xc]
0048e2ea  push       ecx
0048e2eb  push       dword ptr [ebp + 8]
0048e2ee  push       0x1001
0048e2f3  push       dword ptr [eax + 0xc]
0048e2f6  lea        eax, [ebp - 0x10]
0048e2f9  push       eax
0048e2fa  call       0x490b6a

// block 0048e2ff
0048e2ff  add        esp, 0x20
0048e302  cmp        eax, ebx
0048e304  jne        0x48e316

// block 0048e306
0048e306  cmp        byte ptr [ebp - 4], bl
0048e309  je         0x48e312

// block 0048e30b
0048e30b  mov        eax, dword ptr [ebp - 8]
0048e30e  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048e312
0048e312  mov        eax, esi
0048e314  jmp        0x48e325

// block 0048e316
0048e316  add        eax, -2

// block 0048e319
0048e319  cmp        byte ptr [ebp - 4], bl
0048e31c  je         0x48e325

// block 0048e31e
0048e31e  mov        ecx, dword ptr [ebp - 8]
0048e321  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0048e325
0048e325  pop        esi

// block 0048e326
0048e326  pop        ebx
0048e327  leave      
0048e328  ret        
