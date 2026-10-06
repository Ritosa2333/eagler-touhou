
// block 0046f1be
0046f1be  mov        edi, edi
0046f1c0  push       ebp
0046f1c1  mov        ebp, esp
0046f1c3  push       ecx
0046f1c4  push       ecx
0046f1c5  mov        ecx, dword ptr [ebp + 8]
0046f1c8  mov        eax, dword ptr [ecx + 8]
0046f1cb  push       ebx
0046f1cc  push       esi
0046f1cd  mov        esi, dword ptr [ecx + 0x10]
0046f1d0  push       edi
0046f1d1  xor        ebx, ebx
0046f1d3  jmp        0x46f1d8

// block 0046f1d5
0046f1d5  add        eax, eax
0046f1d7  inc        ebx

// block 0046f1d8
0046f1d8  test       eax, eax
0046f1da  jge        0x46f1d5

// block 0046f1dc
0046f1dc  mov        eax, ebx
0046f1de  imul       eax, eax, 0x204
0046f1e4  lea        eax, [eax + esi + 0x144]
0046f1eb  push       0x3f
0046f1ed  mov        dword ptr [ebp - 8], eax
0046f1f0  pop        edx

// block 0046f1f1
0046f1f1  mov        dword ptr [eax + 8], eax
0046f1f4  mov        dword ptr [eax + 4], eax
0046f1f7  add        eax, 8
0046f1fa  dec        edx
0046f1fb  jne        0x46f1f1

// block 0046f1fd
0046f1fd  push       4
0046f1ff  mov        edi, ebx
0046f201  push       0x1000
0046f206  shl        edi, 0xf
0046f209  add        edi, dword ptr [ecx + 0xc]
0046f20c  push       0x8000
0046f211  push       edi
0046f212  call       dword ptr [0x498184]

// block 0046f218
0046f218  test       eax, eax
0046f21a  jne        0x46f224

// block 0046f21c
0046f21c  or         eax, 0xffffffff
0046f21f  jmp        0x46f2c1

// block 0046f224
0046f224  lea        edx, [edi + 0x7000]
0046f22a  mov        dword ptr [ebp - 4], edx
0046f22d  cmp        edi, edx
0046f22f  ja         0x46f274

// block 0046f231
0046f231  mov        ecx, edx
0046f233  sub        ecx, edi
0046f235  shr        ecx, 0xc
0046f238  lea        eax, [edi + 0x10]
0046f23b  inc        ecx

// block 0046f23c
0046f23c  or         dword ptr [eax - 8], 0xffffffff
0046f240  or         dword ptr [eax + 0xfec], 0xffffffff
0046f247  lea        edx, [eax + 0xffc]
0046f24d  mov        dword ptr [eax], edx
0046f24f  lea        edx, [eax - 0x1004]
0046f255  mov        dword ptr [eax - 4], 0xff0
0046f25c  mov        dword ptr [eax + 4], edx
0046f25f  mov        dword ptr [eax + 0xfe8], 0xff0
0046f269  add        eax, 0x1000
0046f26e  dec        ecx
0046f26f  jne        0x46f23c

// block 0046f271
0046f271  mov        edx, dword ptr [ebp - 4]

// block 0046f274
0046f274  mov        eax, dword ptr [ebp - 8]
0046f277  add        eax, 0x1f8
0046f27c  lea        ecx, [edi + 0xc]
0046f27f  mov        dword ptr [eax + 4], ecx
0046f282  mov        dword ptr [ecx + 8], eax
0046f285  lea        ecx, [edx + 0xc]
0046f288  mov        dword ptr [eax + 8], ecx
0046f28b  mov        dword ptr [ecx + 4], eax
0046f28e  and        dword ptr [esi + ebx*4 + 0x44], 0
0046f293  xor        edi, edi
0046f295  inc        edi
0046f296  mov        dword ptr [esi + ebx*4 + 0xc4], edi
0046f29d  mov        al, byte ptr [esi + 0x43]
0046f2a0  mov        cl, al
0046f2a2  inc        cl
0046f2a4  test       al, al
0046f2a6  mov        eax, dword ptr [ebp + 8]
0046f2a9  mov        byte ptr [esi + 0x43], cl
0046f2ac  jne        0x46f2b1

// block 0046f2ae
0046f2ae  or         dword ptr [eax + 4], edi

// block 0046f2b1
0046f2b1  mov        edx, 0x80000000
0046f2b6  mov        ecx, ebx
0046f2b8  shr        edx, cl
0046f2ba  not        edx
0046f2bc  and        dword ptr [eax + 8], edx
0046f2bf  mov        eax, ebx

// block 0046f2c1
0046f2c1  pop        edi
0046f2c2  pop        esi
0046f2c3  pop        ebx
0046f2c4  leave      
0046f2c5  ret        
