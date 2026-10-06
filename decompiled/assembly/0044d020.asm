
// block 0044d020
0044d020  push       ebx
0044d021  push       ebp
0044d022  push       esi
0044d023  mov        esi, ecx
0044d025  mov        eax, dword ptr [esi]
0044d027  mov        edx, dword ptr [eax + 4]
0044d02a  push       edi
0044d02b  call       edx

// block 0044d02d
0044d02d  mov        eax, dword ptr [esp + 0x14]
0044d031  push       0xa
0044d033  push       eax
0044d034  push       0
0044d036  call       dword ptr [0x4980c0]

// block 0044d03c
0044d03c  mov        edi, eax
0044d03e  test       edi, edi
0044d040  je         0x44d093

// block 0044d042
0044d042  push       edi
0044d043  push       0
0044d045  call       dword ptr [0x4980c4]

// block 0044d04b
0044d04b  mov        ebx, eax
0044d04d  test       ebx, ebx
0044d04f  je         0x44d093

// block 0044d051
0044d051  push       ebx
0044d052  call       dword ptr [0x4980c8]

// block 0044d058
0044d058  mov        ebp, eax
0044d05a  test       ebp, ebp
0044d05c  jne        0x44d077

// block 0044d05e
0044d05e  push       eax
0044d05f  call       dword ptr [0x4980cc]

// block 0044d065
0044d065  mov        edx, dword ptr [esi]
0044d067  mov        eax, dword ptr [edx + 4]
0044d06a  mov        ecx, esi
0044d06c  call       eax

// block 0044d06e
0044d06e  pop        edi
0044d06f  pop        esi
0044d070  pop        ebp
0044d071  mov        al, 1
0044d073  pop        ebx
0044d074  ret        8

// block 0044d077
0044d077  push       edi
0044d078  push       0
0044d07a  call       dword ptr [0x4980d0]

// block 0044d080
0044d080  push       eax
0044d081  mov        dword ptr [esi + 4], eax
0044d084  call       0x46d04a

// block 0044d089
0044d089  add        esp, 4
0044d08c  mov        dword ptr [esi + 0xc], eax
0044d08f  test       eax, eax
0044d091  jne        0x44d0a5

// block 0044d093
0044d093  mov        edx, dword ptr [esi]
0044d095  mov        eax, dword ptr [edx + 4]
0044d098  mov        ecx, esi
0044d09a  call       eax

// block 0044d09c
0044d09c  pop        edi
0044d09d  pop        esi
0044d09e  pop        ebp
0044d09f  mov        al, 1
0044d0a1  pop        ebx
0044d0a2  ret        8

// block 0044d0a5
0044d0a5  mov        ecx, dword ptr [esi + 4]
0044d0a8  push       ecx
0044d0a9  push       ebp
0044d0aa  push       eax
0044d0ab  call       0x47a330

// block 0044d0b0
0044d0b0  mov        edx, dword ptr [esi + 0xc]
0044d0b3  add        esp, 0xc
0044d0b6  push       ebx
0044d0b7  mov        dword ptr [esi + 8], edx
0044d0ba  call       dword ptr [0x4980cc]

// block 0044d0c0
0044d0c0  pop        edi
0044d0c1  pop        esi
0044d0c2  pop        ebp
0044d0c3  mov        al, 1
0044d0c5  pop        ebx
0044d0c6  ret        8
