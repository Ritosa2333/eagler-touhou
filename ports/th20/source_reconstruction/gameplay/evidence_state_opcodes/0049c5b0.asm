0049c5b0 55                       push       ebp
0049c5b1 8bec                     mov        ebp, esp
0049c5b3 83ec08                   sub        esp, 8
0049c5b6 894dfc                   mov        dword ptr [ebp - 4], ecx
0049c5b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0049c5bc 83c010                   add        eax, 0x10
0049c5bf 8945f8                   mov        dword ptr [ebp - 8], eax
0049c5c2 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0049c5c5 51                       push       ecx
0049c5c6 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0049c5c9 52                       push       edx
0049c5ca 8b4508                   mov        eax, dword ptr [ebp + 8]
0049c5cd 50                       push       eax
0049c5ce 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0049c5d1 e8eab0fdff               call       0x4776c0
0049c5d6 90                       nop        
0049c5d7 8be5                     mov        esp, ebp
0049c5d9 5d                       pop        ebp
0049c5da c20c00                   ret        0xc
0049c5dd cc                       int3       
0049c5de cc                       int3       
0049c5df cc                       int3       
