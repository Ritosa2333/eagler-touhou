00534e30 55                       push       ebp
00534e31 8bec                     mov        ebp, esp
00534e33 51                       push       ecx
00534e34 8b4508                   mov        eax, dword ptr [ebp + 8]
00534e37 50                       push       eax
00534e38 8d4dff                   lea        ecx, [ebp - 1]
00534e3b e8d0000000               call       0x534f10
00534e40 8be5                     mov        esp, ebp
00534e42 5d                       pop        ebp
00534e43 c3                       ret        
00534e44 cc                       int3       
00534e45 cc                       int3       
00534e46 cc                       int3       
00534e47 cc                       int3       
00534e48 cc                       int3       
00534e49 cc                       int3       
00534e4a cc                       int3       
00534e4b cc                       int3       
00534e4c cc                       int3       
00534e4d cc                       int3       
00534e4e cc                       int3       
00534e4f cc                       int3       
