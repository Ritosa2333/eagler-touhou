
// block 0044bf00
0044bf00  push       ecx
0044bf01  cmp        dword ptr [ecx + 8], 0x40000000
0044bf08  mov        dword ptr [esp], 0
0044bf0f  je         0x44bf17

// block 0044bf11
0044bf11  xor        al, al
0044bf13  pop        ecx
0044bf14  ret        8

// block 0044bf17
0044bf17  mov        edx, dword ptr [esp + 8]

// block 0044bf1b
0044bf1b  push       esi
0044bf1c  mov        esi, dword ptr [esp + 0x10]
0044bf20  push       0
0044bf22  lea        eax, [esp + 8]
0044bf26  push       eax
0044bf27  mov        eax, dword ptr [ecx + 4]
0044bf2a  push       esi
0044bf2b  push       edx
0044bf2c  push       eax
0044bf2d  call       dword ptr [0x4980ac]

// block 0044bf33
0044bf33  cmp        esi, dword ptr [esp + 4]
0044bf37  pop        esi
0044bf38  sete       al
0044bf3b  pop        ecx
0044bf3c  ret        8
