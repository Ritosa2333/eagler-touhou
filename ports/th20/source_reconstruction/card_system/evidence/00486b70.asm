00486b70 55                       push       ebp
00486b71 8bec                     mov        ebp, esp
00486b73 83ec08                   sub        esp, 8
00486b76 894dfc                   mov        dword ptr [ebp - 4], ecx
00486b79 8b45fc                   mov        eax, dword ptr [ebp - 4]
00486b7c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00486b7f 8908                     mov        dword ptr [eax], ecx
00486b81 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486b84 83c204                   add        edx, 4
00486b87 8955f8                   mov        dword ptr [ebp - 8], edx
00486b8a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00486b8d 50                       push       eax
00486b8e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00486b91 e8ea3bfeff               call       0x46a780
00486b96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00486b99 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00486b9c 895120                   mov        dword ptr [ecx + 0x20], edx
00486b9f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00486ba2 8be5                     mov        esp, ebp
00486ba4 5d                       pop        ebp
00486ba5 c20c00                   ret        0xc
00486ba8 cc                       int3       
00486ba9 cc                       int3       
00486baa cc                       int3       
00486bab cc                       int3       
00486bac cc                       int3       
00486bad cc                       int3       
00486bae cc                       int3       
00486baf cc                       int3       
