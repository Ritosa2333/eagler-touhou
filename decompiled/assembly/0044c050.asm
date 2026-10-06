
// block 0044c050
0044c050  push       esi
0044c051  push       edi
0044c052  mov        esi, eax
0044c054  push       0x3a
0044c056  push       esi
0044c057  mov        edi, ecx
0044c059  call       0x46d1a0

// block 0044c05e
0044c05e  add        esp, 8
0044c061  test       eax, eax
0044c063  je         0x44c07d

// block 0044c065
0044c065  mov        eax, esi
0044c067  sub        edi, esi
0044c069  lea        esp, [esp]

// block 0044c070
0044c070  mov        cl, byte ptr [eax]
0044c072  mov        byte ptr [edi + eax], cl
0044c075  inc        eax
0044c076  test       cl, cl
0044c078  jne        0x44c070

// block 0044c07a
0044c07a  pop        edi
0044c07b  pop        esi
0044c07c  ret        

// block 0044c07d
0044c07d  push       0x104
0044c082  push       edi
0044c083  push       0
0044c085  call       dword ptr [0x4980b4]

// block 0044c08b
0044c08b  push       0x5c
0044c08d  push       edi
0044c08e  call       0x46d260

// block 0044c093
0044c093  add        esp, 8
0044c096  test       eax, eax
0044c098  jne        0x44c09c

// block 0044c09a
0044c09a  mov        byte ptr [edi], al

// block 0044c09c
0044c09c  mov        byte ptr [eax + 1], 0
0044c0a0  mov        eax, esi
0044c0a2  mov        ecx, esi

// block 0044c0a4
0044c0a4  mov        dl, byte ptr [eax]
0044c0a6  inc        eax
0044c0a7  test       dl, dl
0044c0a9  jne        0x44c0a4

// block 0044c0ab
0044c0ab  sub        eax, ecx
0044c0ad  dec        edi
0044c0ae  mov        edi, edi

// block 0044c0b0
0044c0b0  mov        cl, byte ptr [edi + 1]
0044c0b3  inc        edi
0044c0b4  test       cl, cl
0044c0b6  jne        0x44c0b0

// block 0044c0b8
0044c0b8  mov        ecx, eax
0044c0ba  shr        ecx, 2

// block 0044c0bd
0044c0bd  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0044c0bf
0044c0bf  mov        ecx, eax
0044c0c1  and        ecx, 3

// block 0044c0c4
0044c0c4  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 0044c0c6
0044c0c6  pop        edi
0044c0c7  pop        esi
0044c0c8  ret        
