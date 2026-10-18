00412dc0 55                       push       ebp
00412dc1 8bec                     mov        ebp, esp
00412dc3 51                       push       ecx
00412dc4 894dfc                   mov        dword ptr [ebp - 4], ecx
00412dc7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00412dca 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00412dcd 89480c                   mov        dword ptr [eax + 0xc], ecx
00412dd0 8be5                     mov        esp, ebp
00412dd2 5d                       pop        ebp
00412dd3 c20400                   ret        4
00412dd6 cc                       int3       
00412dd7 cc                       int3       
00412dd8 cc                       int3       
00412dd9 cc                       int3       
00412dda cc                       int3       
00412ddb cc                       int3       
00412ddc cc                       int3       
00412ddd cc                       int3       
00412dde cc                       int3       
00412ddf cc                       int3       
