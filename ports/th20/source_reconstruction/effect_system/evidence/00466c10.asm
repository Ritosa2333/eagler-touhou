00466c10 55                       push       ebp
00466c11 8bec                     mov        ebp, esp
00466c13 83ec0c                   sub        esp, 0xc
00466c16 894dfc                   mov        dword ptr [ebp - 4], ecx
00466c19 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00466c1c 81c1f0000000             add        ecx, 0xf0
00466c22 e87951faff               call       0x40bda0
00466c27 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00466c2a 8d1488                   lea        edx, [eax + ecx*4]
00466c2d 8955f8                   mov        dword ptr [ebp - 8], edx
00466c30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00466c33 e86851faff               call       0x40bda0
00466c38 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00466c3b 8d14c8                   lea        edx, [eax + ecx*8]
00466c3e 8955f4                   mov        dword ptr [ebp - 0xc], edx
00466c41 8b45f8                   mov        eax, dword ptr [ebp - 8]
00466c44 50                       push       eax
00466c45 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00466c48 51                       push       ecx
00466c49 8b5508                   mov        edx, dword ptr [ebp + 8]
00466c4c 52                       push       edx
00466c4d 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00466c50 50                       push       eax
00466c51 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00466c57 e84465fdff               call       0x43d1a0
00466c5c 90                       nop        
00466c5d 8be5                     mov        esp, ebp
00466c5f 5d                       pop        ebp
00466c60 c20c00                   ret        0xc
00466c63 cc                       int3       
00466c64 cc                       int3       
00466c65 cc                       int3       
00466c66 cc                       int3       
00466c67 cc                       int3       
00466c68 cc                       int3       
00466c69 cc                       int3       
00466c6a cc                       int3       
00466c6b cc                       int3       
00466c6c cc                       int3       
00466c6d cc                       int3       
00466c6e cc                       int3       
00466c6f cc                       int3       
