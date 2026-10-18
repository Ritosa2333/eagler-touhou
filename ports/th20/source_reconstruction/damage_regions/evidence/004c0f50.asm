004c0f50 55                       push       ebp
004c0f51 8bec                     mov        ebp, esp
004c0f53 51                       push       ecx
004c0f54 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0f57 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0f5a 83c134                   add        ecx, 0x34
004c0f5d e83eaef4ff               call       0x40bda0
004c0f62 8be5                     mov        esp, ebp
004c0f64 5d                       pop        ebp
004c0f65 c3                       ret        
004c0f66 cc                       int3       
004c0f67 cc                       int3       
004c0f68 cc                       int3       
004c0f69 cc                       int3       
004c0f6a cc                       int3       
004c0f6b cc                       int3       
004c0f6c cc                       int3       
004c0f6d cc                       int3       
004c0f6e cc                       int3       
004c0f6f cc                       int3       
