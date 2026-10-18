004aadb0 55                       push       ebp
004aadb1 8bec                     mov        ebp, esp
004aadb3 83ec0c                   sub        esp, 0xc
004aadb6 894dfc                   mov        dword ptr [ebp - 4], ecx
004aadb9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004aadbc e8fffcffff               call       0x4aaac0
004aadc1 85c0                     test       eax, eax
004aadc3 7414                     je         0x4aadd9
004aadc5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004aadc8 e8f3fcffff               call       0x4aaac0
004aadcd 8bc8                     mov        ecx, eax
004aadcf e8ecf4fcff               call       0x47a2c0
004aadd4 8945f8                   mov        dword ptr [ebp - 8], eax
004aadd7 eb07                     jmp        0x4aade0
004aadd9 c745f88c035700           mov        dword ptr [ebp - 8], 0x57038c
004aade0 8b45f8                   mov        eax, dword ptr [ebp - 8]
004aade3 8945f4                   mov        dword ptr [ebp - 0xc], eax
004aade6 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004aade9 8be5                     mov        esp, ebp
004aadeb 5d                       pop        ebp
004aadec c3                       ret        
004aaded cc                       int3       
004aadee cc                       int3       
004aadef cc                       int3       
