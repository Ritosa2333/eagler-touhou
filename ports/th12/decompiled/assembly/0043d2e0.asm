
// block 0043d2e0
0043d2e0  sub        esp, 0x10
0043d2e3  push       ebx
0043d2e4  mov        ebx, dword ptr [0x4b451c]
0043d2ea  cmp        dword ptr [ebx], 0
0043d2ed  jne        0x43d2f7

// block 0043d2ef
0043d2ef  or         eax, 0xffffffff
0043d2f2  pop        ebx
0043d2f3  add        esp, 0x10
0043d2f6  ret        

// block 0043d2f7
0043d2f7  push       ebp
0043d2f8  push       esi
0043d2f9  push       edi
0043d2fa  push       0x200000
0043d2ff  call       0x46d04a

// block 0043d304
0043d304  mov        ebp, eax
0043d306  mov        eax, dword ptr [ebx]
0043d308  mov        ecx, dword ptr [eax]
0043d30a  mov        dword ptr [ebp], ecx
0043d30d  mov        edx, dword ptr [eax + 4]
0043d310  mov        dword ptr [ebp + 4], edx
0043d313  mov        ecx, dword ptr [eax + 8]
0043d316  mov        dword ptr [ebp + 8], ecx
0043d319  mov        edx, dword ptr [eax + 0xc]
0043d31c  mov        dword ptr [ebp + 0xc], edx
0043d31f  mov        ecx, dword ptr [eax + 0x10]
0043d322  mov        dword ptr [ebp + 0x10], ecx
0043d325  mov        edx, dword ptr [eax + 0x14]
0043d328  add        esp, 4
0043d32b  mov        dword ptr [ebp + 0x14], edx
0043d32e  mov        dword ptr [esp + 0x10], 0x18
0043d336  xor        edi, edi
0043d338  lea        esi, [ebx + 8]
0043d33b  jmp        0x43d340

// block 0043d340
0043d340  mov        eax, 0x5243
0043d345  cmp        word ptr [esi], ax
0043d348  jne        0x43d379

// block 0043d34a
0043d34a  push       0x45f4
0043d34f  mov        ecx, esi
0043d351  mov        dword ptr [esi + 0xc], edi
0043d354  call       0x43cdb0

// block 0043d359
0043d359  mov        ecx, dword ptr [esp + 0x10]
0043d35d  push       0x45f4
0043d362  add        ecx, ebp
0043d364  push       esi
0043d365  push       ecx
0043d366  mov        dword ptr [esi + 4], eax
0043d369  call       0x47a330

// block 0043d36e
0043d36e  add        esp, 0xc
0043d371  add        dword ptr [esp + 0x10], 0x45f4

// block 0043d379
0043d379  inc        edi
0043d37a  add        esi, 0x45f4
0043d380  cmp        edi, 7
0043d383  jb         0x43d340

// block 0043d385
0043d385  xor        edx, edx
0043d387  xor        ecx, ecx
0043d389  mov        dword ptr [esp + 0x18], edx
0043d38d  mov        dword ptr [esp + 0x14], edx
0043d391  lea        eax, [ebx + 0x1e9bd]
0043d397  mov        dword ptr [esp + 0x1c], 0x110
0043d39f  nop        

// block 0043d3a0
0043d3a0  movzx      esi, byte ptr [eax - 1]
0043d3a4  add        ecx, esi
0043d3a6  movzx      esi, byte ptr [eax]
0043d3a9  add        edx, esi
0043d3ab  movzx      esi, byte ptr [eax + 1]
0043d3af  add        dword ptr [esp + 0x14], esi
0043d3b3  movzx      esi, byte ptr [eax + 2]
0043d3b7  add        dword ptr [esp + 0x18], esi
0043d3bb  add        eax, 4
0043d3be  sub        dword ptr [esp + 0x1c], 1
0043d3c3  jne        0x43d3a0

// block 0043d3c5
0043d3c5  mov        eax, dword ptr [esp + 0x14]
0043d3c9  add        edx, eax
0043d3cb  add        edx, dword ptr [esp + 0x18]
0043d3cf  lea        esi, [ebx + 0x1e9b4]
0043d3d5  add        ecx, edx
0043d3d7  mov        edx, dword ptr [esp + 0x10]
0043d3db  mov        dword ptr [ebx + 0x1e9b8], ecx
0043d3e1  mov        ecx, dword ptr [esp + 0x10]
0043d3e5  lea        edi, [ecx + ebp]
0043d3e8  mov        ecx, 0x112

// block 0043d3ed
0043d3ed  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043d3ef
0043d3ef  mov        eax, dword ptr [ebx]
0043d3f1  add        edx, 0x430
0043d3f7  mov        dword ptr [eax + 0x14], edx
0043d3fa  mov        eax, dword ptr [ebx]
0043d3fc  mov        edx, dword ptr [eax + 0x14]
0043d3ff  lea        ecx, [eax + 0x10]
0043d402  push       ecx
0043d403  push       edx
0043d404  lea        eax, [ebp + 0x18]
0043d407  push       eax
0043d408  call       0x44c3f0

