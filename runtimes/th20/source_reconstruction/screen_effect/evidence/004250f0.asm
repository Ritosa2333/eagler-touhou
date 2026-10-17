004250f0 55                       push       ebp
004250f1 8bec                     mov        ebp, esp
004250f3 51                       push       ecx
004250f4 6868d75600               push       0x56d768
004250f9 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004250ff e83cdcffff               call       0x422d40
00425104 8945fc                   mov        dword ptr [ebp - 4], eax
00425107 837dfc00                 cmp        dword ptr [ebp - 4], 0
0042510b 7504                     jne        0x425111
0042510d 33c0                     xor        eax, eax
0042510f eb2c                     jmp        0x42513d
00425111 8b45fc                   mov        eax, dword ptr [ebp - 4]
00425114 8b4d20                   mov        ecx, dword ptr [ebp + 0x20]
00425117 894840                   mov        dword ptr [eax + 0x40], ecx
0042511a 8b551c                   mov        edx, dword ptr [ebp + 0x1c]
0042511d 52                       push       edx
0042511e 8b4518                   mov        eax, dword ptr [ebp + 0x18]
00425121 50                       push       eax
00425122 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00425125 51                       push       ecx
00425126 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00425129 52                       push       edx
0042512a 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0042512d 50                       push       eax
0042512e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00425131 51                       push       ecx
00425132 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00425135 e8b6eaffff               call       0x423bf0
0042513a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042513d 8be5                     mov        esp, ebp
0042513f 5d                       pop        ebp
00425140 c3                       ret        
00425141 cc                       int3       
00425142 cc                       int3       
00425143 cc                       int3       
00425144 cc                       int3       
00425145 cc                       int3       
00425146 cc                       int3       
00425147 cc                       int3       
00425148 cc                       int3       
00425149 cc                       int3       
0042514a cc                       int3       
0042514b cc                       int3       
0042514c cc                       int3       
0042514d cc                       int3       
0042514e cc                       int3       
0042514f cc                       int3       
