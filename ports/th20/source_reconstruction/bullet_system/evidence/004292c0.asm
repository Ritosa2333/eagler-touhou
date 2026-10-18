004292c0 55                       push       ebp
004292c1 8bec                     mov        ebp, esp
004292c3 51                       push       ecx
004292c4 894dfc                   mov        dword ptr [ebp - 4], ecx
004292c7 6a05                     push       5
004292c9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004292cc e84f31feff               call       0x40c420
004292d1 8be5                     mov        esp, ebp
004292d3 5d                       pop        ebp
004292d4 c3                       ret        
004292d5 cc                       int3       
004292d6 cc                       int3       
004292d7 cc                       int3       
004292d8 cc                       int3       
004292d9 cc                       int3       
004292da cc                       int3       
004292db cc                       int3       
004292dc cc                       int3       
004292dd cc                       int3       
004292de cc                       int3       
004292df cc                       int3       
