004bfde0 55                       push       ebp
004bfde1 8bec                     mov        ebp, esp
004bfde3 83ec0c                   sub        esp, 0xc
004bfde6 894df4                   mov        dword ptr [ebp - 0xc], ecx
004bfde9 6860c40000               push       0xc460
004bfdee e86a290800               call       0x54275d
004bfdf3 83c404                   add        esp, 4
004bfdf6 8945fc                   mov        dword ptr [ebp - 4], eax
004bfdf9 837dfc00                 cmp        dword ptr [ebp - 4], 0
004bfdfd 741a                     je         0x4bfe19
004bfdff 6860c40000               push       0xc460
004bfe04 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfe07 e874c2f4ff               call       0x40c080
004bfe0c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfe0f e8cc000000               call       0x4bfee0
004bfe14 8945f8                   mov        dword ptr [ebp - 8], eax
004bfe17 eb07                     jmp        0x4bfe20
004bfe19 c745f800000000           mov        dword ptr [ebp - 8], 0
004bfe20 8b45f8                   mov        eax, dword ptr [ebp - 8]
004bfe23 8be5                     mov        esp, ebp
004bfe25 5d                       pop        ebp
004bfe26 c20400                   ret        4
004bfe29 cc                       int3       
004bfe2a cc                       int3       
004bfe2b cc                       int3       
004bfe2c cc                       int3       
004bfe2d cc                       int3       
004bfe2e cc                       int3       
004bfe2f cc                       int3       
