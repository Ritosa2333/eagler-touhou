004c6180 55                       push       ebp
004c6181 8bec                     mov        ebp, esp
004c6183 51                       push       ecx
004c6184 894dfc                   mov        dword ptr [ebp - 4], ecx
004c6187 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c618a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c618d 8988e4a10100             mov        dword ptr [eax + 0x1a1e4], ecx
004c6193 8be5                     mov        esp, ebp
004c6195 5d                       pop        ebp
004c6196 c20400                   ret        4
004c6199 cc                       int3       
004c619a cc                       int3       
004c619b cc                       int3       
004c619c cc                       int3       
004c619d cc                       int3       
004c619e cc                       int3       
004c619f cc                       int3       
