
// block 00465440
00465440  mov        edx, dword ptr [ecx]
00465442  push       esi
00465443  mov        esi, 1
00465448  push       edi
00465449  mov        dword ptr [ecx + 8], 0
00465450  mov        dword ptr [ecx + 0x12c], 0
0046545a  lea        eax, [ecx + 0x14]
0046545d  lea        edi, [esi + 0x1f]

// block 00465460
00465460  test       dl, 1
00465463  je         0x46547f

// block 00465465
00465465  inc        dword ptr [eax]
00465467  cmp        dword ptr [eax], 8
0046546a  jb         0x465472

// block 0046546c
0046546c  or         dword ptr [ecx + 0x12c], esi

// block 00465472
00465472  cmp        dword ptr [eax], 0x1a
00465475  jb         0x465485

// block 00465477
00465477  or         dword ptr [ecx + 8], esi
0046547a  add        dword ptr [eax], -8
0046547d  jmp        0x465485

// block 0046547f
0046547f  mov        dword ptr [eax], 0

// block 00465485
00465485  add        eax, 4
00465488  shr        edx, 1
0046548a  add        esi, esi
0046548c  sub        edi, 1
0046548f  jne        0x465460

// block 00465491
00465491  mov        eax, dword ptr [ecx]
00465493  mov        edx, dword ptr [ecx + 4]
00465496  xor        edx, eax
00465498  mov        esi, edx
0046549a  and        esi, eax
0046549c  not        eax
0046549e  and        eax, edx
004654a0  pop        edi
004654a1  mov        dword ptr [ecx + 0xc], esi
004654a4  mov        dword ptr [ecx + 0x10], eax
004654a7  pop        esi
004654a8  ret        
