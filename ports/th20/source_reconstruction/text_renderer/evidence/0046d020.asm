0046d020 55                       push       ebp
0046d021 8bec                     mov        ebp, esp
0046d023 83ec08                   sub        esp, 8
0046d026 56                       push       esi
0046d027 894df8                   mov        dword ptr [ebp - 8], ecx
0046d02a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046d02d e8ee350000               call       0x470620
0046d032 8945fc                   mov        dword ptr [ebp - 4], eax
0046d035 6a00                     push       0
0046d037 6a0a                     push       0xa
0046d039 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0046d03c 50                       push       eax
0046d03d 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0046d040 51                       push       ecx
0046d041 e8ba600d00               call       0x543100
0046d046 8bc8                     mov        ecx, eax
0046d048 8bf2                     mov        esi, edx
0046d04a 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0046d04d 99                       cdq        
0046d04e 03c8                     add        ecx, eax
0046d050 13f2                     adc        esi, edx
0046d052 56                       push       esi
0046d053 51                       push       ecx
0046d054 6800010000               push       0x100
0046d059 8b55fc                   mov        edx, dword ptr [ebp - 4]
0046d05c 52                       push       edx
0046d05d e89e64feff               call       0x453500
0046d062 83c410                   add        esp, 0x10
0046d065 83ec08                   sub        esp, 8
0046d068 8bcc                     mov        ecx, esp
0046d06a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046d06d 50                       push       eax
0046d06e e85d0dfaff               call       0x40ddd0
0046d073 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0046d076 51                       push       ecx
0046d077 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046d07a e811feffff               call       0x46ce90
0046d07f 90                       nop        
0046d080 5e                       pop        esi
0046d081 8be5                     mov        esp, ebp
0046d083 5d                       pop        ebp
0046d084 c21000                   ret        0x10
0046d087 cc                       int3       
0046d088 cc                       int3       
0046d089 cc                       int3       
0046d08a cc                       int3       
0046d08b cc                       int3       
0046d08c cc                       int3       
0046d08d cc                       int3       
0046d08e cc                       int3       
0046d08f cc                       int3       
