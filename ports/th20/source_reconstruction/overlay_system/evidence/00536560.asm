00536560 55                       push       ebp
00536561 8bec                     mov        ebp, esp
00536563 51                       push       ecx
00536564 894dfc                   mov        dword ptr [ebp - 4], ecx
00536567 6a00                     push       0
00536569 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053656c 83c138                   add        ecx, 0x38
0053656f e8accfeeff               call       0x423520
00536574 90                       nop        
00536575 8be5                     mov        esp, ebp
00536577 5d                       pop        ebp
00536578 c3                       ret        
00536579 cc                       int3       
0053657a cc                       int3       
0053657b cc                       int3       
0053657c cc                       int3       
0053657d cc                       int3       
0053657e cc                       int3       
0053657f cc                       int3       
