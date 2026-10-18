004bf990 55                       push       ebp
004bf991 8bec                     mov        ebp, esp
004bf993 51                       push       ecx
004bf994 894dfc                   mov        dword ptr [ebp - 4], ecx
004bf997 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bf99a 50                       push       eax
004bf99b 68d0fa4b00               push       0x4bfad0
004bf9a0 6a10                     push       0x10
004bf9a2 e86929f5ff               call       0x412310
004bf9a7 83c40c                   add        esp, 0xc
004bf9aa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bf9ad 894108                   mov        dword ptr [ecx + 8], eax
004bf9b0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bf9b3 52                       push       edx
004bf9b4 68a0de4900               push       0x49dea0
004bf9b9 6a5c                     push       0x5c
004bf9bb e8f029f5ff               call       0x4123b0
004bf9c0 83c40c                   add        esp, 0xc
004bf9c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bf9c6 89410c                   mov        dword ptr [ecx + 0xc], eax
004bf9c9 6a00                     push       0
004bf9cb 6830f94b00               push       0x4bf930
004bf9d0 b9404d5c00               mov        ecx, 0x5c4d40
004bf9d5 e816a0ffff               call       0x4b99f0
004bf9da 6a00                     push       0
004bf9dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bf9df 83c114                   add        ecx, 0x14
004bf9e2 e8393bf6ff               call       0x423520
004bf9e7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bf9ea c7421000000000           mov        dword ptr [edx + 0x10], 0
004bf9f1 33c0                     xor        eax, eax
004bf9f3 8be5                     mov        esp, ebp
004bf9f5 5d                       pop        ebp
004bf9f6 c3                       ret        
004bf9f7 cc                       int3       
004bf9f8 cc                       int3       
004bf9f9 cc                       int3       
004bf9fa cc                       int3       
004bf9fb cc                       int3       
004bf9fc cc                       int3       
004bf9fd cc                       int3       
004bf9fe cc                       int3       
004bf9ff cc                       int3       
