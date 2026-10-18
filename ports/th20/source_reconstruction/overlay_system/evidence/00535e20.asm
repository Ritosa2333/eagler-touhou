00535e20 55                       push       ebp
00535e21 8bec                     mov        ebp, esp
00535e23 83ec34                   sub        esp, 0x34
00535e26 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00535e2b 33c5                     xor        eax, ebp
00535e2d 8945fc                   mov        dword ptr [ebp - 4], eax
00535e30 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00535e33 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00535e36 c6403500                 mov        byte ptr [eax + 0x35], 0
00535e3a 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00535e3d 83c138                   add        ecx, 0x38
00535e40 e8eb71f2ff               call       0x45d030
00535e45 85c0                     test       eax, eax
00535e47 0f84fd000000             je         0x535f4a
00535e4d c745e800000000           mov        dword ptr [ebp - 0x18], 0
00535e54 eb09                     jmp        0x535e5f
00535e56 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00535e59 83c101                   add        ecx, 1
00535e5c 894de8                   mov        dword ptr [ebp - 0x18], ecx
00535e5f 6a00                     push       0
00535e61 e81ae2f2ff               call       0x464080
00535e66 83c404                   add        esp, 4
00535e69 8bc8                     mov        ecx, eax
00535e6b e83023f8ff               call       0x4b81a0
00535e70 3945e8                   cmp        dword ptr [ebp - 0x18], eax
00535e73 0f8dd1000000             jge        0x535f4a
00535e79 6a00                     push       0
00535e7b e800e2f2ff               call       0x464080
00535e80 83c404                   add        esp, 4
00535e83 8bc8                     mov        ecx, eax
00535e85 e876e5f2ff               call       0x464400
00535e8a 0fb6d0                   movzx      edx, al
00535e8d 85d2                     test       edx, edx
00535e8f 7409                     je         0x535e9a
00535e91 c745e016000000           mov        dword ptr [ebp - 0x20], 0x16
00535e98 eb07                     jmp        0x535ea1
00535e9a c745e010000000           mov        dword ptr [ebp - 0x20], 0x10
00535ea1 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00535ea4 8945dc                   mov        dword ptr [ebp - 0x24], eax
00535ea7 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00535eaa 83c138                   add        ecx, 0x38
00535ead e8dea0edff               call       0x40ff90
00535eb2 99                       cdq        
00535eb3 f77ddc                   idiv       dword ptr [ebp - 0x24]
00535eb6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00535eb9 c1e102                   shl        ecx, 2
00535ebc 3bd1                     cmp        edx, ecx
00535ebe 0f8581000000             jne        0x535f45
00535ec4 6a00                     push       0
00535ec6 e865a9f2ff               call       0x460830
00535ecb 83c404                   add        esp, 4
00535ece 8945d8                   mov        dword ptr [ebp - 0x28], eax
00535ed1 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00535ed4 52                       push       edx
00535ed5 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00535ed8 e8e3f8ffff               call       0x5357c0
00535edd 8bc8                     mov        ecx, eax
00535edf e88cbaeeff               call       0x421970
00535ee4 8b08                     mov        ecx, dword ptr [eax]
00535ee6 894dec                   mov        dword ptr [ebp - 0x14], ecx
00535ee9 8b5004                   mov        edx, dword ptr [eax + 4]
00535eec 8955f0                   mov        dword ptr [ebp - 0x10], edx
00535eef 8b4008                   mov        eax, dword ptr [eax + 8]
00535ef2 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535ef5 6a04                     push       4
00535ef7 8d4df8                   lea        ecx, [ebp - 8]
00535efa e88161edff               call       0x40c080
00535eff 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00535f02 51                       push       ecx
00535f03 8d4df8                   lea        ecx, [ebp - 8]
00535f06 e895e3edff               call       0x4142a0
00535f0b 90                       nop        
00535f0c 6a00                     push       0
00535f0e e86d2ff4ff               call       0x478e80
00535f13 83c404                   add        esp, 4
00535f16 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00535f19 83ec28                   sub        esp, 0x28
00535f1c 8bcc                     mov        ecx, esp
00535f1e 8965cc                   mov        dword ptr [ebp - 0x34], esp
00535f21 8d55f8                   lea        edx, [ebp - 8]
00535f24 52                       push       edx
00535f25 e886faffff               call       0x5359b0
00535f2a 51                       push       ecx
00535f2b f30f1005d8c85600         movss      xmm0, dword ptr [0x56c8d8]
00535f33 f30f110424               movss      dword ptr [esp], xmm0
00535f38 8d45ec                   lea        eax, [ebp - 0x14]
00535f3b 50                       push       eax
00535f3c 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00535f3f e8fc72f4ff               call       0x47d240
00535f44 90                       nop        
00535f45 e90cffffff               jmp        0x535e56
00535f4a 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00535f4d 83c138                   add        ecx, 0x38
00535f50 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00535f53 6a00                     push       0
00535f55 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00535f58 e8e3d5eeff               call       0x423540
00535f5d 90                       nop        
00535f5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00535f61 33cd                     xor        ecx, ebp
00535f63 e886ca0000               call       0x5429ee
00535f68 8be5                     mov        esp, ebp
00535f6a 5d                       pop        ebp
00535f6b c3                       ret        
00535f6c cc                       int3       
00535f6d cc                       int3       
00535f6e cc                       int3       
00535f6f cc                       int3       
