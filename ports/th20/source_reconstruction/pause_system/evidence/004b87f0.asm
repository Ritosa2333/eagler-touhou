004b87f0 55                       push       ebp
004b87f1 8bec                     mov        ebp, esp
004b87f3 51                       push       ecx
004b87f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004b87f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b87fa 81c118010000             add        ecx, 0x118
004b8800 e82b72f9ff               call       0x44fa30
004b8805 90                       nop        
004b8806 8be5                     mov        esp, ebp
004b8808 5d                       pop        ebp
004b8809 c3                       ret        
004b880a cc                       int3       
004b880b cc                       int3       
004b880c cc                       int3       
004b880d cc                       int3       
004b880e cc                       int3       
004b880f cc                       int3       
