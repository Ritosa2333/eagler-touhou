00535350 55                       push       ebp
00535351 8bec                     mov        ebp, esp
00535353 51                       push       ecx
00535354 894dfc                   mov        dword ptr [ebp - 4], ecx
00535357 8b4508                   mov        eax, dword ptr [ebp + 8]
0053535a c78020010000e0e54000     mov        dword ptr [eax + 0x120], 0x40e5e0
00535364 8be5                     mov        esp, ebp
00535366 5d                       pop        ebp
00535367 c20800                   ret        8
0053536a cc                       int3       
0053536b cc                       int3       
0053536c cc                       int3       
0053536d cc                       int3       
0053536e cc                       int3       
0053536f cc                       int3       
