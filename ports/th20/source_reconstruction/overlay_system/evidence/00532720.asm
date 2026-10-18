00532720 55                       push       ebp
00532721 8bec                     mov        ebp, esp
00532723 51                       push       ecx
00532724 8b4508                   mov        eax, dword ptr [ebp + 8]
00532727 50                       push       eax
00532728 8d4dff                   lea        ecx, [ebp - 1]
0053272b e860030000               call       0x532a90
00532730 8be5                     mov        esp, ebp
00532732 5d                       pop        ebp
00532733 c3                       ret        
00532734 cc                       int3       
00532735 cc                       int3       
00532736 cc                       int3       
00532737 cc                       int3       
00532738 cc                       int3       
00532739 cc                       int3       
0053273a cc                       int3       
0053273b cc                       int3       
0053273c cc                       int3       
0053273d cc                       int3       
0053273e cc                       int3       
0053273f cc                       int3       
