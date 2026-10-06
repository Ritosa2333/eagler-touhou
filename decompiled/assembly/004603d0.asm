
// block 004603d0
004603d0  push       ecx
004603d1  push       ebx
004603d2  push       esi
004603d3  mov        esi, dword ptr [0x4ce8cc]
004603d9  mov        dword ptr [esp + 8], esi
004603dd  xor        ebx, ebx
004603df  push       edi
004603e0  add        esi, 0x4b50c0

// block 004603e6
004603e6  mov        edi, dword ptr [esi]
004603e8  test       edi, edi
004603ea  je         0x46042b

// block 004603ec
004603ec  cmp        dword ptr [edi + 0x128], 0
004603f3  je         0x460422

// block 004603f5
004603f5  test       ebx, ebx
004603f7  jl         0x460414

// block 004603f9
004603f9  cmp        ebx, 0x20
004603fc  jae        0x460414

// block 004603fe
004603fe  call       0x4604e0

// block 00460403
00460403  mov        eax, dword ptr [esi]
00460405  push       eax
00460406  call       0x46ca4f

// block 0046040b
0046040b  add        esp, 4
0046040e  mov        dword ptr [esi], 0

// block 00460414
00460414  mov        ecx, dword ptr [esi]
00460416  mov        dword ptr [ecx + 0x128], 0
00460420  jmp        0x46042b

// block 00460422
00460422  cmp        dword ptr [edi + 0x124], 0
00460429  jne        0x46043b

// block 0046042b
0046042b  inc        ebx
0046042c  add        esi, 4
0046042f  cmp        ebx, 0x20
00460432  jb         0x4603e6

// block 00460434
00460434  pop        edi
00460435  pop        esi
00460436  xor        eax, eax
00460438  pop        ebx
00460439  pop        ecx
0046043a  ret        

// block 0046043b
0046043b  mov        edx, dword ptr [esp + 0xc]
0046043f  mov        edi, dword ptr [edx + ebx*4 + 0x4b50c0]
00460446  call       0x45ffc0

// block 0046044b
0046044b  neg        eax
0046044d  sbb        eax, eax
0046044f  pop        edi
00460450  neg        eax
00460452  pop        esi
00460453  dec        eax
00460454  pop        ebx
00460455  pop        ecx
00460456  ret        
