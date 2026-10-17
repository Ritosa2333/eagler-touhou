0052cdf0 55                       push       ebp
0052cdf1 8bec                     mov        ebp, esp
0052cdf3 83ec0c                   sub        esp, 0xc
0052cdf6 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cdf9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cdfc 0564010000               add        eax, 0x164
0052ce01 8945f8                   mov        dword ptr [ebp - 8], eax
0052ce04 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052ce07 51                       push       ecx
0052ce08 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052ce0b e87077eeff               call       0x414580
0052ce10 8b10                     mov        edx, dword ptr [eax]
0052ce12 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052ce15 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052ce18 50                       push       eax
0052ce19 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052ce1f e87c2ef2ff               call       0x44fca0
0052ce24 90                       nop        
0052ce25 8be5                     mov        esp, ebp
0052ce27 5d                       pop        ebp
0052ce28 c20400                   ret        4
0052ce2b cc                       int3       
0052ce2c cc                       int3       
0052ce2d cc                       int3       
0052ce2e cc                       int3       
0052ce2f cc                       int3       
