0045c2e0 55                       push       ebp
0045c2e1 8bec                     mov        ebp, esp
0045c2e3 51                       push       ecx
0045c2e4 894dfc                   mov        dword ptr [ebp - 4], ecx
0045c2e7 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045c2ea 8b4004                   mov        eax, dword ptr [eax + 4]
0045c2ed 0faf4508                 imul       eax, dword ptr [ebp + 8]
0045c2f1 8be5                     mov        esp, ebp
0045c2f3 5d                       pop        ebp
0045c2f4 c20400                   ret        4
0045c2f7 cc                       int3       
0045c2f8 cc                       int3       
0045c2f9 cc                       int3       
0045c2fa cc                       int3       
0045c2fb cc                       int3       
0045c2fc cc                       int3       
0045c2fd cc                       int3       
0045c2fe cc                       int3       
0045c2ff cc                       int3       
