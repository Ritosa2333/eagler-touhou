
// block 0047735d
0047735d  mov        edi, edi
0047735f  push       ebp
00477360  mov        ebp, esp
00477362  mov        eax, dword ptr [ebp + 0xc]
00477365  push       ebx
00477366  xor        ebx, ebx
00477368  push       esi
00477369  cmp        eax, ebx
0047736b  je         0x4773a7

// block 0047736d
0047736d  cmp        dword ptr [ebp + 0x10], ebx
00477370  jbe        0x4773ac

// block 00477372
00477372  cmp        eax, ebx
00477374  je         0x477378

// block 00477376
00477376  mov        byte ptr [eax], bl

// block 00477378
00477378  push       edi
00477379  mov        edi, dword ptr [ebp + 8]
0047737c  cmp        edi, ebx
0047737e  je         0x47738c

// block 00477380
00477380  mov        eax, dword ptr [ebp + 0x14]
00477383  cmp        eax, ebx
00477385  je         0x4773c7

// block 00477387
00477387  cmp        eax, 1
0047738a  je         0x4773c7

// block 0047738c
0047738c  call       0x46e96f

// block 00477391
00477391  push       0x16
00477393  pop        esi
00477394  push       ebx
00477395  push       ebx
00477396  push       ebx
00477397  push       ebx
00477398  push       ebx
00477399  mov        dword ptr [eax], esi
0047739b  call       0x470f2d

// block 004773a0
004773a0  add        esp, 0x14
004773a3  mov        eax, esi
004773a5  jmp        0x4773fc

// block 004773a7
004773a7  cmp        dword ptr [ebp + 0x10], ebx
004773aa  je         0x477372

// block 004773ac
004773ac  call       0x46e96f

// block 004773b1
004773b1  push       0x16
004773b3  pop        esi
004773b4  push       ebx
004773b5  push       ebx
004773b6  push       ebx
004773b7  push       ebx
004773b8  push       ebx
004773b9  mov        dword ptr [eax], esi
004773bb  call       0x470f2d

// block 004773c0
004773c0  add        esp, 0x14
004773c3  mov        eax, esi
004773c5  jmp        0x4773fd

// block 004773c7
004773c7  lea        esi, [eax*4 + 0x4adb88]
004773ce  push       dword ptr [esi]
004773d0  call       0x475860

// block 004773d5
004773d5  inc        eax
004773d6  pop        ecx
004773d7  mov        dword ptr [edi], eax
004773d9  cmp        dword ptr [ebp + 0xc], ebx
004773dc  jne        0x4773e2

// block 004773de
004773de  xor        eax, eax
004773e0  jmp        0x4773fc

// block 004773e2
004773e2  cmp        eax, dword ptr [ebp + 0x10]
004773e5  jbe        0x4773ec

// block 004773e7
004773e7  push       0x22
004773e9  pop        eax
004773ea  jmp        0x4773fc

// block 004773ec
004773ec  push       dword ptr [esi]
004773ee  push       dword ptr [ebp + 0x10]
004773f1  push       dword ptr [ebp + 0xc]
004773f4  call       0x47a2b9

// block 004773f9
004773f9  add        esp, 0xc

// block 004773fc
004773fc  pop        edi

// block 004773fd
004773fd  pop        esi
004773fe  pop        ebx
004773ff  pop        ebp
00477400  ret        
