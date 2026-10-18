004a0e90 55                       push       ebp
004a0e91 8bec                     mov        ebp, esp
004a0e93 51                       push       ecx
004a0e94 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0e97 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0e9a 8b4020                   mov        eax, dword ptr [eax + 0x20]
004a0e9d 83e001                   and        eax, 1
004a0ea0 8be5                     mov        esp, ebp
004a0ea2 5d                       pop        ebp
004a0ea3 c3                       ret        
004a0ea4 cc                       int3       
004a0ea5 cc                       int3       
004a0ea6 cc                       int3       
004a0ea7 cc                       int3       
004a0ea8 cc                       int3       
004a0ea9 cc                       int3       
004a0eaa cc                       int3       
004a0eab cc                       int3       
004a0eac cc                       int3       
004a0ead cc                       int3       
004a0eae cc                       int3       
004a0eaf cc                       int3       
