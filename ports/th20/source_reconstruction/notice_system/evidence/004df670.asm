004df670 55                       push       ebp
004df671 8bec                     mov        ebp, esp
004df673 51                       push       ecx
004df674 894dfc                   mov        dword ptr [ebp - 4], ecx
004df677 8b45fc                   mov        eax, dword ptr [ebp - 4]
004df67a 50                       push       eax
004df67b 68c0fa4d00               push       0x4dfac0
004df680 6a10                     push       0x10
004df682 e8892cf3ff               call       0x412310
004df687 83c40c                   add        esp, 0xc
004df68a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004df68d 894108                   mov        dword ptr [ecx + 8], eax
004df690 8b55fc                   mov        edx, dword ptr [ebp - 4]
004df693 52                       push       edx
004df694 68a0de4900               push       0x49dea0
004df699 6a5c                     push       0x5c
004df69b e8102df3ff               call       0x4123b0
004df6a0 83c40c                   add        esp, 0xc
004df6a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004df6a6 89410c                   mov        dword ptr [ecx + 0xc], eax
004df6a9 6a00                     push       0
004df6ab 68e0f64d00               push       0x4df6e0
004df6b0 b9404d5c00               mov        ecx, 0x5c4d40
004df6b5 e836a3fdff               call       0x4b99f0
004df6ba 6a00                     push       0
004df6bc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004df6bf 83c114                   add        ecx, 0x14
004df6c2 e8593ef4ff               call       0x423520
004df6c7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004df6ca c7421000000000           mov        dword ptr [edx + 0x10], 0
004df6d1 33c0                     xor        eax, eax
004df6d3 8be5                     mov        esp, ebp
004df6d5 5d                       pop        ebp
004df6d6 c3                       ret        
004df6d7 cc                       int3       
004df6d8 cc                       int3       
004df6d9 cc                       int3       
004df6da cc                       int3       
004df6db cc                       int3       
004df6dc cc                       int3       
004df6dd cc                       int3       
004df6de cc                       int3       
004df6df cc                       int3       
