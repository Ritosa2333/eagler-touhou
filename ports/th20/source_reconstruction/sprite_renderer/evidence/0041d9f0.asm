0041d9f0 55                       push       ebp
0041d9f1 8bec                     mov        ebp, esp
0041d9f3 83ec1c                   sub        esp, 0x1c
0041d9f6 894df0                   mov        dword ptr [ebp - 0x10], ecx
0041d9f9 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0041d9fc 0530070006               add        eax, 0x6000730
0041da01 8945ec                   mov        dword ptr [ebp - 0x14], eax
0041da04 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0041da07 e894e3feff               call       0x40bda0
0041da0c 8945f4                   mov        dword ptr [ebp - 0xc], eax
0041da0f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0041da12 e859e4ffff               call       0x41be70
0041da17 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0041da1a eb09                     jmp        0x41da25
0041da1c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0041da1f 83c104                   add        ecx, 4
0041da22 894df4                   mov        dword ptr [ebp - 0xc], ecx
0041da25 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0041da28 3b55e4                   cmp        edx, dword ptr [ebp - 0x1c]
0041da2b 0f84fd000000             je         0x41db2e
0041da31 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0041da34 8945f8                   mov        dword ptr [ebp - 8], eax
0041da37 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0041da3a 833900                   cmp        dword ptr [ecx], 0
0041da3d 0f84e6000000             je         0x41db29
0041da43 c745fc00000000           mov        dword ptr [ebp - 4], 0
0041da4a eb09                     jmp        0x41da55
0041da4c 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041da4f 83c201                   add        edx, 1
0041da52 8955fc                   mov        dword ptr [ebp - 4], edx
0041da55 8b45f8                   mov        eax, dword ptr [ebp - 8]
0041da58 8b08                     mov        ecx, dword ptr [eax]
0041da5a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0041da5d 3b5144                   cmp        edx, dword ptr [ecx + 0x44]
0041da60 0f8dc3000000             jge        0x41db29
0041da66 8b45f8                   mov        eax, dword ptr [ebp - 8]
0041da69 8b08                     mov        ecx, dword ptr [eax]
0041da6b 6b55fc18                 imul       edx, dword ptr [ebp - 4], 0x18
0041da6f 8b4158                   mov        eax, dword ptr [ecx + 0x58]
0041da72 8b4c1014                 mov        ecx, dword ptr [eax + edx + 0x14]
0041da76 83e103                   and        ecx, 3
0041da79 894de8                   mov        dword ptr [ebp - 0x18], ecx
0041da7c 837de801                 cmp        dword ptr [ebp - 0x18], 1
0041da80 740b                     je         0x41da8d
0041da82 837de802                 cmp        dword ptr [ebp - 0x18], 2
0041da86 7447                     je         0x41dacf
0041da88 e997000000               jmp        0x41db24
0041da8d 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041da90 8b02                     mov        eax, dword ptr [edx]
0041da92 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041da96 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041da99 8b440a10                 mov        eax, dword ptr [edx + ecx + 0x10]
0041da9d 0fb7480c                 movzx      ecx, word ptr [eax + 0xc]
0041daa1 51                       push       ecx
0041daa2 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041daa5 8b02                     mov        eax, dword ptr [edx]
0041daa7 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041daab 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041daae 8b440a10                 mov        eax, dword ptr [edx + ecx + 0x10]
0041dab2 0fb7480a                 movzx      ecx, word ptr [eax + 0xa]
0041dab6 51                       push       ecx
0041dab7 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041daba 8b02                     mov        eax, dword ptr [edx]
0041dabc 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041dac0 034858                   add        ecx, dword ptr [eax + 0x58]
0041dac3 51                       push       ecx
0041dac4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0041dac7 e884e60200               call       0x44c150
0041dacc 90                       nop        
0041dacd eb55                     jmp        0x41db24
0041dacf 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041dad2 8b02                     mov        eax, dword ptr [edx]
0041dad4 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041dad8 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041dadb 8b440a10                 mov        eax, dword ptr [edx + ecx + 0x10]
0041dadf 0fb7480e                 movzx      ecx, word ptr [eax + 0xe]
0041dae3 51                       push       ecx
0041dae4 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041dae7 8b02                     mov        eax, dword ptr [edx]
0041dae9 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041daed 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041daf0 8b440a10                 mov        eax, dword ptr [edx + ecx + 0x10]
0041daf4 0fb7480c                 movzx      ecx, word ptr [eax + 0xc]
0041daf8 51                       push       ecx
0041daf9 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041dafc 8b02                     mov        eax, dword ptr [edx]
0041dafe 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041db02 8b5058                   mov        edx, dword ptr [eax + 0x58]
0041db05 8b440a10                 mov        eax, dword ptr [edx + ecx + 0x10]
0041db09 0fb7480a                 movzx      ecx, word ptr [eax + 0xa]
0041db0d 51                       push       ecx
0041db0e 8b55f8                   mov        edx, dword ptr [ebp - 8]
0041db11 8b02                     mov        eax, dword ptr [edx]
0041db13 6b4dfc18                 imul       ecx, dword ptr [ebp - 4], 0x18
0041db17 034858                   add        ecx, dword ptr [eax + 0x58]
0041db1a 51                       push       ecx
0041db1b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0041db1e e88de50200               call       0x44c0b0
0041db23 90                       nop        
0041db24 e923ffffff               jmp        0x41da4c
0041db29 e9eefeffff               jmp        0x41da1c
0041db2e 8be5                     mov        esp, ebp
0041db30 5d                       pop        ebp
0041db31 c3                       ret        
0041db32 cc                       int3       
0041db33 cc                       int3       
0041db34 cc                       int3       
0041db35 cc                       int3       
0041db36 cc                       int3       
0041db37 cc                       int3       
0041db38 cc                       int3       
0041db39 cc                       int3       
0041db3a cc                       int3       
0041db3b cc                       int3       
0041db3c cc                       int3       
0041db3d cc                       int3       
0041db3e cc                       int3       
0041db3f cc                       int3       
