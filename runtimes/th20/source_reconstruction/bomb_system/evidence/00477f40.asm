00477f40 55                       push       ebp
00477f41 8bec                     mov        ebp, esp
00477f43 51                       push       ecx
00477f44 894dfc                   mov        dword ptr [ebp - 4], ecx
00477f47 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477f4a 8b484c                   mov        ecx, dword ptr [eax + 0x4c]
00477f4d 034d08                   add        ecx, dword ptr [ebp + 8]
00477f50 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477f53 894a4c                   mov        dword ptr [edx + 0x4c], ecx
00477f56 8be5                     mov        esp, ebp
00477f58 5d                       pop        ebp
00477f59 c20400                   ret        4
00477f5c cc                       int3       
00477f5d cc                       int3       
00477f5e cc                       int3       
00477f5f cc                       int3       
