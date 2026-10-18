00422c50 55                       push       ebp
00422c51 8bec                     mov        ebp, esp
00422c53 51                       push       ecx
00422c54 894dfc                   mov        dword ptr [ebp - 4], ecx
00422c57 8b45fc                   mov        eax, dword ptr [ebp - 4]
00422c5a c70000000000             mov        dword ptr [eax], 0
00422c60 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422c63 83c104                   add        ecx, 4
00422c66 e8c5ffffff               call       0x422c30
00422c6b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422c6e c7410800000000           mov        dword ptr [ecx + 8], 0
00422c75 8b55fc                   mov        edx, dword ptr [ebp - 4]
00422c78 c7420c00ffff00           mov        dword ptr [edx + 0xc], 0xffff00
00422c7f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00422c82 c7401000000000           mov        dword ptr [eax + 0x10], 0
00422c89 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00422c8c c7411400000000           mov        dword ptr [ecx + 0x14], 0
00422c93 8b55fc                   mov        edx, dword ptr [ebp - 4]
00422c96 8b4508                   mov        eax, dword ptr [ebp + 8]
00422c99 894218                   mov        dword ptr [edx + 0x18], eax
00422c9c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00422c9f 8be5                     mov        esp, ebp
00422ca1 5d                       pop        ebp
00422ca2 c20400                   ret        4
00422ca5 cc                       int3       
00422ca6 cc                       int3       
00422ca7 cc                       int3       
00422ca8 cc                       int3       
00422ca9 cc                       int3       
00422caa cc                       int3       
00422cab cc                       int3       
00422cac cc                       int3       
00422cad cc                       int3       
00422cae cc                       int3       
00422caf cc                       int3       
