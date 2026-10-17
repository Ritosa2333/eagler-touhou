0051d9b0 55                       push       ebp
0051d9b1 8bec                     mov        ebp, esp
0051d9b3 51                       push       ecx
0051d9b4 894dfc                   mov        dword ptr [ebp - 4], ecx
0051d9b7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d9ba 8b08                     mov        ecx, dword ptr [eax]
0051d9bc 83e1fe                   and        ecx, 0xfffffffe
0051d9bf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d9c2 890a                     mov        dword ptr [edx], ecx
0051d9c4 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d9c7 8b08                     mov        ecx, dword ptr [eax]
0051d9c9 83e1fd                   and        ecx, 0xfffffffd
0051d9cc 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d9cf 890a                     mov        dword ptr [edx], ecx
0051d9d1 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d9d4 8b08                     mov        ecx, dword ptr [eax]
0051d9d6 83e1fb                   and        ecx, 0xfffffffb
0051d9d9 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d9dc 890a                     mov        dword ptr [edx], ecx
0051d9de 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d9e1 8b08                     mov        ecx, dword ptr [eax]
0051d9e3 83e1f7                   and        ecx, 0xfffffff7
0051d9e6 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051d9e9 890a                     mov        dword ptr [edx], ecx
0051d9eb 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051d9ee 8be5                     mov        esp, ebp
0051d9f0 5d                       pop        ebp
0051d9f1 c3                       ret        
0051d9f2 cc                       int3       
0051d9f3 cc                       int3       
0051d9f4 cc                       int3       
0051d9f5 cc                       int3       
0051d9f6 cc                       int3       
0051d9f7 cc                       int3       
0051d9f8 cc                       int3       
0051d9f9 cc                       int3       
0051d9fa cc                       int3       
0051d9fb cc                       int3       
0051d9fc cc                       int3       
0051d9fd cc                       int3       
0051d9fe cc                       int3       
0051d9ff cc                       int3       
