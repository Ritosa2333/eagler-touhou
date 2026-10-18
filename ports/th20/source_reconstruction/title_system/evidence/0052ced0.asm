0052ced0 55                       push       ebp
0052ced1 8bec                     mov        ebp, esp
0052ced3 51                       push       ecx
0052ced4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ced7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ceda 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052cedd 8b5118                   mov        edx, dword ptr [ecx + 0x18]
0052cee0 89501c                   mov        dword ptr [eax + 0x1c], edx
0052cee3 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cee6 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cee9 894818                   mov        dword ptr [eax + 0x18], ecx
0052ceec 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052ceef c7422000000000           mov        dword ptr [edx + 0x20], 0
0052cef6 6a00                     push       0
0052cef8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052cefb 81c154010000             add        ecx, 0x154
0052cf01 e81a66efff               call       0x423520
0052cf06 90                       nop        
0052cf07 8be5                     mov        esp, ebp
0052cf09 5d                       pop        ebp
0052cf0a c20400                   ret        4
0052cf0d cc                       int3       
0052cf0e cc                       int3       
0052cf0f cc                       int3       
