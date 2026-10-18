004a0fb0 55                       push       ebp
004a0fb1 8bec                     mov        ebp, esp
004a0fb3 51                       push       ecx
004a0fb4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0fb7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0fba 8b88480b0000             mov        ecx, dword ptr [eax + 0xb48]
004a0fc0 c1e909                   shr        ecx, 9
004a0fc3 83e101                   and        ecx, 1
004a0fc6 750e                     jne        0x4a0fd6
004a0fc8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0fcb 8b4508                   mov        eax, dword ptr [ebp + 8]
004a0fce 89820c0b0000             mov        dword ptr [edx + 0xb0c], eax
004a0fd4 eb0d                     jmp        0x4a0fe3
004a0fd6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0fd9 c7810c0b000002000000     mov        dword ptr [ecx + 0xb0c], 2
004a0fe3 8be5                     mov        esp, ebp
004a0fe5 5d                       pop        ebp
004a0fe6 c20400                   ret        4
004a0fe9 cc                       int3       
004a0fea cc                       int3       
004a0feb cc                       int3       
004a0fec cc                       int3       
004a0fed cc                       int3       
004a0fee cc                       int3       
004a0fef cc                       int3       
