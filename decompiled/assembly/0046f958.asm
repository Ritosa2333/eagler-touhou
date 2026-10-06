
// block 0046f958
0046f958  mov        edi, edi
0046f95a  push       ebp
0046f95b  mov        ebp, esp
0046f95d  push       esi
0046f95e  xor        esi, esi
0046f960  cmp        dword ptr [0x4b3c04], esi
0046f966  jne        0x46f96f

// block 0046f968
0046f968  xor        eax, eax
0046f96a  jmp        0x46fa04

// block 0046f96f
0046f96f  mov        eax, dword ptr [0x4d6458]
0046f974  cmp        eax, 3
0046f977  jne        0x46f9a7

// block 0046f979
0046f979  mov        eax, dword ptr [ebp + 8]
0046f97c  cmp        eax, 0x3f8
0046f981  jbe        0x46f99d

// block 0046f983
0046f983  call       0x46e96f

// block 0046f988
0046f988  push       esi
0046f989  push       esi
0046f98a  push       esi
0046f98b  push       esi
0046f98c  push       esi
0046f98d  mov        dword ptr [eax], 0x16
0046f993  call       0x470f2d

// block 0046f998
0046f998  add        esp, 0x14
0046f99b  jmp        0x46f968

// block 0046f99d
0046f99d  mov        dword ptr [0x4d6448], eax
0046f9a2  xor        eax, eax
0046f9a4  inc        eax
0046f9a5  jmp        0x46fa04

// block 0046f9a7
0046f9a7  push       edi
0046f9a8  mov        edi, dword ptr [ebp + 8]
0046f9ab  cmp        edi, esi
0046f9ad  je         0x46f9f1

// block 0046f9af
0046f9af  cmp        eax, 1
0046f9b2  jne        0x46f9f6

// block 0046f9b4
0046f9b4  cmp        edi, 0x3f8
0046f9ba  ja         0x46f9c7

// block 0046f9bc
0046f9bc  push       edi
0046f9bd  call       0x46ed7a

// block 0046f9c2
0046f9c2  pop        ecx
0046f9c3  test       eax, eax
0046f9c5  jne        0x46f9e1

// block 0046f9c7
0046f9c7  call       0x46e96f

// block 0046f9cc
0046f9cc  push       esi
0046f9cd  push       esi
0046f9ce  push       esi
0046f9cf  push       esi
0046f9d0  push       esi
0046f9d1  mov        dword ptr [eax], 0x16
0046f9d7  call       0x470f2d

// block 0046f9dc
0046f9dc  add        esp, 0x14
0046f9df  jmp        0x46fa01

// block 0046f9e1
0046f9e1  mov        dword ptr [0x4d6448], edi
0046f9e7  mov        dword ptr [0x4d6458], 3

// block 0046f9f1
0046f9f1  xor        eax, eax
0046f9f3  inc        eax
0046f9f4  jmp        0x46fa03

// block 0046f9f6
0046f9f6  call       0x46e96f

// block 0046f9fb
0046f9fb  mov        dword ptr [eax], 0x16

// block 0046fa01
0046fa01  xor        eax, eax

// block 0046fa03
0046fa03  pop        edi

// block 0046fa04
0046fa04  pop        esi
0046fa05  pop        ebp
0046fa06  ret        
