0052df00 55                       push       ebp
0052df01 8bec                     mov        ebp, esp
0052df03 83ec08                   sub        esp, 8
0052df06 894dfc                   mov        dword ptr [ebp - 4], ecx
0052df09 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052df0c 83c014                   add        eax, 0x14
0052df0f 8945f8                   mov        dword ptr [ebp - 8], eax
0052df12 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052df15 51                       push       ecx
0052df16 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052df19 e892100000               call       0x52efb0
0052df1e 90                       nop        
0052df1f 8be5                     mov        esp, ebp
0052df21 5d                       pop        ebp
0052df22 c20400                   ret        4
0052df25 cc                       int3       
0052df26 cc                       int3       
0052df27 cc                       int3       
0052df28 cc                       int3       
0052df29 cc                       int3       
0052df2a cc                       int3       
0052df2b cc                       int3       
0052df2c cc                       int3       
0052df2d cc                       int3       
0052df2e cc                       int3       
0052df2f cc                       int3       
