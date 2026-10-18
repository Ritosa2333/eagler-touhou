00538260 55                       push       ebp
00538261 8bec                     mov        ebp, esp
00538263 51                       push       ecx
00538264 894dfc                   mov        dword ptr [ebp - 4], ecx
00538267 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053826a e86178ffff               call       0x52fad0
0053826f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00538272 c70080665700             mov        dword ptr [eax], 0x576680
00538278 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053827b 83c138                   add        ecx, 0x38
0053827e e80dabeeff               call       0x422d90
00538283 90                       nop        
00538284 8b45fc                   mov        eax, dword ptr [ebp - 4]
00538287 8be5                     mov        esp, ebp
00538289 5d                       pop        ebp
0053828a c3                       ret        
0053828b cc                       int3       
0053828c cc                       int3       
0053828d cc                       int3       
0053828e cc                       int3       
0053828f cc                       int3       
