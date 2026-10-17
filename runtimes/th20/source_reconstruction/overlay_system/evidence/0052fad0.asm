0052fad0 55                       push       ebp
0052fad1 8bec                     mov        ebp, esp
0052fad3 51                       push       ecx
0052fad4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fad7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fada c70010585700             mov        dword ptr [eax], 0x575810
0052fae0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fae3 c7410400000000           mov        dword ptr [ecx + 4], 0
0052faea 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052faed c7420800000000           mov        dword ptr [edx + 8], 0
0052faf4 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052faf7 c7400c00000000           mov        dword ptr [eax + 0xc], 0
0052fafe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fb01 c7411001000000           mov        dword ptr [ecx + 0x10], 1
0052fb08 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fb0b 83c114                   add        ecx, 0x14
0052fb0e e87d32efff               call       0x422d90
0052fb13 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fb16 83c124                   add        ecx, 0x24
0052fb19 e87232efff               call       0x422d90
0052fb1e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052fb21 c6423400                 mov        byte ptr [edx + 0x34], 0
0052fb25 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fb28 c6403500                 mov        byte ptr [eax + 0x35], 0
0052fb2c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fb2f 8be5                     mov        esp, ebp
0052fb31 5d                       pop        ebp
0052fb32 c3                       ret        
0052fb33 cc                       int3       
0052fb34 cc                       int3       
0052fb35 cc                       int3       
0052fb36 cc                       int3       
0052fb37 cc                       int3       
0052fb38 cc                       int3       
0052fb39 cc                       int3       
0052fb3a cc                       int3       
0052fb3b cc                       int3       
0052fb3c cc                       int3       
0052fb3d cc                       int3       
0052fb3e cc                       int3       
0052fb3f cc                       int3       
