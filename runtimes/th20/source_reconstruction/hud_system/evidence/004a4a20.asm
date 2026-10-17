004a4a20 55                       push       ebp
004a4a21 8bec                     mov        ebp, esp
004a4a23 6aff                     push       -1
004a4a25 68cd835600               push       0x5683cd
004a4a2a 64a100000000             mov        eax, dword ptr fs:[0]
004a4a30 50                       push       eax
004a4a31 83ec34                   sub        esp, 0x34
004a4a34 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004a4a39 33c5                     xor        eax, ebp
004a4a3b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004a4a3e 50                       push       eax
004a4a3f 8d45f4                   lea        eax, [ebp - 0xc]
004a4a42 64a300000000             mov        dword ptr fs:[0], eax
004a4a48 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004a4a4b 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004a4a4e 0508010000               add        eax, 0x108
004a4a53 8945dc                   mov        dword ptr [ebp - 0x24], eax
004a4a56 6a08                     push       8
004a4a58 8d4de8                   lea        ecx, [ebp - 0x18]
004a4a5b e82076f6ff               call       0x40c080
004a4a60 8d4de8                   lea        ecx, [ebp - 0x18]
004a4a63 51                       push       ecx
004a4a64 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a4a67 e814d8f6ff               call       0x412280
004a4a6c c745fc00000000           mov        dword ptr [ebp - 4], 0
004a4a73 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a4a76 e8c5daf6ff               call       0x412540
004a4a7b 8945d0                   mov        dword ptr [ebp - 0x30], eax
004a4a7e eb09                     jmp        0x4a4a89
004a4a80 8d4de8                   lea        ecx, [ebp - 0x18]
004a4a83 e8a8d1f6ff               call       0x411c30
004a4a88 90                       nop        
004a4a89 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004a4a8c 52                       push       edx
004a4a8d 8d4de8                   lea        ecx, [ebp - 0x18]
004a4a90 e81bd1f6ff               call       0x411bb0
004a4a95 0fb6c0                   movzx      eax, al
004a4a98 85c0                     test       eax, eax
004a4a9a 0f8435010000             je         0x4a4bd5
004a4aa0 8d4de8                   lea        ecx, [ebp - 0x18]
004a4aa3 e85878f6ff               call       0x40c300
004a4aa8 8945cc                   mov        dword ptr [ebp - 0x34], eax
004a4aab 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004a4aae e84d78f6ff               call       0x40c300
004a4ab3 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004a4ab6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4ab9 8b9150030000             mov        edx, dword ptr [ecx + 0x350]
004a4abf c1ea05                   shr        edx, 5
004a4ac2 83e201                   and        edx, 1
004a4ac5 7544                     jne        0x4a4b0b
004a4ac7 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a4aca 8b8850030000             mov        ecx, dword ptr [eax + 0x350]
004a4ad0 c1e907                   shr        ecx, 7
004a4ad3 83e101                   and        ecx, 1
004a4ad6 7533                     jne        0x4a4b0b
004a4ad8 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a4adb 8b8254030000             mov        eax, dword ptr [edx + 0x354]
004a4ae1 c1e806                   shr        eax, 6
004a4ae4 83e001                   and        eax, 1
004a4ae7 7522                     jne        0x4a4b0b
004a4ae9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4aec 8b9154030000             mov        edx, dword ptr [ecx + 0x354]
004a4af2 c1ea07                   shr        edx, 7
004a4af5 83e201                   and        edx, 1
004a4af8 7511                     jne        0x4a4b0b
004a4afa 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a4afd 8b8850030000             mov        ecx, dword ptr [eax + 0x350]
004a4b03 c1e90a                   shr        ecx, 0xa
004a4b06 83e101                   and        ecx, 1
004a4b09 7415                     je         0x4a4b20
004a4b0b 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a4b0e 8b8250030000             mov        eax, dword ptr [edx + 0x350]
004a4b14 c1e808                   shr        eax, 8
004a4b17 83e001                   and        eax, 1
004a4b1a 0f84b0000000             je         0x4a4bd0
004a4b20 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4b23 81c130020000             add        ecx, 0x230
004a4b29 e832280000               call       0x4a7360
004a4b2e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4b31 0f57c0                   xorps      xmm0, xmm0
004a4b34 f30f1181f4000000         movss      dword ptr [ecx + 0xf4], xmm0
004a4b3c 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a4b3f f30f100550e05600         movss      xmm0, dword ptr [0x56e050]
004a4b47 f30f1182f8000000         movss      dword ptr [edx + 0xf8], xmm0
004a4b4f 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a4b52 c7800c03000000000000     mov        dword ptr [eax + 0x30c], 0
004a4b5c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4b5f e8dc0a0000               call       0x4a5640
004a4b64 90                       nop        
004a4b65 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4b68 81c194000000             add        ecx, 0x94
004a4b6e 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004a4b71 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004a4b74 e8b703f7ff               call       0x414f30
004a4b79 8945e0                   mov        dword ptr [ebp - 0x20], eax
004a4b7c 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004a4b7f e8ec03f7ff               call       0x414f70
004a4b84 8945c8                   mov        dword ptr [ebp - 0x38], eax
004a4b87 eb09                     jmp        0x4a4b92
004a4b89 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004a4b8c 83c214                   add        edx, 0x14
004a4b8f 8955e0                   mov        dword ptr [ebp - 0x20], edx
004a4b92 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004a4b95 3b45c8                   cmp        eax, dword ptr [ebp - 0x38]
004a4b98 7411                     je         0x4a4bab
004a4b9a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004a4b9d 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004a4ba0 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004a4ba3 e828b1faff               call       0x44fcd0
004a4ba8 90                       nop        
004a4ba9 ebde                     jmp        0x4a4b89
004a4bab 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4bae 81c194000000             add        ecx, 0x94
004a4bb4 e88733ffff               call       0x497f40
004a4bb9 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a4bbc 8b8254030000             mov        eax, dword ptr [edx + 0x354]
004a4bc2 0d00020000               or         eax, 0x200
004a4bc7 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a4bca 898154030000             mov        dword ptr [ecx + 0x354], eax
004a4bd0 e9abfeffff               jmp        0x4a4a80
004a4bd5 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004a4bdc 8d4de8                   lea        ecx, [ebp - 0x18]
004a4bdf e81ccff6ff               call       0x411b00
004a4be4 90                       nop        
004a4be5 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004a4be8 81c29c000000             add        edx, 0x9c
004a4bee 8955c0                   mov        dword ptr [ebp - 0x40], edx
004a4bf1 6a00                     push       0
004a4bf3 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004a4bf6 e845e9f7ff               call       0x423540
004a4bfb 90                       nop        
004a4bfc 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a4bff 64890d00000000           mov        dword ptr fs:[0], ecx
004a4c06 59                       pop        ecx
004a4c07 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a4c0a 33cd                     xor        ecx, ebp
004a4c0c e8dddd0900               call       0x5429ee
004a4c11 8be5                     mov        esp, ebp
004a4c13 5d                       pop        ebp
004a4c14 c3                       ret        
004a4c15 cc                       int3       
004a4c16 cc                       int3       
004a4c17 cc                       int3       
004a4c18 cc                       int3       
004a4c19 cc                       int3       
004a4c1a cc                       int3       
004a4c1b cc                       int3       
004a4c1c cc                       int3       
004a4c1d cc                       int3       
004a4c1e cc                       int3       
004a4c1f cc                       int3       
