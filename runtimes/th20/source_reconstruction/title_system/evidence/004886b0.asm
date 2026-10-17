004886b0 55                       push       ebp
004886b1 8bec                     mov        ebp, esp
004886b3 83ec08                   sub        esp, 8
004886b6 c745f800000000           mov        dword ptr [ebp - 8], 0
004886bd c745fc00000000           mov        dword ptr [ebp - 4], 0
004886c4 eb09                     jmp        0x4886cf
004886c6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004886c9 83c001                   add        eax, 1
004886cc 8945fc                   mov        dword ptr [ebp - 4], eax
004886cf 837dfc71                 cmp        dword ptr [ebp - 4], 0x71
004886d3 7d1a                     jge        0x4886ef
004886d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004886d8 8b148d58f05a00           mov        edx, dword ptr [ecx*4 + 0x5af058]
004886df 3b5508                   cmp        edx, dword ptr [ebp + 8]
004886e2 7509                     jne        0x4886ed
004886e4 8b45f8                   mov        eax, dword ptr [ebp - 8]
004886e7 83c001                   add        eax, 1
004886ea 8945f8                   mov        dword ptr [ebp - 8], eax
004886ed ebd7                     jmp        0x4886c6
004886ef 8b45f8                   mov        eax, dword ptr [ebp - 8]
004886f2 8be5                     mov        esp, ebp
004886f4 5d                       pop        ebp
004886f5 c3                       ret        
004886f6 cc                       int3       
004886f7 cc                       int3       
004886f8 cc                       int3       
004886f9 cc                       int3       
004886fa cc                       int3       
004886fb cc                       int3       
004886fc cc                       int3       
004886fd cc                       int3       
004886fe cc                       int3       
004886ff cc                       int3       
