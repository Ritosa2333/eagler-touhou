00483010 55                       push       ebp
00483011 8bec                     mov        ebp, esp
00483013 83ec18                   sub        esp, 0x18
00483016 894dfc                   mov        dword ptr [ebp - 4], ecx
00483019 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048301c 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00483022 e8d992f8ff               call       0x40c300
00483027 8bc8                     mov        ecx, eax
00483029 e862290000               call       0x485990
0048302e 0f57c0                   xorps      xmm0, xmm0
00483031 0f2f00                   comiss     xmm0, dword ptr [eax]
00483034 7222                     jb         0x483058
00483036 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483039 81c1a0000000             add        ecx, 0xa0
0048303f 894df0                   mov        dword ptr [ebp - 0x10], ecx
00483042 6a04                     push       4
00483044 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00483047 e83496ffff               call       0x47c680
0048304c f30f104018               movss      xmm0, dword ptr [eax + 0x18]
00483051 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00483056 eb1e                     jmp        0x483076
00483058 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048305b 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00483061 e89a92f8ff               call       0x40c300
00483066 8bc8                     mov        ecx, eax
00483068 e823290000               call       0x485990
0048306d f30f1000                 movss      xmm0, dword ptr [eax]
00483071 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00483076 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
0048307b f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00483080 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00483085 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048308d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483090 f30f104864               movss      xmm1, dword ptr [eax + 0x64]
00483095 0f2fc8                   comiss     xmm1, xmm0
00483098 0f82ab000000             jb         0x483149
0048309e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004830a1 81c1a0000000             add        ecx, 0xa0
004830a7 894dec                   mov        dword ptr [ebp - 0x14], ecx
004830aa 6a04                     push       4
004830ac 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004830af e8cc95ffff               call       0x47c680
004830b4 8b503c                   mov        edx, dword ptr [eax + 0x3c]
004830b7 83e210                   and        edx, 0x10
004830ba 0f8582000000             jne        0x483142
004830c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004830c3 81c184000000             add        ecx, 0x84
004830c9 e81262faff               call       0x4292e0
004830ce d95de8                   fstp       dword ptr [ebp - 0x18]
004830d1 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004830d6 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
004830dd f30f5c05f0e05600         subss      xmm0, dword ptr [0x56e0f0]
004830e5 51                       push       ecx
004830e6 f30f110424               movss      dword ptr [esp], xmm0
004830eb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004830ee 81c184000000             add        ecx, 0x84
004830f4 e827fefcff               call       0x452f20
004830f9 51                       push       ecx
004830fa 0f57c0                   xorps      xmm0, xmm0
004830fd f30f110424               movss      dword ptr [esp], xmm0
00483102 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483105 81c184000000             add        ecx, 0x84
0048310b e8d061faff               call       0x4292e0
00483110 51                       push       ecx
00483111 d91c24                   fstp       dword ptr [esp]
00483114 e8e754fbff               call       0x438600
00483119 83c404                   add        esp, 4
0048311c d91c24                   fstp       dword ptr [esp]
0048311f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483122 81c184000000             add        ecx, 0x84
00483128 e8f3fdfcff               call       0x452f20
0048312d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483130 f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00483135 f30f5c4064               subss      xmm0, dword ptr [eax + 0x64]
0048313a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048313d f30f114164               movss      dword ptr [ecx + 0x64], xmm0
00483142 b801000000               mov        eax, 1
00483147 eb02                     jmp        0x48314b
00483149 33c0                     xor        eax, eax
0048314b 8be5                     mov        esp, ebp
0048314d 5d                       pop        ebp
0048314e c3                       ret        
0048314f cc                       int3       
