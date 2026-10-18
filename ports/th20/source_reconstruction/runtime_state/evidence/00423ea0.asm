00423ea0 55                       push       ebp
00423ea1 8bec                     mov        ebp, esp
00423ea3 83ec08                   sub        esp, 8
00423ea6 894df8                   mov        dword ptr [ebp - 8], ecx
00423ea9 837d0800                 cmp        dword ptr [ebp + 8], 0
00423ead 7412                     je         0x423ec1
00423eaf 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00423eb2 e829000000               call       0x423ee0
00423eb7 33d2                     xor        edx, edx
00423eb9 f77508                   div        dword ptr [ebp + 8]
00423ebc 8955fc                   mov        dword ptr [ebp - 4], edx
00423ebf eb07                     jmp        0x423ec8
00423ec1 c745fc00000000           mov        dword ptr [ebp - 4], 0
00423ec8 8b45fc                   mov        eax, dword ptr [ebp - 4]
00423ecb 8be5                     mov        esp, ebp
00423ecd 5d                       pop        ebp
00423ece c20400                   ret        4
00423ed1 cc                       int3       
00423ed2 cc                       int3       
00423ed3 cc                       int3       
00423ed4 cc                       int3       
00423ed5 cc                       int3       
00423ed6 cc                       int3       
00423ed7 cc                       int3       
00423ed8 cc                       int3       
00423ed9 cc                       int3       
00423eda cc                       int3       
00423edb cc                       int3       
00423edc cc                       int3       
00423edd cc                       int3       
00423ede cc                       int3       
00423edf cc                       int3       
