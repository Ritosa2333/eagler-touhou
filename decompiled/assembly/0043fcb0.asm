
// block 0043fcb0
0043fcb0  push       ecx
0043fcb1  push       edi
0043fcb2  mov        edi, eax
0043fcb4  mov        eax, dword ptr [edi + 0x24]
0043fcb7  cmp        eax, 4
0043fcba  ja         0x43fdb7

// block 0043fcc0
0043fcc0  jmp        dword ptr [eax*4 + 0x43fdc0]

// block 0043fcc7
0043fcc7  mov        eax, dword ptr [edi + 0x454]
0043fccd  mov        edx, dword ptr [0x4ce8cc]
0043fcd3  push       eax
0043fcd4  call       0x461920

// block 0043fcd9
0043fcd9  test       eax, eax
0043fcdb  jne        0x43fd12

// block 0043fcdd
0043fcdd  push       esi
0043fcde  lea        esi, [eax + 0x63]
0043fce1  call       0x43efa0

// block 0043fce6
0043fce6  mov        esi, 0x6a
0043fceb  call       0x43efa0

// block 0043fcf0
0043fcf0  mov        edx, dword ptr [edi + 0x18]
0043fcf3  push       0
0043fcf5  push       0
0043fcf7  lea        ecx, [esp + 0x10]
0043fcfb  push       ecx
0043fcfc  push       edx
0043fcfd  lea        eax, [esi - 0x53]
0043fd00  xor        ecx, ecx
0043fd02  call       0x4615a0

// block 0043fd07
0043fd07  mov        eax, dword ptr [esp + 8]
0043fd0b  mov        dword ptr [edi + 0x720], eax
0043fd11  pop        esi

// block 0043fd12
0043fd12  mov        ecx, 1
0043fd17  mov        eax, edi
0043fd19  call       0x43ef40

// block 0043fd1e
0043fd1e  mov        eax, dword ptr [edi + 0x30]
0043fd21  test       eax, eax
0043fd23  je         0x43fd34

// block 0043fd25
0043fd25  jg         0x43fd2d

// block 0043fd27
0043fd27  dec        eax
0043fd28  mov        dword ptr [edi + 0x28], eax
0043fd2b  jmp        0x43fd3b

// block 0043fd2d
0043fd2d  xor        eax, eax
0043fd2f  mov        dword ptr [edi + 0x28], eax
0043fd32  jmp        0x43fd3b

// block 0043fd34
0043fd34  mov        dword ptr [edi + 0x28], 0

// block 0043fd3b
0043fd3b  cmp        dword ptr [edi + 0x2b8], 0xa
0043fd42  jle        0x43fdb7

// block 0043fd44
0043fd44  mov        ecx, 2
0043fd49  mov        eax, edi
0043fd4b  call       0x43ef40

// block 0043fd50
0043fd50  mov        eax, 1
0043fd55  pop        edi
0043fd56  pop        ecx
0043fd57  ret        

// block 0043fd58
0043fd58  test       dword ptr [0x4d48c4], 0x80103
0043fd62  je         0x43fdb7

// block 0043fd64
0043fd64  mov        ecx, dword ptr [edi + 0x470]
0043fd6a  push       ebx
0043fd6b  push       ecx
0043fd6c  mov        ebx, 6
0043fd71  call       0x461970

// block 0043fd76
0043fd76  lea        ecx, [ebx - 2]
0043fd79  mov        eax, edi
0043fd7b  call       0x43ef40

// block 0043fd80
0043fd80  lea        edx, [ebx + 8]
0043fd83  call       0x453d90

// block 0043fd88
0043fd88  mov        edx, dword ptr [edi + 0x720]
0043fd8e  push       edx
0043fd8f  mov        ebx, 1
0043fd94  call       0x461970

// block 0043fd99
0043fd99  pop        ebx
0043fd9a  mov        eax, 1
0043fd9f  pop        edi
0043fda0  pop        ecx
0043fda1  ret        

// block 0043fda2
0043fda2  cmp        dword ptr [edi + 0x2b8], 0x1e
0043fda9  jl         0x43fdb7

// block 0043fdab
0043fdab  mov        edx, 1
0043fdb0  mov        eax, edi
0043fdb2  call       0x43eee0

// block 0043fdb7
0043fdb7  mov        eax, 1
0043fdbc  pop        edi
0043fdbd  pop        ecx
0043fdbe  ret        
