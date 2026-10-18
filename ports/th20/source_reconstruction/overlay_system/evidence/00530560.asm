00530560 55                       push       ebp
00530561 8bec                     mov        ebp, esp
00530563 51                       push       ecx
00530564 894dfc                   mov        dword ptr [ebp - 4], ecx
00530567 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053056a 8b10                     mov        edx, dword ptr [eax]
0053056c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053056f 8b426c                   mov        eax, dword ptr [edx + 0x6c]
00530572 ffd0                     call       eax
00530574 8be5                     mov        esp, ebp
00530576 5d                       pop        ebp
00530577 c3                       ret        
00530578 cc                       int3       
00530579 cc                       int3       
0053057a cc                       int3       
0053057b cc                       int3       
0053057c cc                       int3       
0053057d cc                       int3       
0053057e cc                       int3       
0053057f cc                       int3       
