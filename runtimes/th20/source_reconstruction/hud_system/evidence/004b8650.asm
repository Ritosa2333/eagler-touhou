004b8650 55                       push       ebp
004b8651 8bec                     mov        ebp, esp
004b8653 83ec3c                   sub        esp, 0x3c
004b8656 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004b865b 33c5                     xor        eax, ebp
004b865d 8945fc                   mov        dword ptr [ebp - 4], eax
004b8660 894de8                   mov        dword ptr [ebp - 0x18], ecx
004b8663 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8666 83c06c                   add        eax, 0x6c
004b8669 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004b866c 6a00                     push       0
004b866e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004b8671 e80abff5ff               call       0x414580
004b8676 833800                   cmp        dword ptr [eax], 0
004b8679 7505                     jne        0x4b8680
004b867b e95c010000               jmp        0x4b87dc
004b8680 51                       push       ecx
004b8681 0f57c0                   xorps      xmm0, xmm0
004b8684 f30f110424               movss      dword ptr [esp], xmm0
004b8689 51                       push       ecx
004b868a 0f57c0                   xorps      xmm0, xmm0
004b868d f30f110424               movss      dword ptr [esp], xmm0
004b8692 51                       push       ecx
004b8693 0f57c0                   xorps      xmm0, xmm0
004b8696 f30f110424               movss      dword ptr [esp], xmm0
004b869b 8d4df0                   lea        ecx, [ebp - 0x10]
004b869e e82da7f6ff               call       0x422dd0
004b86a3 b907000000               mov        ecx, 7
004b86a8 2b4d10                   sub        ecx, dword ptr [ebp + 0x10]
004b86ab f30f2ac1                 cvtsi2ss   xmm0, ecx
004b86af f30f590548fd5600         mulss      xmm0, dword ptr [0x56fd48]
004b86b7 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b86bc c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004b86c3 eb09                     jmp        0x4b86ce
004b86c5 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b86c8 83c201                   add        edx, 1
004b86cb 8955ec                   mov        dword ptr [ebp - 0x14], edx
004b86ce 837dec07                 cmp        dword ptr [ebp - 0x14], 7
004b86d2 7329                     jae        0x4b86fd
004b86d4 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b86d7 83c06c                   add        eax, 0x6c
004b86da 8945e0                   mov        dword ptr [ebp - 0x20], eax
004b86dd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b86e0 51                       push       ecx
004b86e1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004b86e4 e897bef5ff               call       0x414580
004b86e9 8b10                     mov        edx, dword ptr [eax]
004b86eb 8955dc                   mov        dword ptr [ebp - 0x24], edx
004b86ee 8d45f0                   lea        eax, [ebp - 0x10]
004b86f1 50                       push       eax
004b86f2 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004b86f5 e86656faff               call       0x45dd60
004b86fa 90                       nop        
004b86fb ebc8                     jmp        0x4b86c5
004b86fd c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004b8704 eb09                     jmp        0x4b870f
004b8706 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8709 83c101                   add        ecx, 1
004b870c 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b870f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8712 3b5508                   cmp        edx, dword ptr [ebp + 8]
004b8715 7d1f                     jge        0x4b8736
004b8717 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b871a 83c06c                   add        eax, 0x6c
004b871d 8945d8                   mov        dword ptr [ebp - 0x28], eax
004b8720 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8723 51                       push       ecx
004b8724 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004b8727 e854bef5ff               call       0x414580
004b872c 8b08                     mov        ecx, dword ptr [eax]
004b872e e8fddbfcff               call       0x486330
004b8733 90                       nop        
004b8734 ebd0                     jmp        0x4b8706
004b8736 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8739 3b5510                   cmp        edx, dword ptr [ebp + 0x10]
004b873c 7d3c                     jge        0x4b877a
004b873e 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8741 83c06c                   add        eax, 0x6c
004b8744 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004b8747 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b874a 51                       push       ecx
004b874b 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004b874e e82dbef5ff               call       0x414580
004b8753 8b10                     mov        edx, dword ptr [eax]
004b8755 8955cc                   mov        dword ptr [ebp - 0x34], edx
004b8758 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004b875b 8b0c858c075700           mov        ecx, dword ptr [eax*4 + 0x57078c]
004b8762 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004b8765 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004b8768 52                       push       edx
004b8769 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004b876c e82ffeffff               call       0x4b85a0
004b8771 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004b8774 83c001                   add        eax, 1
004b8777 8945ec                   mov        dword ptr [ebp - 0x14], eax
004b877a eb09                     jmp        0x4b8785
004b877c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b877f 83c101                   add        ecx, 1
004b8782 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b8785 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8788 3b5510                   cmp        edx, dword ptr [ebp + 0x10]
004b878b 7d1f                     jge        0x4b87ac
004b878d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8790 83c06c                   add        eax, 0x6c
004b8793 8945c8                   mov        dword ptr [ebp - 0x38], eax
004b8796 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8799 51                       push       ecx
004b879a 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004b879d e8debdf5ff               call       0x414580
004b87a2 8b08                     mov        ecx, dword ptr [eax]
004b87a4 e8c7dbfcff               call       0x486370
004b87a9 90                       nop        
004b87aa ebd0                     jmp        0x4b877c
004b87ac eb09                     jmp        0x4b87b7
004b87ae 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b87b1 83c201                   add        edx, 1
004b87b4 8955ec                   mov        dword ptr [ebp - 0x14], edx
004b87b7 837dec07                 cmp        dword ptr [ebp - 0x14], 7
004b87bb 731f                     jae        0x4b87dc
004b87bd 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b87c0 83c06c                   add        eax, 0x6c
004b87c3 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004b87c6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b87c9 51                       push       ecx
004b87ca 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004b87cd e8aebdf5ff               call       0x414580
004b87d2 8b08                     mov        ecx, dword ptr [eax]
004b87d4 e8a7fdffff               call       0x4b8580
004b87d9 90                       nop        
004b87da ebd2                     jmp        0x4b87ae
004b87dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b87df 33cd                     xor        ecx, ebp
004b87e1 e808a20800               call       0x5429ee
004b87e6 8be5                     mov        esp, ebp
004b87e8 5d                       pop        ebp
004b87e9 c20c00                   ret        0xc
004b87ec cc                       int3       
004b87ed cc                       int3       
004b87ee cc                       int3       
004b87ef cc                       int3       
