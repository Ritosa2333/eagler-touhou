00534d00 55                       push       ebp
00534d01 8bec                     mov        ebp, esp
00534d03 83ec14                   sub        esp, 0x14
00534d06 894dfc                   mov        dword ptr [ebp - 4], ecx
00534d09 6a00                     push       0
00534d0b e820bbf2ff               call       0x460830
00534d10 83c404                   add        esp, 4
00534d13 8bc8                     mov        ecx, eax
00534d15 e836bbf2ff               call       0x460850
00534d1a 51                       push       ecx
00534d1b f30f1000                 movss      xmm0, dword ptr [eax]
00534d1f f30f110424               movss      dword ptr [esp], xmm0
00534d24 6a1c                     push       0x1c
00534d26 b930a85b00               mov        ecx, 0x5ba830
00534d2b e88021efff               call       0x426eb0
00534d30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534d33 e898fbffff               call       0x5348d0
00534d38 90                       nop        
00534d39 8b45fc                   mov        eax, dword ptr [ebp - 4]
00534d3c 83c020                   add        eax, 0x20
00534d3f 8945f8                   mov        dword ptr [ebp - 8], eax
00534d42 6a00                     push       0
00534d44 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00534d47 e80479f4ff               call       0x47c650
00534d4c 0fb6c8                   movzx      ecx, al
00534d4f 85c9                     test       ecx, ecx
00534d51 740c                     je         0x534d5f
00534d53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534d56 83c120                   add        ecx, 0x20
00534d59 e8e242f4ff               call       0x479040
00534d5e 90                       nop        
00534d5f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00534d62 8b4224                   mov        eax, dword ptr [edx + 0x24]
00534d65 8945f0                   mov        dword ptr [ebp - 0x10], eax
00534d68 6a00                     push       0
00534d6a e8c1baf2ff               call       0x460830
00534d6f 83c404                   add        esp, 4
00534d72 8bc8                     mov        ecx, eax
00534d74 e8d7baf2ff               call       0x460850
00534d79 8945f4                   mov        dword ptr [ebp - 0xc], eax
00534d7c 6a00                     push       0
00534d7e 6aff                     push       -1
00534d80 51                       push       ecx
00534d81 0f57c0                   xorps      xmm0, xmm0
00534d84 f30f110424               movss      dword ptr [esp], xmm0
00534d89 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00534d8c 51                       push       ecx
00534d8d 6a1d                     push       0x1d
00534d8f 68c4625700               push       0x5762c4
00534d94 8d55ec                   lea        edx, [ebp - 0x14]
00534d97 52                       push       edx
00534d98 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00534d9b e84043f4ff               call       0x4790e0
00534da0 8b00                     mov        eax, dword ptr [eax]
00534da2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534da5 894120                   mov        dword ptr [ecx + 0x20], eax
00534da8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00534dab 8b4228                   mov        eax, dword ptr [edx + 0x28]
00534dae 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534db1 8b10                     mov        edx, dword ptr [eax]
00534db3 8b4928                   mov        ecx, dword ptr [ecx + 0x28]
00534db6 8b4250                   mov        eax, dword ptr [edx + 0x50]
00534db9 ffd0                     call       eax
00534dbb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534dbe c7415401000000           mov        dword ptr [ecx + 0x54], 1
00534dc5 8be5                     mov        esp, ebp
00534dc7 5d                       pop        ebp
00534dc8 c3                       ret        
00534dc9 cc                       int3       
00534dca cc                       int3       
00534dcb cc                       int3       
00534dcc cc                       int3       
00534dcd cc                       int3       
00534dce cc                       int3       
00534dcf cc                       int3       
