004c8190 55                       push       ebp
004c8191 8bec                     mov        ebp, esp
004c8193 83ec0c                   sub        esp, 0xc
004c8196 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c8199 68f8180000               push       0x18f8
004c819e e8baa50700               call       0x54275d
004c81a3 83c404                   add        esp, 4
004c81a6 8945fc                   mov        dword ptr [ebp - 4], eax
004c81a9 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c81ad 742d                     je         0x4c81dc
004c81af 68f8180000               push       0x18f8
004c81b4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c81b7 e8c43ef4ff               call       0x40c080
004c81bc 68f8180000               push       0x18f8
004c81c1 6a00                     push       0
004c81c3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c81c6 50                       push       eax
004c81c7 e894c50700               call       0x544760
004c81cc 83c40c                   add        esp, 0xc
004c81cf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c81d2 e809070000               call       0x4c88e0
004c81d7 8945f8                   mov        dword ptr [ebp - 8], eax
004c81da eb07                     jmp        0x4c81e3
004c81dc c745f800000000           mov        dword ptr [ebp - 8], 0
004c81e3 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c81e6 8be5                     mov        esp, ebp
004c81e8 5d                       pop        ebp
004c81e9 c20400                   ret        4
004c81ec cc                       int3       
004c81ed cc                       int3       
004c81ee cc                       int3       
004c81ef cc                       int3       
