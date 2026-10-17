0049e8c0 55                       push       ebp
0049e8c1 8bec                     mov        ebp, esp
0049e8c3 83ec0c                   sub        esp, 0xc
0049e8c6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049e8cb 33c5                     xor        eax, ebp
0049e8cd 8945fc                   mov        dword ptr [ebp - 4], eax
0049e8d0 894df4                   mov        dword ptr [ebp - 0xc], ecx
0049e8d3 837d0800                 cmp        dword ptr [ebp + 8], 0
0049e8d7 7502                     jne        0x49e8db
0049e8d9 eb36                     jmp        0x49e911
0049e8db 8b4508                   mov        eax, dword ptr [ebp + 8]
0049e8de 50                       push       eax
0049e8df e85c010000               call       0x49ea40
0049e8e4 83c404                   add        esp, 4
0049e8e7 6a01                     push       1
0049e8e9 b940025c00               mov        ecx, 0x5c0240
0049e8ee e81ddaf6ff               call       0x40c310
0049e8f3 50                       push       eax
0049e8f4 8d4df8                   lea        ecx, [ebp - 8]
0049e8f7 e834cef6ff               call       0x40b730
0049e8fc 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0049e8ff 51                       push       ecx
0049e900 e8f7400a00               call       0x5429fc
0049e905 83c404                   add        esp, 4
0049e908 8d4df8                   lea        ecx, [ebp - 8]
0049e90b e8f0cff6ff               call       0x40b900
0049e910 90                       nop        
0049e911 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0049e914 33cd                     xor        ecx, ebp
0049e916 e8d3400a00               call       0x5429ee
0049e91b 8be5                     mov        esp, ebp
0049e91d 5d                       pop        ebp
0049e91e c20400                   ret        4
0049e921 cc                       int3       
0049e922 cc                       int3       
0049e923 cc                       int3       
0049e924 cc                       int3       
0049e925 cc                       int3       
0049e926 cc                       int3       
0049e927 cc                       int3       
0049e928 cc                       int3       
0049e929 cc                       int3       
0049e92a cc                       int3       
0049e92b cc                       int3       
0049e92c cc                       int3       
0049e92d cc                       int3       
0049e92e cc                       int3       
0049e92f cc                       int3       
