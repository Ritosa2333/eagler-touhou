00488b00 55                       push       ebp
00488b01 8bec                     mov        ebp, esp
00488b03 51                       push       ecx
00488b04 894dfc                   mov        dword ptr [ebp - 4], ecx
00488b07 8b45fc                   mov        eax, dword ptr [ebp - 4]
00488b0a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00488b0d 8988e0a10100             mov        dword ptr [eax + 0x1a1e0], ecx
00488b13 8be5                     mov        esp, ebp
00488b15 5d                       pop        ebp
00488b16 c20400                   ret        4
00488b19 cc                       int3       
00488b1a cc                       int3       
00488b1b cc                       int3       
00488b1c cc                       int3       
00488b1d cc                       int3       
00488b1e cc                       int3       
00488b1f cc                       int3       
