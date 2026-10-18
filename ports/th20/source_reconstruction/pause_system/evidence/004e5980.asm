004e5980 55                       push       ebp
004e5981 8bec                     mov        ebp, esp
004e5983 51                       push       ecx
004e5984 894dfc                   mov        dword ptr [ebp - 4], ecx
004e5987 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e598a 50                       push       eax
004e598b 6820654e00               push       0x4e6520
004e5990 6a0f                     push       0xf
004e5992 e879c9f2ff               call       0x412310
004e5997 83c40c                   add        esp, 0xc
004e599a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e599d 894108                   mov        dword ptr [ecx + 8], eax
004e59a0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e59a3 52                       push       edx
004e59a4 68a0664e00               push       0x4e66a0
004e59a9 6a5f                     push       0x5f
004e59ab e800caf2ff               call       0x4123b0
004e59b0 83c40c                   add        esp, 0xc
004e59b3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e59b6 89410c                   mov        dword ptr [ecx + 0xc], eax
004e59b9 6a00                     push       0
004e59bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e59be 83c110                   add        ecx, 0x10
004e59c1 e85adbf3ff               call       0x423520
004e59c6 6a00                     push       0
004e59c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e59cb 83c120                   add        ecx, 0x20
004e59ce e84ddbf3ff               call       0x423520
004e59d3 33c0                     xor        eax, eax
004e59d5 8be5                     mov        esp, ebp
004e59d7 5d                       pop        ebp
004e59d8 c3                       ret        
004e59d9 cc                       int3       
004e59da cc                       int3       
004e59db cc                       int3       
004e59dc cc                       int3       
004e59dd cc                       int3       
004e59de cc                       int3       
004e59df cc                       int3       
