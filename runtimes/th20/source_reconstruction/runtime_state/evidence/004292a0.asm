004292a0 55                       push       ebp
004292a1 8bec                     mov        ebp, esp
004292a3 51                       push       ecx
004292a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004292a7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004292aa f30f104508               movss      xmm0, dword ptr [ebp + 8]
004292af f30f1100                 movss      dword ptr [eax], xmm0
004292b3 8be5                     mov        esp, ebp
004292b5 5d                       pop        ebp
004292b6 c20400                   ret        4
004292b9 cc                       int3       
004292ba cc                       int3       
004292bb cc                       int3       
004292bc cc                       int3       
004292bd cc                       int3       
004292be cc                       int3       
004292bf cc                       int3       
