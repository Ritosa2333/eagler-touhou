00429420 55                       push       ebp
00429421 8bec                     mov        ebp, esp
00429423 51                       push       ecx
00429424 894dfc                   mov        dword ptr [ebp - 4], ecx
00429427 6a01                     push       1
00429429 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042942c e83f0a0000               call       0x429e70
00429431 90                       nop        
00429432 8be5                     mov        esp, ebp
00429434 5d                       pop        ebp
00429435 c20400                   ret        4
00429438 cc                       int3       
00429439 cc                       int3       
0042943a cc                       int3       
0042943b cc                       int3       
0042943c cc                       int3       
0042943d cc                       int3       
0042943e cc                       int3       
0042943f cc                       int3       
