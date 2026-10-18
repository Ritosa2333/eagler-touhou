00534ee0 55                       push       ebp
00534ee1 8bec                     mov        ebp, esp
00534ee3 51                       push       ecx
00534ee4 894dfc                   mov        dword ptr [ebp - 4], ecx
00534ee7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534eea e8e1abffff               call       0x52fad0
00534eef 8b45fc                   mov        eax, dword ptr [ebp - 4]
00534ef2 c70068635700             mov        dword ptr [eax], 0x576368
00534ef8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534efb 83c138                   add        ecx, 0x38
00534efe e88ddeeeff               call       0x422d90
00534f03 90                       nop        
00534f04 8b45fc                   mov        eax, dword ptr [ebp - 4]
00534f07 8be5                     mov        esp, ebp
00534f09 5d                       pop        ebp
00534f0a c3                       ret        
00534f0b cc                       int3       
00534f0c cc                       int3       
00534f0d cc                       int3       
00534f0e cc                       int3       
00534f0f cc                       int3       
