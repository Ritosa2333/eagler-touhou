
// block 0044c230
0044c230  push       -1
0044c232  push       0x496fd8
0044c237  mov        eax, dword ptr fs:[0]
0044c23d  push       eax
0044c23e  sub        esp, 0xc
0044c241  push       esi
0044c242  push       edi
0044c243  mov        eax, dword ptr [0x4ad138]
0044c248  xor        eax, esp
0044c24a  push       eax
0044c24b  lea        eax, [esp + 0x18]
0044c24f  mov        dword ptr fs:[0], eax
0044c255  mov        esi, ecx
0044c257  call       0x44c130

// block 0044c25c
0044c25c  mov        edi, eax
0044c25e  test       edi, edi
0044c260  je         0x44c290

// block 0044c262
0044c262  push       0x2f
0044c264  push       esi
0044c265  call       0x46d1a0

// block 0044c26a
0044c26a  add        esp, 8
0044c26d  lea        ecx, [eax + 1]
0044c270  test       eax, eax
0044c272  jne        0x44c276

// block 0044c274
0044c274  mov        ecx, esi

// block 0044c276
0044c276  push       0
0044c278  push       edi
0044c279  call       0x44b7b0

// block 0044c27e
0044c27e  mov        ecx, dword ptr [esp + 0x18]
0044c282  mov        dword ptr fs:[0], ecx
0044c289  pop        ecx
0044c28a  pop        edi
0044c28b  pop        esi
0044c28c  add        esp, 0x18
0044c28f  ret        

// block 0044c290
0044c290  mov        edi, 0x4a22dc
0044c295  mov        dword ptr [esp + 0xc], edi
0044c299  mov        dword ptr [esp + 0x10], 0xffffffff
0044c2a1  mov        dword ptr [esp + 0x14], 0
0044c2a9  push       0x4a2334
0044c2ae  push       esi
0044c2af  lea        ecx, [esp + 0x14]
0044c2b3  mov        dword ptr [esp + 0x28], 0
0044c2bb  call       0x44bda0

// block 0044c2c0
0044c2c0  mov        eax, dword ptr [esp + 0x10]
0044c2c4  cmp        eax, -1
0044c2c7  jne        0x44c2cd

// block 0044c2c9
0044c2c9  xor        eax, eax
0044c2cb  jmp        0x44c2d6

// block 0044c2cd
0044c2cd  push       0
0044c2cf  push       eax
0044c2d0  call       dword ptr [0x4980b0]

// block 0044c2d6
0044c2d6  push       eax
0044c2d7  lea        ecx, [esp + 0x10]
0044c2db  call       0x44bfb0

// block 0044c2e0
0044c2e0  mov        esi, eax
0044c2e2  mov        eax, dword ptr [esp + 0x10]
0044c2e6  mov        dword ptr [esp + 0xc], edi
0044c2ea  cmp        eax, -1
0044c2ed  je         0x44c2f6

// block 0044c2ef
0044c2ef  push       eax
0044c2f0  call       dword ptr [0x4980a4]

// block 0044c2f6
0044c2f6  mov        eax, esi
0044c2f8  mov        ecx, dword ptr [esp + 0x18]
0044c2fc  mov        dword ptr fs:[0], ecx
0044c303  pop        ecx
0044c304  pop        edi
0044c305  pop        esi
0044c306  add        esp, 0x18
0044c309  ret        
