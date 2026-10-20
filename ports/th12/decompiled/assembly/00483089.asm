
// block 00483089
00483089  mov        edi, edi
0048308b  push       ebp
0048308c  mov        ebp, esp
0048308e  mov        eax, dword ptr [ebp + 8]
00483091  push       ebx
00483092  xor        ebx, ebx
00483094  push       esi
00483095  push       edi
00483096  cmp        eax, ebx
00483098  je         0x4830a1

// block 0048309a
0048309a  mov        edi, dword ptr [ebp + 0xc]
0048309d  cmp        edi, ebx
0048309f  ja         0x4830bc

// block 004830a1
004830a1  call       0x46e96f

// block 004830a6
004830a6  push       0x16
004830a8  pop        esi
004830a9  mov        dword ptr [eax], esi

// block 004830ab
004830ab  push       ebx
004830ac  push       ebx
004830ad  push       ebx
004830ae  push       ebx
004830af  push       ebx
004830b0  call       0x470f2d

// block 004830b5
004830b5  add        esp, 0x14
004830b8  mov        eax, esi
004830ba  jmp        0x4830f8

// block 004830bc
004830bc  mov        esi, dword ptr [ebp + 0x10]
004830bf  cmp        esi, ebx
004830c1  jne        0x4830c7

// block 004830c3
004830c3  mov        byte ptr [eax], bl
004830c5  jmp        0x4830a1

// block 004830c7
004830c7  mov        edx, eax

// block 004830c9
004830c9  cmp        byte ptr [edx], bl
004830cb  je         0x4830d1

// block 004830cd
004830cd  inc        edx
004830ce  dec        edi
004830cf  jne        0x4830c9

// block 004830d1
004830d1  cmp        edi, ebx
004830d3  je         0x4830c3

// block 004830d5
004830d5  mov        cl, byte ptr [esi]
004830d7  mov        byte ptr [edx], cl
004830d9  inc        edx
004830da  inc        esi
004830db  cmp        cl, bl
004830dd  je         0x4830e2

// block 004830df
004830df  dec        edi
004830e0  jne        0x4830d5

// block 004830e2
004830e2  cmp        edi, ebx
004830e4  jne        0x4830f6

// block 004830e6
004830e6  mov        byte ptr [eax], bl
004830e8  call       0x46e96f

// block 004830ed
004830ed  push       0x22
004830ef  pop        ecx
004830f0  mov        dword ptr [eax], ecx
004830f2  mov        esi, ecx
004830f4  jmp        0x4830ab

// block 004830f6
004830f6  xor        eax, eax

// block 004830f8
004830f8  pop        edi
004830f9  pop        esi
004830fa  pop        ebx
004830fb  pop        ebp
004830fc  ret        
