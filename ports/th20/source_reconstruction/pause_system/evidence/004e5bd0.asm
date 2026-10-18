004e5bd0 55                       push       ebp
004e5bd1 8bec                     mov        ebp, esp
004e5bd3 51                       push       ecx
004e5bd4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e5bd7 b968a55b00               mov        ecx, 0x5ba568
004e5bdc e82f72fdff               call       0x4bce10
004e5be1 90                       nop        
004e5be2 a128a85b00               mov        eax, dword ptr [0x5ba828]
004e5be7 83b80801000001           cmp        dword ptr [eax + 0x108], 1
004e5bee 7512                     jne        0x4e5c02
004e5bf0 6a04                     push       4
004e5bf2 b9404d5c00               mov        ecx, 0x5c4d40
004e5bf7 e8b4b3fbff               call       0x4a0fb0
004e5bfc 90                       nop        
004e5bfd e95a010000               jmp        0x4e5d5c
004e5c02 6a02                     push       2
004e5c04 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5c07 e8840e0000               call       0x4e6a90
004e5c0c 6a02                     push       2
004e5c0e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5c11 e88a0d0000               call       0x4e69a0
004e5c16 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004e5c1c 8b91e8000000             mov        edx, dword ptr [ecx + 0xe8]
004e5c22 83ca10                   or         edx, 0x10
004e5c25 a128a85b00               mov        eax, dword ptr [0x5ba828]
004e5c2a 8990e8000000             mov        dword ptr [eax + 0xe8], edx
004e5c30 b930a85b00               mov        ecx, 0x5ba830
004e5c35 e80648fdff               call       0x4ba440
004e5c3a 6a00                     push       0
004e5c3c 6a0e                     push       0xe
004e5c3e b930a85b00               mov        ecx, 0x5ba830
004e5c43 e82811f4ff               call       0x426d70
004e5c48 90                       nop        
004e5c49 b968a55b00               mov        ecx, 0x5ba568
004e5c4e e8fd2bfaff               call       0x488850
004e5c53 0fb6c8                   movzx      ecx, al
004e5c56 85c9                     test       ecx, ecx
004e5c58 750b                     jne        0x4e5c65
004e5c5a b930a85b00               mov        ecx, 0x5ba830
004e5c5f e83c040000               call       0x4e60a0
004e5c64 90                       nop        
004e5c65 b930a85b00               mov        ecx, 0x5ba830
004e5c6a e8811bf4ff               call       0x4277f0
004e5c6f 85c0                     test       eax, eax
004e5c71 7402                     je         0x4e5c75
004e5c73 ebf0                     jmp        0x4e5c65
004e5c75 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5c78 e843040000               call       0x4e60c0
004e5c7d 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5c83 e8c825fdff               call       0x4b8250
004e5c88 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5c8b 8982d4020000             mov        dword ptr [edx + 0x2d4], eax
004e5c91 b968a55b00               mov        ecx, 0x5ba568
004e5c96 e8b52bfaff               call       0x488850
004e5c9b 0fb6c0                   movzx      eax, al
004e5c9e 85c0                     test       eax, eax
004e5ca0 7555                     jne        0x4e5cf7
004e5ca2 b930a85b00               mov        ecx, 0x5ba830
004e5ca7 e8e423fdff               call       0x4b8090
004e5cac 50                       push       eax
004e5cad 6800010000               push       0x100
004e5cb2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5cb5 81c1d0010000             add        ecx, 0x1d0
004e5cbb 51                       push       ecx
004e5cbc e84f290600               call       0x548610
004e5cc1 83c40c                   add        esp, 0xc
004e5cc4 b930a85b00               mov        ecx, 0x5ba830
004e5cc9 e8420a0000               call       0x4e6710
004e5cce 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5cd1 dd9ac8010000             fstp       qword ptr [edx + 0x1c8]
004e5cd7 6828265700               push       0x572628
004e5cdc 6a00                     push       0
004e5cde b9404d5c00               mov        ecx, 0x5c4d40
004e5ce3 e8883dffff               call       0x4d9a70
004e5ce8 6a00                     push       0
004e5cea 6a00                     push       0
004e5cec b9404d5c00               mov        ecx, 0x5c4d40
004e5cf1 e85a3effff               call       0x4d9b50
004e5cf6 90                       nop        
004e5cf7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5cfa c780e000000000000000     mov        dword ptr [eax + 0xe0], 0
004e5d04 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5d09 e8d235f4ff               call       0x4292e0
004e5d0e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5d11 d999c4010000             fstp       dword ptr [ecx + 0x1c4]
004e5d17 51                       push       ecx
004e5d18 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e5d20 f30f110424               movss      dword ptr [esp], xmm0
004e5d25 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5d2a e87135f4ff               call       0x4292a0
004e5d2f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5d32 a158885b00               mov        eax, dword ptr [0x5b8858]
004e5d37 8982ec000000             mov        dword ptr [edx + 0xec], eax
004e5d3d c70558885b0001000000     mov        dword ptr [0x5b8858], 1
004e5d47 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5d4a 8b91d0020000             mov        edx, dword ptr [ecx + 0x2d0]
004e5d50 83e2fb                   and        edx, 0xfffffffb
004e5d53 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5d56 8990d0020000             mov        dword ptr [eax + 0x2d0], edx
004e5d5c 8be5                     mov        esp, ebp
004e5d5e 5d                       pop        ebp
004e5d5f c3                       ret        
