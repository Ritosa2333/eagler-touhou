004e6790 55                       push       ebp
004e6791 8bec                     mov        ebp, esp
004e6793 51                       push       ecx
004e6794 894dfc                   mov        dword ptr [ebp - 4], ecx
004e6797 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e679a 8b805c020000             mov        eax, dword ptr [eax + 0x25c]
004e67a0 d1e8                     shr        eax, 1
004e67a2 83e001                   and        eax, 1
004e67a5 8be5                     mov        esp, ebp
004e67a7 5d                       pop        ebp
004e67a8 c3                       ret        
004e67a9 cc                       int3       
004e67aa cc                       int3       
004e67ab cc                       int3       
004e67ac cc                       int3       
004e67ad cc                       int3       
004e67ae cc                       int3       
004e67af cc                       int3       
