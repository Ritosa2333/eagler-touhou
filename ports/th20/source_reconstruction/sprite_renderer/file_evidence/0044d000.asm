0044d000 55                       push       ebp
0044d001 8bec                     mov        ebp, esp
0044d003 51                       push       ecx
0044d004 894dfc                   mov        dword ptr [ebp - 4], ecx
0044d007 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044d00a 8b00                     mov        eax, dword ptr [eax]
0044d00c c1e008                   shl        eax, 8
0044d00f 0b4508                   or         eax, dword ptr [ebp + 8]
0044d012 8be5                     mov        esp, ebp
0044d014 5d                       pop        ebp
0044d015 c20400                   ret        4
0044d018 cc                       int3       
0044d019 cc                       int3       
0044d01a cc                       int3       
0044d01b cc                       int3       
0044d01c cc                       int3       
0044d01d cc                       int3       
0044d01e cc                       int3       
0044d01f cc                       int3       
