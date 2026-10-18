004e5d60 55                       push       ebp
004e5d61 8bec                     mov        ebp, esp
004e5d63 83ec14                   sub        esp, 0x14
004e5d66 894dfc                   mov        dword ptr [ebp - 4], ecx
004e5d69 b968a55b00               mov        ecx, 0x5ba568
004e5d6e e89d70fdff               call       0x4bce10
004e5d73 6a01                     push       1
004e5d75 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5d78 e8130d0000               call       0x4e6a90
004e5d7d a128a85b00               mov        eax, dword ptr [0x5ba828]
004e5d82 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004e5d88 83c910                   or         ecx, 0x10
004e5d8b 8b1528a85b00             mov        edx, dword ptr [0x5ba828]
004e5d91 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004e5d97 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5d9d e8ae24fdff               call       0x4b8250
004e5da2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5da5 8981d4020000             mov        dword ptr [ecx + 0x2d4], eax
004e5dab 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5dae 81c1c8000000             add        ecx, 0xc8
004e5db4 e8179ff6ff               call       0x44fcd0
004e5db9 90                       nop        
004e5dba 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004e5dc0 e86b2afaff               call       0x488830
004e5dc5 85c0                     test       eax, eax
004e5dc7 7431                     je         0x4e5dfa
004e5dc9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5dcc 8b82d4020000             mov        eax, dword ptr [edx + 0x2d4]
004e5dd2 8945f8                   mov        dword ptr [ebp - 8], eax
004e5dd5 6a00                     push       0
004e5dd7 6891000000               push       0x91
004e5ddc 6838065700               push       0x570638
004e5de1 8d4df0                   lea        ecx, [ebp - 0x10]
004e5de4 51                       push       ecx
004e5de5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e5de8 e89382fbff               call       0x49e080
004e5ded 8b10                     mov        edx, dword ptr [eax]
004e5def 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5df2 8990c8000000             mov        dword ptr [eax + 0xc8], edx
004e5df8 eb2f                     jmp        0x4e5e29
004e5dfa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5dfd 8b91d4020000             mov        edx, dword ptr [ecx + 0x2d4]
004e5e03 8955f4                   mov        dword ptr [ebp - 0xc], edx
004e5e06 6a00                     push       0
004e5e08 6890000000               push       0x90
004e5e0d 6838065700               push       0x570638
004e5e12 8d45ec                   lea        eax, [ebp - 0x14]
004e5e15 50                       push       eax
004e5e16 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e5e19 e86282fbff               call       0x49e080
004e5e1e 8b08                     mov        ecx, dword ptr [eax]
004e5e20 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5e23 898ac8000000             mov        dword ptr [edx + 0xc8], ecx
004e5e29 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5e2c 81c1c8000000             add        ecx, 0xc8
004e5e32 e81905faff               call       0x486350
004e5e37 b930a85b00               mov        ecx, 0x5ba830
004e5e3c e8ff45fdff               call       0x4ba440
004e5e41 6a00                     push       0
004e5e43 6a0e                     push       0xe
004e5e45 b930a85b00               mov        ecx, 0x5ba830
004e5e4a e8210ff4ff               call       0x426d70
004e5e4f 90                       nop        
004e5e50 b968a55b00               mov        ecx, 0x5ba568
004e5e55 e8f629faff               call       0x488850
004e5e5a 0fb6c0                   movzx      eax, al
004e5e5d 85c0                     test       eax, eax
004e5e5f 750b                     jne        0x4e5e6c
004e5e61 b930a85b00               mov        ecx, 0x5ba830
004e5e66 e835020000               call       0x4e60a0
004e5e6b 90                       nop        
004e5e6c b930a85b00               mov        ecx, 0x5ba830
004e5e71 e87a19f4ff               call       0x4277f0
004e5e76 85c0                     test       eax, eax
004e5e78 7402                     je         0x4e5e7c
004e5e7a ebf0                     jmp        0x4e5e6c
004e5e7c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5e7f e83c020000               call       0x4e60c0
004e5e84 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5e89 e85234f4ff               call       0x4292e0
004e5e8e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5e91 d999c4010000             fstp       dword ptr [ecx + 0x1c4]
004e5e97 51                       push       ecx
004e5e98 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e5ea0 f30f110424               movss      dword ptr [esp], xmm0
004e5ea5 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5eaa e8f133f4ff               call       0x4292a0
004e5eaf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5eb2 a158885b00               mov        eax, dword ptr [0x5b8858]
004e5eb7 8982ec000000             mov        dword ptr [edx + 0xec], eax
004e5ebd c70558885b0000000000     mov        dword ptr [0x5b8858], 0
004e5ec7 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5ecd e88e22f9ff               call       0x478160
004e5ed2 85c0                     test       eax, eax
004e5ed4 7413                     je         0x4e5ee9
004e5ed6 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5edc e88f21fdff               call       0x4b8070
004e5ee1 8bc8                     mov        ecx, eax
004e5ee3 e8482cfdff               call       0x4b8b30
004e5ee8 90                       nop        
004e5ee9 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5eef e81c29fdff               call       0x4b8810
004e5ef4 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5efa e8310c0000               call       0x4e6b30
004e5eff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5f02 8b91d0020000             mov        edx, dword ptr [ecx + 0x2d0]
004e5f08 83e2fb                   and        edx, 0xfffffffb
004e5f0b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5f0e 8990d0020000             mov        dword ptr [eax + 0x2d0], edx
004e5f14 8be5                     mov        esp, ebp
004e5f16 5d                       pop        ebp
004e5f17 c3                       ret        
004e5f18 cc                       int3       
004e5f19 cc                       int3       
004e5f1a cc                       int3       
004e5f1b cc                       int3       
004e5f1c cc                       int3       
004e5f1d cc                       int3       
004e5f1e cc                       int3       
004e5f1f cc                       int3       
004e5f20 cc                       int3       
004e5f21 cc                       int3       
004e5f22 cc                       int3       
004e5f23 cc                       int3       
004e5f24 cc                       int3       
004e5f25 cc                       int3       
004e5f26 cc                       int3       
004e5f27 cc                       int3       
004e5f28 cc                       int3       
004e5f29 cc                       int3       
004e5f2a cc                       int3       
004e5f2b cc                       int3       
004e5f2c cc                       int3       
004e5f2d cc                       int3       
004e5f2e cc                       int3       
004e5f2f cc                       int3       
