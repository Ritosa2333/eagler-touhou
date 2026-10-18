004c9580 55                       push       ebp
004c9581 8bec                     mov        ebp, esp
004c9583 83ec5c                   sub        esp, 0x5c
004c9586 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c958b 33c5                     xor        eax, ebp
004c958d 8945fc                   mov        dword ptr [ebp - 4], eax
004c9590 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c9593 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004c9597 7413                     je         0x4c95ac
004c9599 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c959c 83b8cc06000000           cmp        dword ptr [eax + 0x6cc], 0
004c95a3 7407                     je         0x4c95ac
004c95a5 33c0                     xor        eax, eax
004c95a7 e96f020000               jmp        0x4c981b
004c95ac c745d800000000           mov        dword ptr [ebp - 0x28], 0
004c95b3 8d4df0                   lea        ecx, [ebp - 0x10]
004c95b6 e85598f5ff               call       0x422e10
004c95bb 8d4de4                   lea        ecx, [ebp - 0x1c]
004c95be e84d98f5ff               call       0x422e10
004c95c3 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c95cb f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c95d3 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004c95d8 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c95e0 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c95e8 51                       push       ecx
004c95e9 f30f110424               movss      dword ptr [esp], xmm0
004c95ee 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c95f1 51                       push       ecx
004c95f2 f30f104170               movss      xmm0, dword ptr [ecx + 0x70]
004c95f7 f30f110424               movss      dword ptr [esp], xmm0
004c95fc 8d55e4                   lea        edx, [ebp - 0x1c]
004c95ff 52                       push       edx
004c9600 e82bfdf6ff               call       0x439330
004c9605 83c40c                   add        esp, 0xc
004c9608 0f57c0                   xorps      xmm0, xmm0
004c960b f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004c9610 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9613 83c058                   add        eax, 0x58
004c9616 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c9619 8d4de4                   lea        ecx, [ebp - 0x1c]
004c961c 51                       push       ecx
004c961d 8d55a4                   lea        edx, [ebp - 0x5c]
004c9620 52                       push       edx
004c9621 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c9624 e847fff5ff               call       0x429570
004c9629 8b08                     mov        ecx, dword ptr [eax]
004c962b 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c962e 8b5004                   mov        edx, dword ptr [eax + 4]
004c9631 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c9634 8b4008                   mov        eax, dword ptr [eax + 8]
004c9637 8945f8                   mov        dword ptr [ebp - 8], eax
004c963a 8d4de4                   lea        ecx, [ebp - 0x1c]
004c963d 51                       push       ecx
004c963e 8d4de4                   lea        ecx, [ebp - 0x1c]
004c9641 e89a00f6ff               call       0x4296e0
004c9646 90                       nop        
004c9647 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c964f f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004c9657 f30f5845dc               addss      xmm0, dword ptr [ebp - 0x24]
004c965c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c965f f30f104a74               movss      xmm1, dword ptr [edx + 0x74]
004c9664 0f2fc8                   comiss     xmm1, xmm0
004c9667 0f86a1010000             jbe        0x4c980e
004c966d 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c9670 83c001                   add        eax, 1
004c9673 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c9676 51                       push       ecx
004c9677 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c967f f30f110424               movss      dword ptr [esp], xmm0
004c9684 51                       push       ecx
004c9685 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
004c968d f30f110424               movss      dword ptr [esp], xmm0
004c9692 8d4df0                   lea        ecx, [ebp - 0x10]
004c9695 51                       push       ecx
004c9696 e8f5bcfbff               call       0x485390
004c969b 83c40c                   add        esp, 0xc
004c969e 85c0                     test       eax, eax
004c96a0 0f8545010000             jne        0x4c97eb
004c96a6 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c96a9 83bae406000011           cmp        dword ptr [edx + 0x6e4], 0x11
004c96b0 7e18                     jle        0x4c96ca
004c96b2 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c96b5 83b8e406000022           cmp        dword ptr [eax + 0x6e4], 0x22
004c96bc 740c                     je         0x4c96ca
004c96be 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c96c1 83b9e406000026           cmp        dword ptr [ecx + 0x6e4], 0x26
004c96c8 7557                     jne        0x4c9721
004c96ca 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c96cd 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004c96d3 e8282cf4ff               call       0x40c300
004c96d8 8bc8                     mov        ecx, eax
004c96da e8d1c2fbff               call       0x4859b0
004c96df 8945cc                   mov        dword ptr [ebp - 0x34], eax
004c96e2 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c96e5 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004c96eb 8d9409d4000000           lea        edx, [ecx + ecx + 0xd4]
004c96f2 8955d0                   mov        dword ptr [ebp - 0x30], edx
004c96f5 6a00                     push       0
004c96f7 6aff                     push       -1
004c96f9 51                       push       ecx
004c96fa 0f57c0                   xorps      xmm0, xmm0
004c96fd f30f110424               movss      dword ptr [esp], xmm0
004c9702 8d45f0                   lea        eax, [ebp - 0x10]
004c9705 50                       push       eax
004c9706 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004c9709 51                       push       ecx
004c970a 6814fd5600               push       0x56fd14
004c970f 8d55b8                   lea        edx, [ebp - 0x48]
004c9712 52                       push       edx
004c9713 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004c9716 e8c5f9faff               call       0x4790e0
004c971b 90                       nop        
004c971c e9ca000000               jmp        0x4c97eb
004c9721 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9724 83b8e40600001f           cmp        dword ptr [eax + 0x6e4], 0x1f
004c972b 7e0c                     jle        0x4c9739
004c972d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c9730 83b9e40600001b           cmp        dword ptr [ecx + 0x6e4], 0x1b
004c9737 7554                     jne        0x4c978d
004c9739 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c973c 8b8af0060000             mov        ecx, dword ptr [edx + 0x6f0]
004c9742 e8b92bf4ff               call       0x40c300
004c9747 8bc8                     mov        ecx, eax
004c9749 e862c2fbff               call       0x4859b0
004c974e 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004c9751 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9754 8b8850070000             mov        ecx, dword ptr [eax + 0x750]
004c975a 8d940904010000           lea        edx, [ecx + ecx + 0x104]
004c9761 8955c8                   mov        dword ptr [ebp - 0x38], edx
004c9764 6a00                     push       0
004c9766 6aff                     push       -1
004c9768 51                       push       ecx
004c9769 0f57c0                   xorps      xmm0, xmm0
004c976c f30f110424               movss      dword ptr [esp], xmm0
004c9771 8d45f0                   lea        eax, [ebp - 0x10]
004c9774 50                       push       eax
004c9775 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004c9778 51                       push       ecx
004c9779 6814fd5600               push       0x56fd14
004c977e 8d55b4                   lea        edx, [ebp - 0x4c]
004c9781 52                       push       edx
004c9782 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004c9785 e856f9faff               call       0x4790e0
004c978a 90                       nop        
004c978b eb5e                     jmp        0x4c97eb
004c978d 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9790 83b8e406000021           cmp        dword ptr [eax + 0x6e4], 0x21
004c9797 7f52                     jg         0x4c97eb
004c9799 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c979c 8b89f0060000             mov        ecx, dword ptr [ecx + 0x6f0]
004c97a2 e8592bf4ff               call       0x40c300
004c97a7 8bc8                     mov        ecx, eax
004c97a9 e802c2fbff               call       0x4859b0
004c97ae 8945bc                   mov        dword ptr [ebp - 0x44], eax
004c97b1 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c97b4 8b8250070000             mov        eax, dword ptr [edx + 0x750]
004c97ba 8d8c001c010000           lea        ecx, [eax + eax + 0x11c]
004c97c1 894dc0                   mov        dword ptr [ebp - 0x40], ecx
004c97c4 6a00                     push       0
004c97c6 6aff                     push       -1
004c97c8 51                       push       ecx
004c97c9 0f57c0                   xorps      xmm0, xmm0
004c97cc f30f110424               movss      dword ptr [esp], xmm0
004c97d1 8d55f0                   lea        edx, [ebp - 0x10]
004c97d4 52                       push       edx
004c97d5 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004c97d8 50                       push       eax
004c97d9 6814fd5600               push       0x56fd14
004c97de 8d4db0                   lea        ecx, [ebp - 0x50]
004c97e1 51                       push       ecx
004c97e2 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004c97e5 e8f6f8faff               call       0x4790e0
004c97ea 90                       nop        
004c97eb 8d55e4                   lea        edx, [ebp - 0x1c]
004c97ee 52                       push       edx
004c97ef 8d4df0                   lea        ecx, [ebp - 0x10]
004c97f2 e8e9fef5ff               call       0x4296e0
004c97f7 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004c97fc f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004c9804 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004c9809 e939feffff               jmp        0x4c9647
004c980e 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c9811 c7402001000000           mov        dword ptr [eax + 0x20], 1
004c9818 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c981b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c981e 33cd                     xor        ecx, ebp
004c9820 e8c9910700               call       0x5429ee
004c9825 8be5                     mov        esp, ebp
004c9827 5d                       pop        ebp
004c9828 c20800                   ret        8
004c982b cc                       int3       
004c982c cc                       int3       
004c982d cc                       int3       
004c982e cc                       int3       
004c982f cc                       int3       
