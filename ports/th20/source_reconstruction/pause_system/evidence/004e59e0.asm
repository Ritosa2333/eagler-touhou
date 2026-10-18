004e59e0 55                       push       ebp
004e59e1 8bec                     mov        ebp, esp
004e59e3 83ec0c                   sub        esp, 0xc
004e59e6 894dfc                   mov        dword ptr [ebp - 4], ecx
004e59e9 b968a55b00               mov        ecx, 0x5ba568
004e59ee e81d74fdff               call       0x4bce10
004e59f3 90                       nop        
004e59f4 a128a85b00               mov        eax, dword ptr [0x5ba828]
004e59f9 83b80801000001           cmp        dword ptr [eax + 0x108], 1
004e5a00 7512                     jne        0x4e5a14
004e5a02 6a04                     push       4
004e5a04 b9404d5c00               mov        ecx, 0x5c4d40
004e5a09 e8a2b5fbff               call       0x4a0fb0
004e5a0e 90                       nop        
004e5a0f e9b4010000               jmp        0x4e5bc8
004e5a14 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004e5a1a 8b91e8000000             mov        edx, dword ptr [ecx + 0xe8]
004e5a20 83ca10                   or         edx, 0x10
004e5a23 a128a85b00               mov        eax, dword ptr [0x5ba828]
004e5a28 8990e8000000             mov        dword ptr [eax + 0xe8], edx
004e5a2e 6a03                     push       3
004e5a30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5a33 e858100000               call       0x4e6a90
004e5a38 6a03                     push       3
004e5a3a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5a3d e85e0f0000               call       0x4e69a0
004e5a42 90                       nop        
004e5a43 b968a55b00               mov        ecx, 0x5ba568
004e5a48 e8032efaff               call       0x488850
004e5a4d 0fb6c8                   movzx      ecx, al
004e5a50 85c9                     test       ecx, ecx
004e5a52 740b                     je         0x4e5a5f
004e5a54 6a05                     push       5
004e5a56 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5a59 e8420f0000               call       0x4e69a0
004e5a5e 90                       nop        
004e5a5f b968a55b00               mov        ecx, 0x5ba568
004e5a64 e8b77dfdff               call       0x4bd820
004e5a69 0fb6d0                   movzx      edx, al
004e5a6c 85d2                     test       edx, edx
004e5a6e 0f85ad000000             jne        0x4e5b21
004e5a74 b968a55b00               mov        ecx, 0x5ba568
004e5a79 e8d22dfaff               call       0x488850
004e5a7e 0fb6c0                   movzx      eax, al
004e5a81 85c0                     test       eax, eax
004e5a83 0f8598000000             jne        0x4e5b21
004e5a89 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004e5a8f e8dcabf8ff               call       0x470670
004e5a94 8945f8                   mov        dword ptr [ebp - 8], eax
004e5a97 6a00                     push       0
004e5a99 6a57                     push       0x57
004e5a9b 681cfe5600               push       0x56fe1c
004e5aa0 8d4df4                   lea        ecx, [ebp - 0xc]
004e5aa3 51                       push       ecx
004e5aa4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e5aa7 e8d485fbff               call       0x49e080
004e5aac 8b10                     mov        edx, dword ptr [eax]
004e5aae 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5ab1 8990cc000000             mov        dword ptr [eax + 0xcc], edx
004e5ab7 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004e5abf f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
004e5ac7 f30f2cc8                 cvttss2si  ecx, xmm0
004e5acb 51                       push       ecx
004e5acc f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
004e5ad4 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
004e5adc f30f2cd0                 cvttss2si  edx, xmm0
004e5ae0 52                       push       edx
004e5ae1 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004e5ae9 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
004e5af1 f30f2cc0                 cvttss2si  eax, xmm0
004e5af5 50                       push       eax
004e5af6 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004e5afe f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
004e5b06 f30f2cc8                 cvttss2si  ecx, xmm0
004e5b0a 51                       push       ecx
004e5b0b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5b0e 8b82cc000000             mov        eax, dword ptr [edx + 0xcc]
004e5b14 50                       push       eax
004e5b15 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004e5b1b e8e00a0000               call       0x4e6600
004e5b20 90                       nop        
004e5b21 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5b27 e82427fdff               call       0x4b8250
004e5b2c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5b2f 8981d4020000             mov        dword ptr [ecx + 0x2d4], eax
004e5b35 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5b38 c782e000000001000000     mov        dword ptr [edx + 0xe0], 1
004e5b42 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5b47 e89437f4ff               call       0x4292e0
004e5b4c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5b4f d998c4010000             fstp       dword ptr [eax + 0x1c4]
004e5b55 51                       push       ecx
004e5b56 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e5b5e f30f110424               movss      dword ptr [esp], xmm0
004e5b63 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5b68 e83337f4ff               call       0x4292a0
004e5b6d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5b70 8b1558885b00             mov        edx, dword ptr [0x5b8858]
004e5b76 8991ec000000             mov        dword ptr [ecx + 0xec], edx
004e5b7c c70558885b0001000000     mov        dword ptr [0x5b8858], 1
004e5b86 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5b8c e8cf25f9ff               call       0x478160
004e5b91 85c0                     test       eax, eax
004e5b93 7413                     je         0x4e5ba8
004e5b95 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5b9b e8d024fdff               call       0x4b8070
004e5ba0 8bc8                     mov        ecx, eax
004e5ba2 e8892ffdff               call       0x4b8b30
004e5ba7 90                       nop        
004e5ba8 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5bae e85d2cfdff               call       0x4b8810
004e5bb3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5bb6 8b88d0020000             mov        ecx, dword ptr [eax + 0x2d0]
004e5bbc 83c904                   or         ecx, 4
004e5bbf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5bc2 898ad0020000             mov        dword ptr [edx + 0x2d0], ecx
004e5bc8 8be5                     mov        esp, ebp
004e5bca 5d                       pop        ebp
004e5bcb c3                       ret        
004e5bcc cc                       int3       
004e5bcd cc                       int3       
004e5bce cc                       int3       
004e5bcf cc                       int3       
