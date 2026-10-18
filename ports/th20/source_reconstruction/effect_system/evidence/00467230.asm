00467230 55                       push       ebp
00467231 8bec                     mov        ebp, esp
00467233 83ec10                   sub        esp, 0x10
00467236 8d4510                   lea        eax, [ebp + 0x10]
00467239 50                       push       eax
0046723a 8d4d0c                   lea        ecx, [ebp + 0xc]
0046723d 51                       push       ecx
0046723e e86d54faff               call       0x40c6b0
00467243 83c408                   add        esp, 8
00467246 837d1400                 cmp        dword ptr [ebp + 0x14], 0
0046724a 750d                     jne        0x467259
0046724c 8b5508                   mov        edx, dword ptr [ebp + 8]
0046724f 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00467252 8902                     mov        dword ptr [edx], eax
00467254 8b4508                   mov        eax, dword ptr [ebp + 8]
00467257 eb7f                     jmp        0x4672d8
00467259 8d4d0c                   lea        ecx, [ebp + 0xc]
0046725c 51                       push       ecx
0046725d e80efeffff               call       0x467070
00467262 83c404                   add        esp, 4
00467265 8945f4                   mov        dword ptr [ebp - 0xc], eax
00467268 8d5510                   lea        edx, [ebp + 0x10]
0046726b 52                       push       edx
0046726c e8fffdffff               call       0x467070
00467271 83c404                   add        esp, 4
00467274 8945fc                   mov        dword ptr [ebp - 4], eax
00467277 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046727a 8945f8                   mov        dword ptr [ebp - 8], eax
0046727d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467280 2b4df4                   sub        ecx, dword ptr [ebp - 0xc]
00467283 c1f903                   sar        ecx, 3
00467286 394d14                   cmp        dword ptr [ebp + 0x14], ecx
00467289 7c0d                     jl         0x467298
0046728b 8b5508                   mov        edx, dword ptr [ebp + 8]
0046728e 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00467291 8902                     mov        dword ptr [edx], eax
00467293 8b4508                   mov        eax, dword ptr [ebp + 8]
00467296 eb40                     jmp        0x4672d8
00467298 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0046729b c1e103                   shl        ecx, 3
0046729e 8b55f8                   mov        edx, dword ptr [ebp - 8]
004672a1 2bd1                     sub        edx, ecx
004672a3 8955f8                   mov        dword ptr [ebp - 8], edx
004672a6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004672a9 50                       push       eax
004672aa 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004672ad 51                       push       ecx
004672ae 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004672b1 52                       push       edx
004672b2 e8c9fdffff               call       0x467080
004672b7 83c40c                   add        esp, 0xc
004672ba 8945f0                   mov        dword ptr [ebp - 0x10], eax
004672bd 8d45f0                   lea        eax, [ebp - 0x10]
004672c0 50                       push       eax
004672c1 8d4d0c                   lea        ecx, [ebp + 0xc]
004672c4 51                       push       ecx
004672c5 e816feffff               call       0x4670e0
004672ca 83c408                   add        esp, 8
004672cd 8b5508                   mov        edx, dword ptr [ebp + 8]
004672d0 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004672d3 8902                     mov        dword ptr [edx], eax
004672d5 8b4508                   mov        eax, dword ptr [ebp + 8]
004672d8 8be5                     mov        esp, ebp
004672da 5d                       pop        ebp
004672db c3                       ret        
004672dc cc                       int3       
004672dd cc                       int3       
004672de cc                       int3       
004672df cc                       int3       