// block 0043d40d
0043d40d  mov        edi, eax
0043d40f  mov        eax, dword ptr [ebx]
0043d411  mov        ecx, dword ptr [eax + 0x10]
0043d414  add        ecx, 0x18
0043d417  mov        dword ptr [eax + 4], ecx
0043d41a  mov        edx, dword ptr [ebx]
0043d41c  mov        eax, dword ptr [edx + 0x10]
0043d41f  push       eax
0043d420  push       0x10
0043d422  push       0x35
0043d424  push       eax
0043d425  push       edi
0043d426  mov        al, 0xac
0043d428  mov        dword ptr [esp + 0x30], edi
0043d42c  call       0x463af0

// block 0043d431
0043d431  mov        esi, 0x4a14b0
0043d436  call       0x463fb0

// block 0043d43b
0043d43b  test       eax, eax
0043d43d  je         0x43d476

// block 0043d43f
0043d43f  push       0x4a15a4
0043d444  mov        edi, 0x4b0ec8
0043d449  call       0x464300

// block 0043d44e
0043d44e  mov        eax, dword ptr [esp + 0x20]
0043d452  add        esp, 4
0043d455  test       eax, eax
0043d457  je         0x43d462

// block 0043d459
0043d459  push       eax
0043d45a  call       0x46c941

// block 0043d45f
0043d45f  add        esp, 4

// block 0043d462
0043d462  push       ebp
0043d463  call       0x46c941

// block 0043d468
0043d468  add        esp, 4
0043d46b  pop        edi
0043d46c  pop        esi
0043d46d  pop        ebp
0043d46e  or         eax, 0xffffffff
0043d471  pop        ebx
0043d472  add        esp, 0x10
0043d475  ret        

// block 0043d476
0043d476  mov        eax, dword ptr [0x4ae590]
0043d47b  cmp        eax, -1
0043d47e  je         0x43d4c8

// block 0043d480
0043d480  mov        edx, dword ptr [ebx]
0043d482  push       0
0043d484  lea        ecx, [esp + 0x20]
0043d488  push       ecx
0043d489  push       0x18
0043d48b  push       edx
0043d48c  push       eax
0043d48d  call       dword ptr [0x4980ac]

// block 0043d493
0043d493  cmp        dword ptr [esp + 0x1c], 0x18
0043d498  je         0x43d4c3

// block 0043d49a
0043d49a  mov        eax, dword ptr [0x4ae590]
0043d49f  push       eax
0043d4a0  call       dword ptr [0x4980a4]

// block 0043d4a6
0043d4a6  test       dword ptr [0x4cee78], 0x8000
0043d4b0  je         0x43d4c3

// block 0043d4b2
0043d4b2  push       0x4cf128
0043d4b7  call       dword ptr [0x49808c]

// block 0043d4bd
0043d4bd  dec        byte ptr [0x4cf21a]

// block 0043d4c3
0043d4c3  mov        eax, dword ptr [0x4ae590]

// block 0043d4c8
0043d4c8  mov        ecx, dword ptr [ebx]
0043d4ca  mov        esi, dword ptr [ecx + 0x10]
0043d4cd  cmp        eax, -1
0043d4d0  je         0x43d53f

// block 0043d4d2
0043d4d2  push       0
0043d4d4  lea        edx, [esp + 0x1c]
0043d4d8  push       edx
0043d4d9  push       esi
0043d4da  push       edi
0043d4db  push       eax
0043d4dc  call       dword ptr [0x4980ac]

// block 0043d4e2
0043d4e2  cmp        esi, dword ptr [esp + 0x18]
0043d4e6  je         0x43d511

// block 0043d4e8
0043d4e8  mov        eax, dword ptr [0x4ae590]
0043d4ed  push       eax
0043d4ee  call       dword ptr [0x4980a4]

// block 0043d4f4
0043d4f4  test       dword ptr [0x4cee78], 0x8000
0043d4fe  je         0x43d511

// block 0043d500
0043d500  push       0x4cf128
0043d505  call       dword ptr [0x49808c]

// block 0043d50b
0043d50b  dec        byte ptr [0x4cf21a]

// block 0043d511
0043d511  mov        eax, dword ptr [0x4ae590]
0043d516  cmp        eax, -1
0043d519  je         0x43d53f

// block 0043d51b
0043d51b  push       eax
0043d51c  call       dword ptr [0x4980a4]

// block 0043d522
0043d522  test       dword ptr [0x4cee78], 0x8000
0043d52c  je         0x43d53f

// block 0043d52e
0043d52e  push       0x4cf128
0043d533  call       dword ptr [0x49808c]

// block 0043d539
0043d539  dec        byte ptr [0x4cf21a]

// block 0043d53f
0043d53f  test       edi, edi
0043d541  je         0x43d54c

// block 0043d543
0043d543  push       edi
0043d544  call       0x46c941

// block 0043d549
0043d549  add        esp, 4

// block 0043d54c
0043d54c  push       ebp
0043d54d  call       0x46c941

// block 0043d552
0043d552  add        esp, 4
0043d555  pop        edi
0043d556  pop        esi
0043d557  pop        ebp
0043d558  xor        eax, eax
0043d55a  pop        ebx
0043d55b  add        esp, 0x10
0043d55e  ret        
