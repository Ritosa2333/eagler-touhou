00537a30 55                       push       ebp
00537a31 8bec                     mov        ebp, esp
00537a33 83ec0c                   sub        esp, 0xc
00537a36 894dfc                   mov        dword ptr [ebp - 4], ecx
00537a39 e8e28affff               call       0x530520
00537a3e 0fb6c0                   movzx      eax, al
00537a41 85c0                     test       eax, eax
00537a43 753a                     jne        0x537a7f
00537a45 6a00                     push       0
00537a47 e8e48df2ff               call       0x460830
00537a4c 83c404                   add        esp, 4
00537a4f 8bc8                     mov        ecx, eax
00537a51 e85a7cfcff               call       0x4ff6b0
00537a56 8945f4                   mov        dword ptr [ebp - 0xc], eax
00537a59 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537a5c 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
00537a60 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00537a63 8d4c0205                 lea        ecx, [edx + eax + 5]
00537a67 894df8                   mov        dword ptr [ebp - 8], ecx
00537a6a 8b55f8                   mov        edx, dword ptr [ebp - 8]
00537a6d 52                       push       edx
00537a6e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00537a71 50                       push       eax
00537a72 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00537a75 51                       push       ecx
00537a76 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00537a79 e8c2d2fcff               call       0x504d40
00537a7e 90                       nop        
00537a7f 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00537a82 52                       push       edx
00537a83 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537a86 e815000000               call       0x537aa0
00537a8b 90                       nop        
00537a8c 8be5                     mov        esp, ebp
00537a8e 5d                       pop        ebp
00537a8f c20c00                   ret        0xc
00537a92 cc                       int3       
00537a93 cc                       int3       
00537a94 cc                       int3       
00537a95 cc                       int3       
00537a96 cc                       int3       
00537a97 cc                       int3       
00537a98 cc                       int3       
00537a99 cc                       int3       
00537a9a cc                       int3       
00537a9b cc                       int3       
00537a9c cc                       int3       
00537a9d cc                       int3       
00537a9e cc                       int3       
00537a9f cc                       int3       
