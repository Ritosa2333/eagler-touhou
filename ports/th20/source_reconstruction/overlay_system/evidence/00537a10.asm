00537a10 55                       push       ebp
00537a11 8bec                     mov        ebp, esp
00537a13 51                       push       ecx
00537a14 894dfc                   mov        dword ptr [ebp - 4], ecx
00537a17 8b4508                   mov        eax, dword ptr [ebp + 8]
00537a1a c78020010000c07e5300     mov        dword ptr [eax + 0x120], 0x537ec0
00537a24 8be5                     mov        esp, ebp
00537a26 5d                       pop        ebp
00537a27 c20800                   ret        8
00537a2a cc                       int3       
00537a2b cc                       int3       
00537a2c cc                       int3       
00537a2d cc                       int3       
00537a2e cc                       int3       
00537a2f cc                       int3       
