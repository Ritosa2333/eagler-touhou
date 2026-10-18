004862b0 55                       push       ebp
004862b1 8bec                     mov        ebp, esp
004862b3 83ec10                   sub        esp, 0x10
004862b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004862b9 33c0                     xor        eax, eax
004862bb 8945f0                   mov        dword ptr [ebp - 0x10], eax
004862be 8945f4                   mov        dword ptr [ebp - 0xc], eax
004862c1 8d4df0                   lea        ecx, [ebp - 0x10]
004862c4 e8b755ffff               call       0x47b880
004862c9 8945f8                   mov        dword ptr [ebp - 8], eax
004862cc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004862cf 51                       push       ecx
004862d0 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004862d3 e8b8030000               call       0x486690
004862d8 8d4df0                   lea        ecx, [ebp - 0x10]
004862db e82060ffff               call       0x47c300
004862e0 90                       nop        
004862e1 8be5                     mov        esp, ebp
004862e3 5d                       pop        ebp
004862e4 c3                       ret        
004862e5 cc                       int3       
004862e6 cc                       int3       
004862e7 cc                       int3       
004862e8 cc                       int3       
004862e9 cc                       int3       
004862ea cc                       int3       
004862eb cc                       int3       
004862ec cc                       int3       
004862ed cc                       int3       
004862ee cc                       int3       
004862ef cc                       int3       
