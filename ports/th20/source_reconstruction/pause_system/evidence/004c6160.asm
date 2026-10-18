004c6160 55                       push       ebp
004c6161 8bec                     mov        ebp, esp
004c6163 51                       push       ecx
004c6164 894dfc                   mov        dword ptr [ebp - 4], ecx
004c6167 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c616a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c616d 8988c8a10100             mov        dword ptr [eax + 0x1a1c8], ecx
004c6173 8be5                     mov        esp, ebp
004c6175 5d                       pop        ebp
004c6176 c20400                   ret        4
004c6179 cc                       int3       
004c617a cc                       int3       
004c617b cc                       int3       
004c617c cc                       int3       
004c617d cc                       int3       
004c617e cc                       int3       
004c617f cc                       int3       
