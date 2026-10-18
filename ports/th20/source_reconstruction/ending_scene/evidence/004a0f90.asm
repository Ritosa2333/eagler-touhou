004a0f90 55                       push       ebp
004a0f91 8bec                     mov        ebp, esp
004a0f93 51                       push       ecx
004a0f94 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0f97 6a03                     push       3
004a0f99 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0f9c e82fe1faff               call       0x44f0d0
004a0fa1 90                       nop        
004a0fa2 8be5                     mov        esp, ebp
004a0fa4 5d                       pop        ebp
004a0fa5 c3                       ret        
004a0fa6 cc                       int3       
004a0fa7 cc                       int3       
004a0fa8 cc                       int3       
004a0fa9 cc                       int3       
004a0faa cc                       int3       
004a0fab cc                       int3       
004a0fac cc                       int3       
004a0fad cc                       int3       
004a0fae cc                       int3       
004a0faf cc                       int3       
