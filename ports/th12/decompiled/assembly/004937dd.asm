
// block 004937dd
004937dd  push       edx
004937de  wait       
004937df  fnstcw     word ptr [esp]
004937e2  mov        eax, dword ptr [esp + 0xc]
004937e6  je         0x493839

// block 004937e8
004937e8  cmp        word ptr [esp], 0x27f
004937ee  je         0x4937f5

// block 004937f0
004937f0  call       0x495675

// block 004937f5
004937f5  test       eax, 0x80000000
004937fa  jne        0x49381b

// block 004937fc
004937fc  fsqrt      

// block 004937fe
004937fe  cmp        dword ptr [0x4b4110], 0
00493805  jne        0x4956fe

// block 0049380b
0049380b  mov        edx, 5
00493810  lea        ecx, [0x4b3590]
00493816  jmp        0x49570b

// block 0049381b
0049381b  test       eax, 0x7ff00000
00493820  jne        0x49384e

// block 00493822
00493822  test       eax, 0xfffff
00493827  jne        0x49384e

// block 00493829
00493829  cmp        dword ptr [esp + 8], 0
0049382e  jne        0x49384e

// block 00493830
00493830  jmp        0x4937fe

// block 00493832
00493832  call       0x49568c

// block 00493837
00493837  jmp        0x49385b

// block 00493839
00493839  test       eax, 0xfffff
0049383e  jne        0x493832

// block 00493840
00493840  cmp        dword ptr [esp + 8], 0
00493845  jne        0x493832

// block 00493847
00493847  and        eax, 0x80000000
0049384c  je         0x4937fe

// block 0049384e
0049384e  fstp       st(0)
00493850  fld        xword ptr [0x4b35c0]
00493856  mov        eax, 1

// block 0049385b
0049385b  cmp        dword ptr [0x4b4110], 0
00493862  jne        0x4956fe

// block 00493868
00493868  mov        edx, 5
0049386d  lea        ecx, [0x4b3590]
00493873  call       0x495617

// block 00493878
00493878  pop        edx
00493879  ret        
