004887d0 55                       push       ebp
004887d1 8bec                     mov        ebp, esp
004887d3 83ec08                   sub        esp, 8
004887d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004887d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004887dc 83c024                   add        eax, 0x24
004887df 8945f8                   mov        dword ptr [ebp - 8], eax
004887e2 6a3c                     push       0x3c
004887e4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004887e7 e8d4adf9ff               call       0x4235c0
004887ec 0fb6c0                   movzx      eax, al
004887ef 8be5                     mov        esp, ebp
004887f1 5d                       pop        ebp
004887f2 c3                       ret        
004887f3 cc                       int3       
004887f4 cc                       int3       
004887f5 cc                       int3       
004887f6 cc                       int3       
004887f7 cc                       int3       
004887f8 cc                       int3       
004887f9 cc                       int3       
004887fa cc                       int3       
004887fb cc                       int3       
004887fc cc                       int3       
004887fd cc                       int3       
004887fe cc                       int3       
004887ff cc                       int3       
