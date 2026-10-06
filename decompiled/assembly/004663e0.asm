
// block 004663e0
004663e0  push       ecx
004663e1  cmp        dword ptr [0x4d477c], 0
004663e8  mov        eax, dword ptr [eax + 4]
004663eb  je         0x46644c

// block 004663ed
004663ed  fild       dword ptr [0x4d477c]
004663f3  add        ecx, 0x1388
004663f9  push       esi
004663fa  mov        esi, dword ptr [eax]
004663fc  fdiv       qword ptr [0x4a3ce8]
00466402  push       edi
00466403  mov        edi, dword ptr [esi]
00466405  fstp       dword ptr [esp + 8]
00466409  fld        dword ptr [esp + 8]
0046640d  fld1       
0046640f  fld        st(0)
00466411  fsubrp     st(2)
00466413  fxch       st(1)
00466415  fstp       dword ptr [esp + 8]
00466419  fld        dword ptr [esp + 8]
0046641d  fmul       st(0), st(0)
0046641f  fstp       dword ptr [esp + 8]
00466423  fsub       dword ptr [esp + 8]
00466427  fstp       dword ptr [esp + 8]
0046642b  fld        dword ptr [esp + 8]
0046642f  mov        dword ptr [esp + 8], ecx
00466433  fimul      dword ptr [esp + 8]
00466437  call       0x4931e0

// block 0046643c
0046643c  mov        edx, dword ptr [edi + 0x3c]
0046643f  sub        eax, 0x1388
00466444  push       eax
00466445  push       esi
00466446  call       edx

// block 00466448
00466448  pop        edi
00466449  pop        esi
0046644a  pop        ecx
0046644b  ret        

// block 0046644c
0046644c  mov        eax, dword ptr [eax]
0046644e  mov        ecx, dword ptr [eax]
00466450  mov        edx, dword ptr [ecx + 0x3c]
00466453  push       0xffffd8f0
00466458  push       eax
00466459  call       edx

// block 0046645b
0046645b  pop        ecx
0046645c  ret        
