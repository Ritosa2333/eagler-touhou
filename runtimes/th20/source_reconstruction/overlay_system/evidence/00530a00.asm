00530a00 55                       push       ebp
00530a01 8bec                     mov        ebp, esp
00530a03 83ec1c                   sub        esp, 0x1c
00530a06 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00530a0b 33c5                     xor        eax, ebp
00530a0d 8945fc                   mov        dword ptr [ebp - 4], eax
00530a10 894dec                   mov        dword ptr [ebp - 0x14], ecx
00530a13 e848380000               call       0x534260
00530a18 50                       push       eax
00530a19 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530a1c 83c114                   add        ecx, 0x14
00530a1f e8fc2aefff               call       0x423520
00530a24 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530a27 c6403401                 mov        byte ptr [eax + 0x34], 1
00530a2b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530a2e 83c138                   add        ecx, 0x38
00530a31 e89af2f1ff               call       0x44fcd0
00530a36 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530a39 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00530a41 f30f11413c               movss      dword ptr [ecx + 0x3c], xmm0
00530a46 51                       push       ecx
00530a47 0f57c0                   xorps      xmm0, xmm0
00530a4a f30f110424               movss      dword ptr [esp], xmm0
00530a4f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00530a52 51                       push       ecx
00530a53 f30f10423c               movss      xmm0, dword ptr [edx + 0x3c]
00530a58 f30f110424               movss      dword ptr [esp], xmm0
00530a5d 51                       push       ecx
00530a5e 0f57c0                   xorps      xmm0, xmm0
00530a61 f30f110424               movss      dword ptr [esp], xmm0
00530a66 8d4df0                   lea        ecx, [ebp - 0x10]
00530a69 e86223efff               call       0x422dd0
00530a6e 90                       nop        
00530a6f 6a00                     push       0
00530a71 e8bafdf2ff               call       0x460830
00530a76 83c404                   add        esp, 4
00530a79 8bc8                     mov        ecx, eax
00530a7b e8900defff               call       0x421810
00530a80 8945e8                   mov        dword ptr [ebp - 0x18], eax
00530a83 6a00                     push       0
00530a85 6aff                     push       -1
00530a87 51                       push       ecx
00530a88 0f57c0                   xorps      xmm0, xmm0
00530a8b f30f110424               movss      dword ptr [esp], xmm0
00530a90 8d45f0                   lea        eax, [ebp - 0x10]
00530a93 50                       push       eax
00530a94 6a19                     push       0x19
00530a96 688cfb5600               push       0x56fb8c
00530a9b 8d4de4                   lea        ecx, [ebp - 0x1c]
00530a9e 51                       push       ecx
00530a9f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00530aa2 e83986f4ff               call       0x4790e0
00530aa7 8b10                     mov        edx, dword ptr [eax]
00530aa9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530aac 895038                   mov        dword ptr [eax + 0x38], edx
00530aaf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530ab2 33cd                     xor        ecx, ebp
00530ab4 e8351f0100               call       0x5429ee
00530ab9 8be5                     mov        esp, ebp
00530abb 5d                       pop        ebp
00530abc c3                       ret        
00530abd cc                       int3       
00530abe cc                       int3       
00530abf cc                       int3       
