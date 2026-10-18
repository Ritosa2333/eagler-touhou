004393f0 55                       push       ebp
004393f1 8bec                     mov        ebp, esp
004393f3 51                       push       ecx
004393f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004393f7 8b4508                   mov        eax, dword ptr [ebp + 8]
004393fa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004393fd 8b10                     mov        edx, dword ptr [eax]
004393ff 8911                     mov        dword ptr [ecx], edx
00439401 8b5004                   mov        edx, dword ptr [eax + 4]
00439404 895104                   mov        dword ptr [ecx + 4], edx
00439407 8b4008                   mov        eax, dword ptr [eax + 8]
0043940a 894108                   mov        dword ptr [ecx + 8], eax
0043940d 8be5                     mov        esp, ebp
0043940f 5d                       pop        ebp
00439410 c20400                   ret        4
00439413 cc                       int3       
00439414 cc                       int3       
00439415 cc                       int3       
00439416 cc                       int3       
00439417 cc                       int3       
00439418 cc                       int3       
00439419 cc                       int3       
0043941a cc                       int3       
0043941b cc                       int3       
0043941c cc                       int3       
0043941d cc                       int3       
0043941e cc                       int3       
0043941f cc                       int3       
