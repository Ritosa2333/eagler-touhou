004c4b70 55                       push       ebp
004c4b71 8bec                     mov        ebp, esp
004c4b73 51                       push       ecx
004c4b74 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4b77 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4b7a d94030                   fld        dword ptr [eax + 0x30]
004c4b7d 8be5                     mov        esp, ebp
004c4b7f 5d                       pop        ebp
004c4b80 c3                       ret        
004c4b81 cc                       int3       
004c4b82 cc                       int3       
004c4b83 cc                       int3       
004c4b84 cc                       int3       
004c4b85 cc                       int3       
004c4b86 cc                       int3       
004c4b87 cc                       int3       
004c4b88 cc                       int3       
004c4b89 cc                       int3       
004c4b8a cc                       int3       
004c4b8b cc                       int3       
004c4b8c cc                       int3       
004c4b8d cc                       int3       
004c4b8e cc                       int3       
004c4b8f cc                       int3       
