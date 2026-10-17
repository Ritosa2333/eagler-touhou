00532440 55                       push       ebp
00532441 8bec                     mov        ebp, esp
00532443 51                       push       ecx
00532444 894dfc                   mov        dword ptr [ebp - 4], ecx
00532447 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053244a 8b10                     mov        edx, dword ptr [eax]
0053244c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053244f 8b4270                   mov        eax, dword ptr [edx + 0x70]
00532452 ffd0                     call       eax
00532454 8be5                     mov        esp, ebp
00532456 5d                       pop        ebp
00532457 c3                       ret        
00532458 cc                       int3       
00532459 cc                       int3       
0053245a cc                       int3       
0053245b cc                       int3       
0053245c cc                       int3       
0053245d cc                       int3       
0053245e cc                       int3       
0053245f cc                       int3       
