004d3ca0 55                       push       ebp
004d3ca1 8bec                     mov        ebp, esp
004d3ca3 83ec10                   sub        esp, 0x10
004d3ca6 894dfc                   mov        dword ptr [ebp - 4], ecx
004d3ca9 6880fc5600               push       0x56fc80
004d3cae 6a07                     push       7
004d3cb0 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004d3cb6 e875aef7ff               call       0x44eb30
004d3cbb 8945f8                   mov        dword ptr [ebp - 8], eax
004d3cbe 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d3cc1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d3cc4 894848                   mov        dword ptr [eax + 0x48], ecx
004d3cc7 837df800                 cmp        dword ptr [ebp - 8], 0
004d3ccb 7517                     jne        0x4d3ce4
004d3ccd 688cfc5600               push       0x56fc8c
004d3cd2 6878065c00               push       0x5c0678
004d3cd7 e87404f8ff               call       0x454150
004d3cdc 83c408                   add        esp, 8
004d3cdf 83c8ff                   or         eax, 0xffffffff
004d3ce2 eb70                     jmp        0x4d3d54
004d3ce4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d3ce7 52                       push       edx
004d3ce8 68105b4d00               push       0x4d5b10
004d3ced 6a25                     push       0x25
004d3cef e81ce6f3ff               call       0x412310
004d3cf4 83c40c                   add        esp, 0xc
004d3cf7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d3cfa 894108                   mov        dword ptr [ecx + 8], eax
004d3cfd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d3d00 52                       push       edx
004d3d01 68b05b4d00               push       0x4d5bb0
004d3d06 6a27                     push       0x27
004d3d08 e8a3e6f3ff               call       0x4123b0
004d3d0d 83c40c                   add        esp, 0xc
004d3d10 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d3d13 89410c                   mov        dword ptr [ecx + 0xc], eax
004d3d16 8b5508                   mov        edx, dword ptr [ebp + 8]
004d3d19 52                       push       edx
004d3d1a b968a55b00               mov        ecx, 0x5ba568
004d3d1f e89c7ef3ff               call       0x40bbc0
004d3d24 8945f4                   mov        dword ptr [ebp - 0xc], eax
004d3d27 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d3d2a 50                       push       eax
004d3d2b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d3d2e e81da1f4ff               call       0x41de50
004d3d33 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d3d36 51                       push       ecx
004d3d37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d3d3a e8d1400000               call       0x4d7e10
004d3d3f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d3d42 83c210                   add        edx, 0x10
004d3d45 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d3d48 6a00                     push       0
004d3d4a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d3d4d e8ee68f7ff               call       0x44a640
004d3d52 33c0                     xor        eax, eax
004d3d54 8be5                     mov        esp, ebp
004d3d56 5d                       pop        ebp
004d3d57 c20400                   ret        4
004d3d5a cc                       int3       
004d3d5b cc                       int3       
004d3d5c cc                       int3       
004d3d5d cc                       int3       
004d3d5e cc                       int3       
004d3d5f cc                       int3       
004d3d60 cc                       int3       
004d3d61 cc                       int3       
004d3d62 cc                       int3       
004d3d63 cc                       int3       
004d3d64 cc                       int3       
004d3d65 cc                       int3       
004d3d66 cc                       int3       
004d3d67 cc                       int3       
004d3d68 cc                       int3       
004d3d69 cc                       int3       
004d3d6a cc                       int3       
004d3d6b cc                       int3       
004d3d6c cc                       int3       
004d3d6d cc                       int3       
004d3d6e cc                       int3       
004d3d6f cc                       int3       
