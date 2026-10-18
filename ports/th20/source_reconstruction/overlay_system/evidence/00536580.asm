00536580 55                       push       ebp
00536581 8bec                     mov        ebp, esp
00536583 51                       push       ecx
00536584 894dfc                   mov        dword ptr [ebp - 4], ecx
00536587 8b4508                   mov        eax, dword ptr [ebp + 8]
0053658a c78020010000c06a5300     mov        dword ptr [eax + 0x120], 0x536ac0
00536594 8be5                     mov        esp, ebp
00536596 5d                       pop        ebp
00536597 c20800                   ret        8
0053659a cc                       int3       
0053659b cc                       int3       
0053659c cc                       int3       
0053659d cc                       int3       
0053659e cc                       int3       
0053659f cc                       int3       
