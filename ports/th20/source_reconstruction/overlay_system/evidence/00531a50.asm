00531a50 55                       push       ebp
00531a51 8bec                     mov        ebp, esp
00531a53 83ec60                   sub        esp, 0x60
00531a56 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00531a5b 33c5                     xor        eax, ebp
00531a5d 8945fc                   mov        dword ptr [ebp - 4], eax
00531a60 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00531a63 e8f8270000               call       0x534260
00531a68 50                       push       eax
00531a69 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531a6c 83c114                   add        ecx, 0x14
00531a6f e8ac1aefff               call       0x423520
00531a74 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00531a77 c6403401                 mov        byte ptr [eax + 0x34], 1
00531a7b 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531a7e 83c138                   add        ecx, 0x38
00531a81 e84ae2f1ff               call       0x44fcd0
00531a86 8d4dc4                   lea        ecx, [ebp - 0x3c]
00531a89 e8a29ff4ff               call       0x47ba30
00531a8e 6a00                     push       0
00531a90 e89bedf2ff               call       0x460830
00531a95 83c404                   add        esp, 4
00531a98 8bc8                     mov        ecx, eax
00531a9a e8b1edf2ff               call       0x460850
00531a9f 8b08                     mov        ecx, dword ptr [eax]
00531aa1 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00531aa4 8b5004                   mov        edx, dword ptr [eax + 4]
00531aa7 8955c8                   mov        dword ptr [ebp - 0x38], edx
00531aaa 8b4008                   mov        eax, dword ptr [eax + 8]
00531aad 8945cc                   mov        dword ptr [ebp - 0x34], eax
00531ab0 6a00                     push       0
00531ab2 e8695af0ff               call       0x437520
00531ab7 83c404                   add        esp, 4
00531aba 8945bc                   mov        dword ptr [ebp - 0x44], eax
00531abd 6a00                     push       0
00531abf 8d4dc4                   lea        ecx, [ebp - 0x3c]
00531ac2 51                       push       ecx
00531ac3 6a09                     push       9
00531ac5 8d55a0                   lea        edx, [ebp - 0x60]
00531ac8 52                       push       edx
00531ac9 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00531acc e89fc0f6ff               call       0x49db70
00531ad1 8b00                     mov        eax, dword ptr [eax]
00531ad3 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531ad6 894138                   mov        dword ptr [ecx + 0x38], eax
00531ad9 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
00531adc f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
00531ae4 f30f114268               movss      dword ptr [edx + 0x68], xmm0
00531ae9 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00531aec 83c038                   add        eax, 0x38
00531aef 8945b8                   mov        dword ptr [ebp - 0x48], eax
00531af2 51                       push       ecx
00531af3 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00531afb f30f110424               movss      dword ptr [esp], xmm0
00531b00 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531b03 51                       push       ecx
00531b04 f30f104168               movss      xmm0, dword ptr [ecx + 0x68]
00531b09 f30f110424               movss      dword ptr [esp], xmm0
00531b0e 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00531b11 e89aebf1ff               call       0x4506b0
00531b16 6a00                     push       0
00531b18 e813edf2ff               call       0x460830
00531b1d 83c404                   add        esp, 4
00531b20 8bc8                     mov        ecx, eax
00531b22 e829edf2ff               call       0x460850
00531b27 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
00531b2a 83c25c                   add        edx, 0x5c
00531b2d 8b08                     mov        ecx, dword ptr [eax]
00531b2f 890a                     mov        dword ptr [edx], ecx
00531b31 8b4804                   mov        ecx, dword ptr [eax + 4]
00531b34 894a04                   mov        dword ptr [edx + 4], ecx
00531b37 8b4008                   mov        eax, dword ptr [eax + 8]
00531b3a 894208                   mov        dword ptr [edx + 8], eax
00531b3d 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531b40 83c16c                   add        ecx, 0x6c
00531b43 894dac                   mov        dword ptr [ebp - 0x54], ecx
00531b46 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
00531b4e f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
00531b53 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
00531b5b f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
00531b60 8d55b4                   lea        edx, [ebp - 0x4c]
00531b63 52                       push       edx
00531b64 8d45b0                   lea        eax, [ebp - 0x50]
00531b67 50                       push       eax
00531b68 6a04                     push       4
00531b6a 6a14                     push       0x14
00531b6c 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00531b6f e8dc79f0ff               call       0x439550
00531b74 90                       nop        
00531b75 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00531b78 83c138                   add        ecx, 0x38
00531b7b 894da8                   mov        dword ptr [ebp - 0x58], ecx
00531b7e 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
00531b81 52                       push       edx
00531b82 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00531b85 e8c6ecf1ff               call       0x450850
00531b8a 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00531b8d 83c038                   add        eax, 0x38
00531b90 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00531b93 68a0155300               push       0x5315a0
00531b98 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00531b9b e860d5f1ff               call       0x44f100
00531ba0 90                       nop        
00531ba1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531ba4 33cd                     xor        ecx, ebp
00531ba6 e8430e0100               call       0x5429ee
00531bab 8be5                     mov        esp, ebp
00531bad 5d                       pop        ebp
00531bae c3                       ret        
00531baf cc                       int3       
