00422c30 55                       push       ebp
00422c31 8bec                     mov        ebp, esp
00422c33 51                       push       ecx
00422c34 894dfc                   mov        dword ptr [ebp - 4], ecx
00422c37 6a01                     push       1
00422c39 e8c2ffffff               call       0x422c00
00422c3e 83c404                   add        esp, 4
00422c41 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422c44 8901                     mov        dword ptr [ecx], eax
00422c46 8b45fc                   mov        eax, dword ptr [ebp - 4]
00422c49 8be5                     mov        esp, ebp
00422c4b 5d                       pop        ebp
00422c4c c3                       ret        
00422c4d cc                       int3       
00422c4e cc                       int3       
00422c4f cc                       int3       
