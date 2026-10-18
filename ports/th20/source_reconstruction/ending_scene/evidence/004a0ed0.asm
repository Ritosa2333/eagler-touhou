004a0ed0 55                       push       ebp
004a0ed1 8bec                     mov        ebp, esp
004a0ed3 51                       push       ecx
004a0ed4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0ed7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0eda 8b407c                   mov        eax, dword ptr [eax + 0x7c]
004a0edd c1e802                   shr        eax, 2
004a0ee0 83e001                   and        eax, 1
004a0ee3 8be5                     mov        esp, ebp
004a0ee5 5d                       pop        ebp
004a0ee6 c3                       ret        
004a0ee7 cc                       int3       
004a0ee8 cc                       int3       
004a0ee9 cc                       int3       
004a0eea cc                       int3       
004a0eeb cc                       int3       
004a0eec cc                       int3       
004a0eed cc                       int3       
004a0eee cc                       int3       
004a0eef cc                       int3       
