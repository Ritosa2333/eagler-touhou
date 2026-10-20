
// block 00401090
00401090  push       ebx
00401091  push       ebp
00401092  push       edi
00401093  mov        ebx, 0x49f428
00401098  mov        ecx, 2
0040109d  mov        edi, eax
0040109f  call       0x45fe60

// block 004010a4
004010a4  xor        ebp, ebp
004010a6  mov        dword ptr [edi + 0x18fb4], eax
004010ac  cmp        eax, ebp
004010ae  jne        0x4010c9

// block 004010b0
004010b0  push       0x49f434
004010b5  mov        ecx, 0x4b0ec8
004010ba  call       0x464220

// block 004010bf
004010bf  add        esp, 4
004010c2  pop        edi
004010c3  pop        ebp
004010c4  or         eax, 0xffffffff
004010c7  pop        ebx
004010c8  ret        

// block 004010c9
004010c9  push       esi
004010ca  push       0x24
004010cc  call       0x46c9ea

// block 004010d1
004010d1  add        esp, 4
004010d4  cmp        eax, ebp
004010d6  je         0x4010f4

// block 004010d8
004010d8  and        dword ptr [eax + 4], 0xfffffffe
004010dc  mov        dword ptr [eax + 8], ebp
004010df  mov        dword ptr [eax + 0xc], ebp
004010e2  mov        dword ptr [eax + 0x10], ebp
004010e5  mov        dword ptr [eax], ebp
004010e7  mov        dword ptr [eax + 0x14], eax
004010ea  mov        dword ptr [eax + 0x18], ebp
004010ed  mov        dword ptr [eax + 0x1c], ebp
004010f0  mov        esi, eax
004010f2  jmp        0x4010f6

// block 004010f4
004010f4  xor        esi, esi

// block 004010f6
004010f6  mov        eax, dword ptr [esi + 4]
004010f9  and        eax, 0xfffffffd
004010fc  or         eax, 1
004010ff  mov        ebx, 4
00401104  mov        dword ptr [esi + 8], 0x4014b0
0040110b  mov        dword ptr [esi + 0xc], ebp
0040110e  mov        dword ptr [esi + 0x10], ebp
00401111  mov        dword ptr [esi + 0x20], edi
00401114  mov        dword ptr [esi + 4], eax
00401117  call       0x462380

// block 0040111c
0040111c  push       0x24
0040111e  mov        dword ptr [edi + 0xc], esi
00401121  call       0x46c9ea

// block 00401126
00401126  add        esp, 4
00401129  cmp        eax, ebp
0040112b  je         0x401149

// block 0040112d
0040112d  and        dword ptr [eax + 4], 0xfffffffe
00401131  mov        dword ptr [eax + 8], ebp
00401134  mov        dword ptr [eax + 0xc], ebp
00401137  mov        dword ptr [eax + 0x10], ebp
0040113a  mov        dword ptr [eax], ebp
0040113c  mov        dword ptr [eax + 0x14], eax
0040113f  mov        dword ptr [eax + 0x18], ebp
00401142  mov        dword ptr [eax + 0x1c], ebp
00401145  mov        esi, eax
00401147  jmp        0x40114b

// block 00401149
00401149  xor        esi, esi

// block 0040114b
0040114b  mov        ecx, dword ptr [esi + 4]
0040114e  and        ecx, 0xfffffffd
00401151  or         ecx, 1
00401154  mov        ebx, 0x45
00401159  mov        dword ptr [esi + 8], 0x4014d0
00401160  mov        dword ptr [esi + 0xc], ebp
00401163  mov        dword ptr [esi + 0x10], ebp
00401166  mov        dword ptr [esi + 0x20], edi
00401169  mov        dword ptr [esi + 4], ecx
0040116c  call       0x462420

// block 00401171
00401171  push       0x24
00401173  mov        dword ptr [edi + 0x10], esi
00401176  call       0x46c9ea

// block 0040117b
0040117b  add        esp, 4
0040117e  cmp        eax, ebp
00401180  je         0x40119e

// block 00401182
00401182  and        dword ptr [eax + 4], 0xfffffffe
00401186  mov        dword ptr [eax + 8], ebp
00401189  mov        dword ptr [eax + 0xc], ebp
0040118c  mov        dword ptr [eax + 0x10], ebp
0040118f  mov        dword ptr [eax], ebp
00401191  mov        dword ptr [eax + 0x14], eax
00401194  mov        dword ptr [eax + 0x18], ebp
00401197  mov        dword ptr [eax + 0x1c], ebp
0040119a  mov        esi, eax
0040119c  jmp        0x4011a0

// block 0040119e
0040119e  xor        esi, esi

// block 004011a0
004011a0  mov        edx, dword ptr [esi + 4]
004011a3  and        edx, 0xfffffffd
004011a6  or         edx, 1
004011a9  mov        ebx, 0x2d
004011ae  mov        dword ptr [esi + 8], 0x4014e0
004011b5  mov        dword ptr [esi + 0xc], ebp
004011b8  mov        dword ptr [esi + 0x10], ebp
004011bb  mov        dword ptr [esi + 0x20], edi
004011be  mov        dword ptr [esi + 4], edx
004011c1  call       0x462420

// block 004011c6
004011c6  mov        ebx, dword ptr [edi + 0x18fb4]
004011cc  mov        dword ptr [edi + 0x18fc0], esi
004011d2  lea        esi, [edi + 0x14]
004011d5  call       0x402520

// block 004011da
004011da  xor        ecx, ecx
004011dc  mov        eax, esi
004011de  mov        edx, ebx
004011e0  mov        dword ptr [esi + 0x3f8], ebx
004011e6  call       0x454b80

// block 004011eb
004011eb  lea        esi, [edi + 0x4c8]
004011f1  mov        edi, dword ptr [edi + 0x18fb4]
004011f7  call       0x402520

// block 004011fc
004011fc  mov        ecx, 0x62
00401201  mov        eax, esi
00401203  mov        edx, edi
00401205  mov        dword ptr [esi + 0x3f8], edi
0040120b  call       0x454b80

// block 00401210
00401210  pop        esi
00401211  pop        edi
00401212  pop        ebp
00401213  xor        eax, eax
00401215  pop        ebx
00401216  ret        
