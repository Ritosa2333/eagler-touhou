004be2d0 55                       push       ebp
004be2d1 8bec                     mov        ebp, esp
004be2d3 83ec10                   sub        esp, 0x10
004be2d6 894df8                   mov        dword ptr [ebp - 8], ecx
004be2d9 6a00                     push       0
004be2db 6a0a                     push       0xa
004be2dd 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004be2e0 50                       push       eax
004be2e1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004be2e4 51                       push       ecx
004be2e5 e8164e0800               call       0x543100
004be2ea 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004be2ed 894dfc                   mov        dword ptr [ebp - 4], ecx
004be2f0 8945f0                   mov        dword ptr [ebp - 0x10], eax
004be2f3 8955f4                   mov        dword ptr [ebp - 0xc], edx
004be2f6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004be2f9 8b82dc000000             mov        eax, dword ptr [edx + 0xdc]
004be2ff 3b45f4                   cmp        eax, dword ptr [ebp - 0xc]
004be302 7730                     ja         0x4be334
004be304 720e                     jb         0x4be314
004be306 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004be309 8b91d8000000             mov        edx, dword ptr [ecx + 0xd8]
004be30f 3b55f0                   cmp        edx, dword ptr [ebp - 0x10]
004be312 7320                     jae        0x4be334
004be314 6a00                     push       0
004be316 6a0a                     push       0xa
004be318 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004be31b 50                       push       eax
004be31c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004be31f 51                       push       ecx
004be320 e8db4d0800               call       0x543100
004be325 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004be328 8981d8000000             mov        dword ptr [ecx + 0xd8], eax
004be32e 8991dc000000             mov        dword ptr [ecx + 0xdc], edx
004be334 8be5                     mov        esp, ebp
004be336 5d                       pop        ebp
004be337 c20800                   ret        8
004be33a cc                       int3       
004be33b cc                       int3       
004be33c cc                       int3       
004be33d cc                       int3       
004be33e cc                       int3       
004be33f cc                       int3       
