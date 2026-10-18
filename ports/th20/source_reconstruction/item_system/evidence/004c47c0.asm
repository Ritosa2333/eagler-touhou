004c47c0 55                       push       ebp
004c47c1 8bec                     mov        ebp, esp
004c47c3 83ec20                   sub        esp, 0x20
004c47c6 56                       push       esi
004c47c7 894df8                   mov        dword ptr [ebp - 8], ecx
004c47ca 6a00                     push       0
004c47cc e8aff8f9ff               call       0x464080
004c47d1 83c404                   add        esp, 4
004c47d4 8bc8                     mov        ecx, eax
004c47d6 e8d54bfdff               call       0x4993b0
004c47db 8bf0                     mov        esi, eax
004c47dd 6a00                     push       0
004c47df e89cf8f9ff               call       0x464080
004c47e4 83c404                   add        esp, 4
004c47e7 8bc8                     mov        ecx, eax
004c47e9 e8223affff               call       0x4b8210
004c47ee 3bf0                     cmp        esi, eax
004c47f0 7c6b                     jl         0x4c485d
004c47f2 6a00                     push       0
004c47f4 e887f8f9ff               call       0x464080
004c47f9 83c404                   add        esp, 4
004c47fc 8bc8                     mov        ecx, eax
004c47fe e82d030000               call       0x4c4b30
004c4803 8945fc                   mov        dword ptr [ebp - 4], eax
004c4806 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4809 99                       cdq        
004c480a b90a000000               mov        ecx, 0xa
004c480f f7f9                     idiv       ecx
004c4811 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4814 2bc2                     sub        eax, edx
004c4816 8945fc                   mov        dword ptr [ebp - 4], eax
004c4819 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c481d 7f07                     jg         0x4c4826
004c481f c745fc0a000000           mov        dword ptr [ebp - 4], 0xa
004c4826 6a00                     push       0
004c4828 e8738dffff               call       0x4bd5a0
004c482d 83c404                   add        esp, 4
004c4830 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c4833 6840ff40ff               push       0xff40ff40
004c4838 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c483b 51                       push       ecx
004c483c 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c483f 81c2e40b0000             add        edx, 0xbe4
004c4845 52                       push       edx
004c4846 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c4849 e8c2be0400               call       0x510710
004c484e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4851 50                       push       eax
004c4852 b968a55b00               mov        ecx, 0x5ba568
004c4857 e8f43cfcff               call       0x488550
004c485c 90                       nop        
004c485d 6a00                     push       0
004c485f e81cf8f9ff               call       0x464080
004c4864 83c404                   add        esp, 4
004c4867 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c486a 6a00                     push       0
004c486c e80ff8f9ff               call       0x464080
004c4871 83c404                   add        esp, 4
004c4874 8bc8                     mov        ecx, eax
004c4876 e89539ffff               call       0x4b8210
004c487b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c487e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c4881 51                       push       ecx
004c4882 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c4885 e886cb0100               call       0x4e1410
004c488a 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c488d 837de800                 cmp        dword ptr [ebp - 0x18], 0
004c4891 745b                     je         0x4c48ee
004c4893 6a00                     push       0
004c4895 e896bff9ff               call       0x460830
004c489a 83c404                   add        esp, 4
004c489d 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c48a0 6aff                     push       -1
004c48a2 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c48a5 e8f6630300               call       0x4faca0
004c48aa 6a00                     push       0
004c48ac e8ef8cffff               call       0x4bd5a0
004c48b1 83c404                   add        esp, 4
004c48b4 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c48b7 6840ffffff               push       0xffffff40
004c48bc 6aff                     push       -1
004c48be 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c48c1 81c2e40b0000             add        edx, 0xbe4
004c48c7 52                       push       edx
004c48c8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c48cb e840be0400               call       0x510710
004c48d0 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c48d3 51                       push       ecx
004c48d4 f30f1080e40b0000         movss      xmm0, dword ptr [eax + 0xbe4]
004c48dc f30f110424               movss      dword ptr [esp], xmm0
004c48e1 6a0d                     push       0xd
004c48e3 b930a85b00               mov        ecx, 0x5ba830
004c48e8 e8c325f6ff               call       0x426eb0
004c48ed 90                       nop        
004c48ee 5e                       pop        esi
004c48ef 8be5                     mov        esp, ebp
004c48f1 5d                       pop        ebp
004c48f2 c3                       ret        
004c48f3 cc                       int3       
004c48f4 cc                       int3       
004c48f5 cc                       int3       
004c48f6 cc                       int3       
004c48f7 cc                       int3       
004c48f8 cc                       int3       
004c48f9 cc                       int3       
004c48fa cc                       int3       
004c48fb cc                       int3       
004c48fc cc                       int3       
004c48fd cc                       int3       
004c48fe cc                       int3       
004c48ff cc                       int3       
