005309d0 55                       push       ebp
005309d1 8bec                     mov        ebp, esp
005309d3 83ec08                   sub        esp, 8
005309d6 894df8                   mov        dword ptr [ebp - 8], ecx
005309d9 6a00                     push       0
005309db e8a036f3ff               call       0x464080
005309e0 83c404                   add        esp, 4
005309e3 8945fc                   mov        dword ptr [ebp - 4], eax
005309e6 6a01                     push       1
005309e8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005309eb e8a0d7f8ff               call       0x4be190
005309f0 90                       nop        
005309f1 8be5                     mov        esp, ebp
005309f3 5d                       pop        ebp
005309f4 c3                       ret        
005309f5 cc                       int3       
005309f6 cc                       int3       
005309f7 cc                       int3       
005309f8 cc                       int3       
005309f9 cc                       int3       
005309fa cc                       int3       
005309fb cc                       int3       
005309fc cc                       int3       
005309fd cc                       int3       
005309fe cc                       int3       
005309ff cc                       int3       
