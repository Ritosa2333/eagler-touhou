00488a00 55                       push       ebp
00488a01 8bec                     mov        ebp, esp
00488a03 51                       push       ecx
00488a04 894dfc                   mov        dword ptr [ebp - 4], ecx
00488a07 8b45fc                   mov        eax, dword ptr [ebp - 4]
00488a0a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00488a0d 8988eca10100             mov        dword ptr [eax + 0x1a1ec], ecx
00488a13 8be5                     mov        esp, ebp
00488a15 5d                       pop        ebp
00488a16 c20400                   ret        4
00488a19 cc                       int3       
00488a1a cc                       int3       
00488a1b cc                       int3       
00488a1c cc                       int3       
00488a1d cc                       int3       
00488a1e cc                       int3       
00488a1f cc                       int3       
