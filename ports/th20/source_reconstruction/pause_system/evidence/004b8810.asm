004b8810 55                       push       ebp
004b8811 8bec                     mov        ebp, esp
004b8813 51                       push       ecx
004b8814 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8817 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b881a 81c118010000             add        ecx, 0x118
004b8820 e8fb78f9ff               call       0x450120
004b8825 90                       nop        
004b8826 8be5                     mov        esp, ebp
004b8828 5d                       pop        ebp
004b8829 c3                       ret        
004b882a cc                       int3       
004b882b cc                       int3       
004b882c cc                       int3       
004b882d cc                       int3       
004b882e cc                       int3       
004b882f cc                       int3       
