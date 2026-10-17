004d7d20 55                       push       ebp
004d7d21 8bec                     mov        ebp, esp
004d7d23 51                       push       ecx
004d7d24 894dfc                   mov        dword ptr [ebp - 4], ecx
004d7d27 8b4508                   mov        eax, dword ptr [ebp + 8]
004d7d2a 83e001                   and        eax, 1
004d7d2d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d7d30 8b9130070000             mov        edx, dword ptr [ecx + 0x730]
004d7d36 83e2fe                   and        edx, 0xfffffffe
004d7d39 0bd0                     or         edx, eax
004d7d3b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d7d3e 899030070000             mov        dword ptr [eax + 0x730], edx
004d7d44 8be5                     mov        esp, ebp
004d7d46 5d                       pop        ebp
004d7d47 c20400                   ret        4
004d7d4a cc                       int3       
004d7d4b cc                       int3       
004d7d4c cc                       int3       
004d7d4d cc                       int3       
004d7d4e cc                       int3       
004d7d4f cc                       int3       
