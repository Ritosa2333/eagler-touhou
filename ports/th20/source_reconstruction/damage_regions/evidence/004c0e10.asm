004c0e10 55                       push       ebp
004c0e11 8bec                     mov        ebp, esp
004c0e13 83ec10                   sub        esp, 0x10
004c0e16 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0e19 6a00                     push       0
004c0e1b e8a080fbff               call       0x478ec0
004c0e20 83c404                   add        esp, 4
004c0e23 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c0e26 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0e29 8b08                     mov        ecx, dword ptr [eax]
004c0e2b 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c0e2e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c0e31 52                       push       edx
004c0e32 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c0e35 e8c6feffff               call       0x4c0d00
004c0e3a 8945f8                   mov        dword ptr [ebp - 8], eax
004c0e3d 837df800                 cmp        dword ptr [ebp - 8], 0
004c0e41 7509                     jne        0x4c0e4c
004c0e43 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0e46 c70000000000             mov        dword ptr [eax], 0
004c0e4c 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c0e4f 8be5                     mov        esp, ebp
004c0e51 5d                       pop        ebp
004c0e52 c3                       ret        
004c0e53 cc                       int3       
004c0e54 cc                       int3       
004c0e55 cc                       int3       
004c0e56 cc                       int3       
004c0e57 cc                       int3       
004c0e58 cc                       int3       
004c0e59 cc                       int3       
004c0e5a cc                       int3       
004c0e5b cc                       int3       
004c0e5c cc                       int3       
004c0e5d cc                       int3       
004c0e5e cc                       int3       
004c0e5f cc                       int3       
