004c9830 55                       push       ebp
004c9831 8bec                     mov        ebp, esp
004c9833 83ec5c                   sub        esp, 0x5c
004c9836 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c983b 33c5                     xor        eax, ebp
004c983d 8945fc                   mov        dword ptr [ebp - 4], eax
004c9840 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c9843 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004c9847 7413                     je         0x4c985c
004c9849 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c984c 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004c9853 7407                     je         0x4c985c
004c9855 33c0                     xor        eax, eax
004c9857 e93f020000               jmp        0x4c9a9b
004c985c c745d800000000           mov        dword ptr [ebp - 0x28], 0
004c9863 8d4df0                   lea        ecx, [ebp - 0x10]
004c9866 e8a595f5ff               call       0x422e10
004c986b 8d4de4                   lea        ecx, [ebp - 0x1c]
004c986e e89d95f5ff               call       0x422e10
004c9873 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c987b f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c9883 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004c9888 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c9890 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c9898 51                       push       ecx
004c9899 f30f110424               movss      dword ptr [esp], xmm0
004c989e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c98a1 51                       push       ecx
004c98a2 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004c98a7 f30f110424               movss      dword ptr [esp], xmm0
004c98ac 8d55e4                   lea        edx, [ebp - 0x1c]
004c98af 52                       push       edx
004c98b0 e87bfaf6ff               call       0x439330
004c98b5 83c40c                   add        esp, 0xc
004c98b8 0f57c0                   xorps      xmm0, xmm0
004c98bb f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004c98c0 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c98c3 83c058                   add        eax, 0x58
004c98c6 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c98c9 8d4de4                   lea        ecx, [ebp - 0x1c]
004c98cc 51                       push       ecx
004c98cd 8d55a4                   lea        edx, [ebp - 0x5c]
004c98d0 52                       push       edx
004c98d1 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c98d4 e897fcf5ff               call       0x429570
004c98d9 8b08                     mov        ecx, dword ptr [eax]
004c98db 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c98de 8b5004                   mov        edx, dword ptr [eax + 4]
004c98e1 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c98e4 8b4008                   mov        eax, dword ptr [eax + 8]
004c98e7 8945f8                   mov        dword ptr [ebp - 8], eax
004c98ea 8d4de4                   lea        ecx, [ebp - 0x1c]
004c98ed 51                       push       ecx
004c98ee 8d4de4                   lea        ecx, [ebp - 0x1c]
004c98f1 e8eafdf5ff               call       0x4296e0
004c98f6 90                       nop        
004c98f7 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c98ff f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c9907 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
004c990c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c990f f30f104a74               movss      xmm1, dword ptr [edx + 0x74]
004c9914 0f2fc8                   comiss     xmm1, xmm0
004c9917 0f8671010000             jbe        0x4c9a8e
004c991d 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c9920 83c001                   add        eax, 1
004c9923 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c9926 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c9929 83b9e406000011           cmp        dword ptr [ecx + 0x6e4], 0x11
004c9930 7e18                     jle        0x4c994a
004c9932 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c9935 83bae406000022           cmp        dword ptr [edx + 0x6e4], 0x22
004c993c 740c                     je         0x4c994a
004c993e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9941 83b8e406000026           cmp        dword ptr [eax + 0x6e4], 0x26
004c9948 7557                     jne        0x4c99a1
004c994a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c994d 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004c9953 e8a829f4ff               call       0x40c300
004c9958 8bc8                     mov        ecx, eax
004c995a e851c0fbff               call       0x4859b0
004c995f 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c9962 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c9965 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004c996b 8d8c00d4000000           lea        ecx, [eax + eax + 0xd4]
004c9972 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004c9975 6a00                     push       0
004c9977 6aff                     push       -1
004c9979 51                       push       ecx
004c997a 0f57c0                   xorps      xmm0, xmm0
004c997d f30f110424               movss      dword ptr [esp], xmm0
004c9982 8d55f0                   lea        edx, [ebp - 0x10]
004c9985 52                       push       edx
004c9986 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004c9989 50                       push       eax
004c998a 6814fd5600               push       0x56fd14
004c998f 8d4db8                   lea        ecx, [ebp - 0x48]
004c9992 51                       push       ecx
004c9993 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004c9996 e845f7faff               call       0x4790e0
004c999b 90                       nop        
004c999c e9ca000000               jmp        0x4c9a6b
004c99a1 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c99a4 83bae40600001f           cmp        dword ptr [edx + 0x6e4], 0x1f
004c99ab 7e0c                     jle        0x4c99b9
004c99ad 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c99b0 83b8e40600001b           cmp        dword ptr [eax + 0x6e4], 0x1b
004c99b7 7554                     jne        0x4c9a0d
004c99b9 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c99bc 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004c99c2 e83929f4ff               call       0x40c300
004c99c7 8bc8                     mov        ecx, eax
004c99c9 e8e2bffbff               call       0x4859b0
004c99ce 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c99d1 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c99d4 8b8220070000             mov        eax, dword ptr [edx + 0x720]
004c99da 8d8c0004010000           lea        ecx, [eax + eax + 0x104]
004c99e1 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004c99e4 6a00                     push       0
004c99e6 6aff                     push       -1
004c99e8 51                       push       ecx
004c99e9 0f57c0                   xorps      xmm0, xmm0
004c99ec f30f110424               movss      dword ptr [esp], xmm0
004c99f1 8d55f0                   lea        edx, [ebp - 0x10]
004c99f4 52                       push       edx
004c99f5 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c99f8 50                       push       eax
004c99f9 6814fd5600               push       0x56fd14
004c99fe 8d4db4                   lea        ecx, [ebp - 0x4c]
004c9a01 51                       push       ecx
004c9a02 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c9a05 e8d6f6faff               call       0x4790e0
004c9a0a 90                       nop        
004c9a0b eb5e                     jmp        0x4c9a6b
004c9a0d 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c9a10 83bae406000021           cmp        dword ptr [edx + 0x6e4], 0x21
004c9a17 7f52                     jg         0x4c9a6b
004c9a19 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9a1c 8b88f0060000             mov        ecx, dword ptr [eax + 0x6f0]
004c9a22 e8d928f4ff               call       0x40c300
004c9a27 8bc8                     mov        ecx, eax
004c9a29 e882bffbff               call       0x4859b0
004c9a2e 8945bc                   mov        dword ptr [ebp - 0x44], eax
004c9a31 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c9a34 8b9120070000             mov        edx, dword ptr [ecx + 0x720]
004c9a3a 8d84121c010000           lea        eax, [edx + edx + 0x11c]
004c9a41 8945c0                   mov        dword ptr [ebp - 0x40], eax
004c9a44 6a00                     push       0
004c9a46 6aff                     push       -1
004c9a48 51                       push       ecx
004c9a49 0f57c0                   xorps      xmm0, xmm0
004c9a4c f30f110424               movss      dword ptr [esp], xmm0
004c9a51 8d4df0                   lea        ecx, [ebp - 0x10]
004c9a54 51                       push       ecx
004c9a55 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
004c9a58 52                       push       edx
004c9a59 6814fd5600               push       0x56fd14
004c9a5e 8d45b0                   lea        eax, [ebp - 0x50]
004c9a61 50                       push       eax
004c9a62 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004c9a65 e876f6faff               call       0x4790e0
004c9a6a 90                       nop        
004c9a6b 8d4de4                   lea        ecx, [ebp - 0x1c]
004c9a6e 51                       push       ecx
004c9a6f 8d4df0                   lea        ecx, [ebp - 0x10]
004c9a72 e869fcf5ff               call       0x4296e0
004c9a77 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004c9a7c f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004c9a84 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004c9a89 e969feffff               jmp        0x4c98f7
004c9a8e 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c9a91 c7422001000000           mov        dword ptr [edx + 0x20], 1
004c9a98 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c9a9b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c9a9e 33cd                     xor        ecx, ebp
004c9aa0 e8498f0700               call       0x5429ee
004c9aa5 8be5                     mov        esp, ebp
004c9aa7 5d                       pop        ebp
004c9aa8 c20800                   ret        8
004c9aab cc                       int3       
004c9aac cc                       int3       
004c9aad cc                       int3       
004c9aae cc                       int3       
004c9aaf cc                       int3       
