00419c00 55                       push       ebp
00419c01 8bec                     mov        ebp, esp
00419c03 51                       push       ecx
00419c04 894dfc                   mov        dword ptr [ebp - 4], ecx
00419c07 8b45fc                   mov        eax, dword ptr [ebp - 4]
00419c0a 8b4010                   mov        eax, dword ptr [eax + 0x10]
00419c0d 234508                   and        eax, dword ptr [ebp + 8]
00419c10 8be5                     mov        esp, ebp
00419c12 5d                       pop        ebp
00419c13 c20400                   ret        4
00419c16 cc                       int3       
00419c17 cc                       int3       
00419c18 cc                       int3       
00419c19 cc                       int3       
00419c1a cc                       int3       
00419c1b cc                       int3       
00419c1c cc                       int3       
00419c1d cc                       int3       
00419c1e cc                       int3       
00419c1f cc                       int3       
