
// block 0047f1b4
0047f1b4  mov        edi, edi
0047f1b6  push       ebp
0047f1b7  mov        ebp, esp
0047f1b9  mov        eax, dword ptr [0x4b4318]
0047f1be  sub        esp, 0x30
0047f1c1  push       ebx
0047f1c2  xor        ebx, ebx
0047f1c4  cmp        byte ptr [eax], bl
0047f1c6  je         0x47f2c7

// block 0047f1cc
0047f1cc  call       0x47d5e8

// block 0047f1d1
0047f1d1  cmp        eax, ebx
0047f1d3  mov        dword ptr [ebp - 4], eax
0047f1d6  jge        0x47f1db

// block 0047f1d8
0047f1d8  mov        dword ptr [ebp - 4], ebx

// block 0047f1db
0047f1db  cmp        dword ptr [ebp - 4], ebx
0047f1de  jne        0x47f1f1

// block 0047f1e0
0047f1e0  lea        eax, [ebp - 0x18]
0047f1e3  push       0x5d
0047f1e5  push       eax
0047f1e6  lea        eax, [ebp - 0x10]
0047f1e9  lea        ecx, [ebp - 8]
0047f1ec  jmp        0x47f305

// block 0047f1f1
0047f1f1  and        dword ptr [ebp - 0xc], 0xffff0000
0047f1f8  push       esi
0047f1f9  push       edi
0047f1fa  mov        edi, dword ptr [ebp + 0xc]
0047f1fd  mov        esi, 0x800
0047f202  mov        dword ptr [ebp - 0x10], ebx
0047f205  test       dword ptr [edi + 4], esi
0047f208  je         0x47f25b

// block 0047f20a
0047f20a  push       0x49db94
0047f20f  lea        ecx, [ebp - 0x10]
0047f212  call       0x47ea48

// block 0047f217
0047f217  jmp        0x47f25b

// block 0047f219
0047f219  mov        eax, dword ptr [ebp - 4]
0047f21c  dec        dword ptr [ebp - 4]
0047f21f  test       eax, eax
0047f221  je         0x47f261

// block 0047f223
0047f223  mov        eax, dword ptr [0x4b4318]
0047f228  cmp        byte ptr [eax], bl
0047f22a  je         0x47f261

// block 0047f22c
0047f22c  push       0x5d
0047f22e  lea        eax, [ebp - 0x20]
0047f231  push       eax
0047f232  lea        eax, [ebp - 0x28]
0047f235  push       ebx
0047f236  push       eax
0047f237  call       0x47ecb6

// block 0047f23c
0047f23c  push       eax
0047f23d  lea        eax, [ebp - 0x30]
0047f240  push       0x5b
0047f242  push       eax
0047f243  call       0x47ec02

// block 0047f248
0047f248  add        esp, 0x14
0047f24b  mov        ecx, eax
0047f24d  call       0x47ec6e

// block 0047f252
0047f252  push       eax
0047f253  lea        ecx, [ebp - 0x10]
0047f256  call       0x47e7bf

// block 0047f25b
0047f25b  cmp        byte ptr [ebp - 0xc], 1
0047f25f  jle        0x47f219

// block 0047f261
0047f261  cmp        dword ptr [edi], ebx
0047f263  je         0x47f2a4

// block 0047f265
0047f265  lea        eax, [ebp - 0x10]
0047f268  push       eax
0047f269  lea        eax, [ebp - 0x30]
0047f26c  push       eax
0047f26d  test       dword ptr [edi + 4], esi
0047f270  je         0x47f276

// block 0047f272
0047f272  mov        ecx, edi
0047f274  jmp        0x47f294

// block 0047f276
0047f276  push       0x29
0047f278  lea        eax, [ebp - 0x28]
0047f27b  push       eax
0047f27c  push       edi
0047f27d  lea        eax, [ebp - 0x20]
0047f280  push       0x28
0047f282  push       eax
0047f283  call       0x47ec02

// block 0047f288
0047f288  add        esp, 0xc
0047f28b  mov        ecx, eax
0047f28d  call       0x47ec6e

// block 0047f292
0047f292  mov        ecx, eax

// block 0047f294
0047f294  call       0x47e9ae

// block 0047f299
0047f299  mov        ecx, dword ptr [eax]
0047f29b  mov        dword ptr [ebp - 0x10], ecx
0047f29e  mov        eax, dword ptr [eax + 4]
0047f2a1  mov        dword ptr [ebp - 0xc], eax

// block 0047f2a4
0047f2a4  lea        eax, [ebp - 0x10]
0047f2a7  push       eax
0047f2a8  lea        eax, [ebp - 0x18]
0047f2ab  push       eax
0047f2ac  call       0x4826a7

// block 0047f2b1
0047f2b1  mov        eax, dword ptr [ebp + 8]
0047f2b4  mov        edx, dword ptr [ebp - 0x18]
0047f2b7  pop        ecx
0047f2b8  pop        ecx
0047f2b9  mov        ecx, dword ptr [ebp - 0x14]
0047f2bc  or         ecx, esi
0047f2be  pop        edi
0047f2bf  mov        dword ptr [eax], edx
0047f2c1  mov        dword ptr [eax + 4], ecx
0047f2c4  pop        esi
0047f2c5  jmp        0x47f32b

// block 0047f2c7
0047f2c7  mov        eax, dword ptr [ebp + 0xc]
0047f2ca  push       0x5d
0047f2cc  cmp        dword ptr [eax], ebx
0047f2ce  je         0x47f2fb

// block 0047f2d0
0047f2d0  lea        ecx, [ebp - 0x30]
0047f2d3  push       ecx
0047f2d4  push       1
0047f2d6  lea        ecx, [ebp - 0x28]
0047f2d9  push       ecx
0047f2da  push       0x49de8c
0047f2df  lea        ecx, [ebp - 0x20]
0047f2e2  push       ecx
0047f2e3  push       eax
0047f2e4  lea        eax, [ebp - 0x18]
0047f2e7  push       0x28
0047f2e9  push       eax
0047f2ea  call       0x47ec02

// block 0047f2ef
0047f2ef  add        esp, 0xc
0047f2f2  mov        ecx, eax
0047f2f4  call       0x47ec92

// block 0047f2f9
0047f2f9  jmp        0x47f30f

// block 0047f2fb
0047f2fb  lea        eax, [ebp - 0x30]
0047f2fe  push       eax
0047f2ff  lea        eax, [ebp - 0x28]
0047f302  lea        ecx, [ebp - 0x20]

// block 0047f305
0047f305  push       1
0047f307  push       eax
0047f308  push       0x5b
0047f30a  call       0x47e56d

// block 0047f30f
0047f30f  mov        ecx, eax
0047f311  call       0x47e79b

// block 0047f316
0047f316  mov        ecx, eax
0047f318  call       0x47ec6e

// block 0047f31d
0047f31d  push       eax
0047f31e  push       dword ptr [ebp + 8]
0047f321  call       0x4822f0

// block 0047f326
0047f326  mov        eax, dword ptr [ebp + 8]
0047f329  pop        ecx
0047f32a  pop        ecx

// block 0047f32b
0047f32b  pop        ebx
0047f32c  leave      
0047f32d  ret        
