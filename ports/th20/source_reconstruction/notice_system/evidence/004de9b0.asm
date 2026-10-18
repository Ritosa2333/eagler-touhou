004de9b0 55                       push       ebp
004de9b1 8bec                     mov        ebp, esp
004de9b3 83ec0c                   sub        esp, 0xc
004de9b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004de9b9 8b4508                   mov        eax, dword ptr [ebp + 8]
004de9bc 50                       push       eax
004de9bd e84e4ff3ff               call       0x413910
004de9c2 83c404                   add        esp, 4
004de9c5 0fb6c8                   movzx      ecx, al
004de9c8 85c9                     test       ecx, ecx
004de9ca 7502                     jne        0x4de9ce
004de9cc eb36                     jmp        0x4dea04
004de9ce 8b55fc                   mov        edx, dword ptr [ebp - 4]
004de9d1 52                       push       edx
004de9d2 6a08                     push       8
004de9d4 e827fdf2ff               call       0x40e700
004de9d9 83c408                   add        esp, 8
004de9dc 8945f4                   mov        dword ptr [ebp - 0xc], eax
004de9df 8b4508                   mov        eax, dword ptr [ebp + 8]
004de9e2 50                       push       eax
004de9e3 e898c8f2ff               call       0x40b280
004de9e8 83c404                   add        esp, 4
004de9eb 8945f8                   mov        dword ptr [ebp - 8], eax
004de9ee 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004de9f1 51                       push       ecx
004de9f2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004de9f5 e8d6fbffff               call       0x4de5d0
004de9fa 50                       push       eax
004de9fb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de9fe e85d62f3ff               call       0x414c60
004dea03 90                       nop        
004dea04 8be5                     mov        esp, ebp
004dea06 5d                       pop        ebp
004dea07 c20400                   ret        4
004dea0a cc                       int3       
004dea0b cc                       int3       
004dea0c cc                       int3       
004dea0d cc                       int3       
004dea0e cc                       int3       
004dea0f cc                       int3       
