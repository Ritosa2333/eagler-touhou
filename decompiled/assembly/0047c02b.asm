
// block 0047c02b
0047c02b  mov        eax, dword ptr [0x4d6300]
0047c030  push       esi
0047c031  push       0x14
0047c033  pop        esi
0047c034  test       eax, eax
0047c036  jne        0x47c03f

// block 0047c038
0047c038  mov        eax, 0x200
0047c03d  jmp        0x47c045

// block 0047c03f
0047c03f  cmp        eax, esi
0047c041  jge        0x47c04a

// block 0047c043
0047c043  mov        eax, esi

// block 0047c045
0047c045  mov        dword ptr [0x4d6300], eax

// block 0047c04a
0047c04a  push       4
0047c04c  push       eax
0047c04d  call       0x477813

// block 0047c052
0047c052  pop        ecx
0047c053  pop        ecx
0047c054  mov        dword ptr [0x4d52e0], eax
0047c059  test       eax, eax
0047c05b  jne        0x47c07b

// block 0047c05d
0047c05d  push       4
0047c05f  push       esi
0047c060  mov        dword ptr [0x4d6300], esi
0047c066  call       0x477813

// block 0047c06b
0047c06b  pop        ecx
0047c06c  pop        ecx
0047c06d  mov        dword ptr [0x4d52e0], eax
0047c072  test       eax, eax
0047c074  jne        0x47c07b

// block 0047c076
0047c076  push       0x1a
0047c078  pop        eax
0047c079  pop        esi
0047c07a  ret        

// block 0047c07b
0047c07b  xor        edx, edx
0047c07d  mov        ecx, 0x4adbf0
0047c082  jmp        0x47c089

// block 0047c084
0047c084  mov        eax, dword ptr [0x4d52e0]

// block 0047c089
0047c089  mov        dword ptr [edx + eax], ecx
0047c08c  add        ecx, 0x20
0047c08f  add        edx, 4
0047c092  cmp        ecx, 0x4ade70
0047c098  jl         0x47c084

// block 0047c09a
0047c09a  push       -2
0047c09c  pop        esi
0047c09d  xor        edx, edx
0047c09f  mov        ecx, 0x4adc00

// block 0047c0a4
0047c0a4  push       edi

// block 0047c0a5
0047c0a5  mov        eax, edx
0047c0a7  sar        eax, 5
0047c0aa  mov        eax, dword ptr [eax*4 + 0x4d6320]
0047c0b1  mov        edi, edx
0047c0b3  and        edi, 0x1f
0047c0b6  shl        edi, 6
0047c0b9  mov        eax, dword ptr [edi + eax]
0047c0bc  cmp        eax, -1
0047c0bf  je         0x47c0c9

// block 0047c0c1
0047c0c1  cmp        eax, esi
0047c0c3  je         0x47c0c9

// block 0047c0c5
0047c0c5  test       eax, eax
0047c0c7  jne        0x47c0cb

// block 0047c0c9
0047c0c9  mov        dword ptr [ecx], esi

// block 0047c0cb
0047c0cb  add        ecx, 0x20
0047c0ce  inc        edx
0047c0cf  cmp        ecx, 0x4adc60
0047c0d5  jl         0x47c0a5

// block 0047c0d7
0047c0d7  pop        edi
0047c0d8  xor        eax, eax
0047c0da  pop        esi
0047c0db  ret        
