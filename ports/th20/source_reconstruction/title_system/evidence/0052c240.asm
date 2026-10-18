0052c240 55                       push       ebp
0052c241 8bec                     mov        ebp, esp
0052c243 83ec0c                   sub        esp, 0xc
0052c246 894dfc                   mov        dword ptr [ebp - 4], ecx
0052c249 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052c24c 0564010000               add        eax, 0x164
0052c251 8945f8                   mov        dword ptr [ebp - 8], eax
0052c254 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0052c257 51                       push       ecx
0052c258 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052c25b e82083eeff               call       0x414580
0052c260 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052c263 6a00                     push       0
0052c265 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0052c268 52                       push       edx
0052c269 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c26c 50                       push       eax
0052c26d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c270 e83b05f2ff               call       0x44c7b0
0052c275 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c278 8be5                     mov        esp, ebp
0052c27a 5d                       pop        ebp
0052c27b c20c00                   ret        0xc
0052c27e cc                       int3       
0052c27f cc                       int3       
