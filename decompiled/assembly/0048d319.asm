
// block 0048d319
0048d319  mov        edi, edi
0048d31b  push       ebp
0048d31c  mov        ebp, esp
0048d31e  push       esi
0048d31f  mov        esi, dword ptr [ebp + 8]
0048d322  push       esi
0048d323  call       0x47c1da

// block 0048d328
0048d328  push       eax
0048d329  call       0x47bfc1

// block 0048d32e
0048d32e  pop        ecx
0048d32f  pop        ecx
0048d330  test       eax, eax
0048d332  je         0x48d3b0

// block 0048d334
0048d334  call       0x47c025

// block 0048d339
0048d339  add        eax, 0x20
0048d33c  cmp        esi, eax
0048d33e  jne        0x48d344

// block 0048d340
0048d340  xor        eax, eax
0048d342  jmp        0x48d353

// block 0048d344
0048d344  call       0x47c025

// block 0048d349
0048d349  add        eax, 0x40
0048d34c  cmp        esi, eax
0048d34e  jne        0x48d3b0

// block 0048d350
0048d350  xor        eax, eax
0048d352  inc        eax

// block 0048d353
0048d353  inc        dword ptr [0x4b42f0]
0048d359  test       dword ptr [esi + 0xc], 0x10c
0048d360  jne        0x48d3b0

// block 0048d362
0048d362  push       ebx
0048d363  push       edi
0048d364  lea        edi, [eax*4 + 0x4b43a4]
0048d36b  cmp        dword ptr [edi], 0
0048d36e  mov        ebx, 0x1000
0048d373  jne        0x48d395

// block 0048d375
0048d375  push       ebx
0048d376  call       0x4777ce

// block 0048d37b
0048d37b  pop        ecx
0048d37c  mov        dword ptr [edi], eax
0048d37e  test       eax, eax
0048d380  jne        0x48d395

// block 0048d382
0048d382  lea        eax, [esi + 0x14]
0048d385  push       2
0048d387  mov        dword ptr [esi + 8], eax
0048d38a  mov        dword ptr [esi], eax
0048d38c  pop        eax
0048d38d  mov        dword ptr [esi + 0x18], eax
0048d390  mov        dword ptr [esi + 4], eax
0048d393  jmp        0x48d3a2

// block 0048d395
0048d395  mov        edi, dword ptr [edi]
0048d397  mov        dword ptr [esi + 8], edi
0048d39a  mov        dword ptr [esi], edi
0048d39c  mov        dword ptr [esi + 0x18], ebx
0048d39f  mov        dword ptr [esi + 4], ebx

// block 0048d3a2
0048d3a2  or         dword ptr [esi + 0xc], 0x1102
0048d3a9  xor        eax, eax
0048d3ab  pop        edi
0048d3ac  inc        eax
0048d3ad  pop        ebx
0048d3ae  jmp        0x48d3b2

// block 0048d3b0
0048d3b0  xor        eax, eax

// block 0048d3b2
0048d3b2  pop        esi
0048d3b3  pop        ebp
0048d3b4  ret        
