004bfbb0 55                       push       ebp
004bfbb1 8bec                     mov        ebp, esp
004bfbb3 83ec08                   sub        esp, 8
004bfbb6 894dfc                   mov        dword ptr [ebp - 4], ecx
004bfbb9 8b4508                   mov        eax, dword ptr [ebp + 8]
004bfbbc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfbbf 8d548170                 lea        edx, [ecx + eax*4 + 0x70]
004bfbc3 8955f8                   mov        dword ptr [ebp - 8], edx
004bfbc6 0fbf450c                 movsx      eax, word ptr [ebp + 0xc]
004bfbca 50                       push       eax
004bfbcb 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bfbce e8fdf4f8ff               call       0x44f0d0
004bfbd3 90                       nop        
004bfbd4 8be5                     mov        esp, ebp
004bfbd6 5d                       pop        ebp
004bfbd7 c20800                   ret        8
004bfbda cc                       int3       
004bfbdb cc                       int3       
004bfbdc cc                       int3       
004bfbdd cc                       int3       
004bfbde cc                       int3       
004bfbdf cc                       int3       
