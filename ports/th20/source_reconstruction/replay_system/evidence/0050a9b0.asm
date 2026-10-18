0050a9b0 55                       push       ebp
0050a9b1 8bec                     mov        ebp, esp
0050a9b3 83ec0c                   sub        esp, 0xc
0050a9b6 894df8                   mov        dword ptr [ebp - 8], ecx
0050a9b9 0fb645ff                 movzx      eax, byte ptr [ebp - 1]
0050a9bd 50                       push       eax
0050a9be 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050a9c1 e85a07f0ff               call       0x40b120
0050a9c6 90                       nop        
0050a9c7 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050a9ca 894df4                   mov        dword ptr [ebp - 0xc], ecx
0050a9cd 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0050a9d0 52                       push       edx
0050a9d1 e8aa08f0ff               call       0x40b280
0050a9d6 83c404                   add        esp, 4
0050a9d9 50                       push       eax
0050a9da 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050a9dd e8fe000000               call       0x50aae0
0050a9e2 90                       nop        
0050a9e3 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a9e6 8be5                     mov        esp, ebp
0050a9e8 5d                       pop        ebp
0050a9e9 c20800                   ret        8
0050a9ec cc                       int3       
0050a9ed cc                       int3       
0050a9ee cc                       int3       
0050a9ef cc                       int3       
