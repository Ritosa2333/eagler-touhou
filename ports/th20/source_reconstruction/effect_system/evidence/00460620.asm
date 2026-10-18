00460620 55                       push       ebp
00460621 8bec                     mov        ebp, esp
00460623 83ec3c                   sub        esp, 0x3c
00460626 53                       push       ebx
00460627 894df8                   mov        dword ptr [ebp - 8], ecx
0046062a 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046062d 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00460630 8988e0030000             mov        dword ptr [eax + 0x3e0], ecx
00460636 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460639 83c208                   add        edx, 8
0046063c 8955ec                   mov        dword ptr [ebp - 0x14], edx
0046063f 6a00                     push       0
00460641 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00460644 e827d0ffff               call       0x45d670
00460649 0f57c0                   xorps      xmm0, xmm0
0046064c f30f1100                 movss      dword ptr [eax], xmm0
00460650 8b45f8                   mov        eax, dword ptr [ebp - 8]
00460653 83c008                   add        eax, 8
00460656 8945e8                   mov        dword ptr [ebp - 0x18], eax
00460659 6a00                     push       0
0046065b 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0046065e e80dd0ffff               call       0x45d670
00460663 0f57c0                   xorps      xmm0, xmm0
00460666 f30f114004               movss      dword ptr [eax + 4], xmm0
0046066b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046066e 8b5508                   mov        edx, dword ptr [ebp + 8]
00460671 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00460674 8981cc030000             mov        dword ptr [ecx + 0x3cc], eax
0046067a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046067d 8b5508                   mov        edx, dword ptr [ebp + 8]
00460680 8b4218                   mov        eax, dword ptr [edx + 0x18]
00460683 8981c8030000             mov        dword ptr [ecx + 0x3c8], eax
00460689 6a01                     push       1
0046068b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046068e 81c1d0030000             add        ecx, 0x3d0
00460694 e8872efcff               call       0x423520
00460699 c745f000000000           mov        dword ptr [ebp - 0x10], 0
004606a0 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004606a7 eb09                     jmp        0x4606b2
004606a9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004606ac 83c101                   add        ecx, 1
004606af 894df4                   mov        dword ptr [ebp - 0xc], ecx
004606b2 837df450                 cmp        dword ptr [ebp - 0xc], 0x50
004606b6 0f8df4000000             jge        0x4607b0
004606bc 8b55f8                   mov        edx, dword ptr [ebp - 8]
004606bf 81c288020000             add        edx, 0x288
004606c5 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004606c8 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004606cb 50                       push       eax
004606cc 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004606cf e8ac3efbff               call       0x414580
004606d4 c64002ff                 mov        byte ptr [eax + 2], 0xff
004606d8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004606db 81c188020000             add        ecx, 0x288
004606e1 894de0                   mov        dword ptr [ebp - 0x20], ecx
004606e4 ba50000000               mov        edx, 0x50
004606e9 2b55f4                   sub        edx, dword ptr [ebp - 0xc]
004606ec 6bda03                   imul       ebx, edx, 3
004606ef 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004606f2 50                       push       eax
004606f3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004606f6 e8853efbff               call       0x414580
004606fb 8818                     mov        byte ptr [eax], bl
004606fd b9c4a45b00               mov        ecx, 0x5ba4c4
00460702 e81996feff               call       0x449d20
00460707 8845ff                   mov        byte ptr [ebp - 1], al
0046070a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0046070d 81c188020000             add        ecx, 0x288
00460713 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00460716 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00460719 52                       push       edx
0046071a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0046071d e85e3efbff               call       0x414580
00460722 8a4dff                   mov        cl, byte ptr [ebp - 1]
00460725 884801                   mov        byte ptr [eax + 1], cl
00460728 8b55f8                   mov        edx, dword ptr [ebp - 8]
0046072b 81c288020000             add        edx, 0x288
00460731 8955d8                   mov        dword ptr [ebp - 0x28], edx
00460734 b850000000               mov        eax, 0x50
00460739 2b45f4                   sub        eax, dword ptr [ebp - 0xc]
0046073c 6bd803                   imul       ebx, eax, 3
0046073f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460742 51                       push       ecx
00460743 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00460746 e8353efbff               call       0x414580
0046074b 885803                   mov        byte ptr [eax + 3], bl
0046074e 837df428                 cmp        dword ptr [ebp - 0xc], 0x28
00460752 7c31                     jl         0x460785
00460754 8b55f8                   mov        edx, dword ptr [ebp - 8]
00460757 81c288020000             add        edx, 0x288
0046075d 8955d4                   mov        dword ptr [ebp - 0x2c], edx
00460760 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00460763 c1e005                   shl        eax, 5
00460766 bbff000000               mov        ebx, 0xff
0046076b 2bd8                     sub        ebx, eax
0046076d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00460770 51                       push       ecx
00460771 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00460774 e8073efbff               call       0x414580
00460779 885803                   mov        byte ptr [eax + 3], bl
0046077c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0046077f 83c201                   add        edx, 1
00460782 8955f0                   mov        dword ptr [ebp - 0x10], edx
00460785 837df408                 cmp        dword ptr [ebp - 0xc], 8
00460789 7d20                     jge        0x4607ab
0046078b 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046078e 0588020000               add        eax, 0x288
00460793 8945d0                   mov        dword ptr [ebp - 0x30], eax
00460796 8b5df4                   mov        ebx, dword ptr [ebp - 0xc]
00460799 c1e305                   shl        ebx, 5
0046079c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0046079f 51                       push       ecx
004607a0 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004607a3 e8d83dfbff               call       0x414580
004607a8 885803                   mov        byte ptr [eax + 3], bl
004607ab e9f9feffff               jmp        0x4606a9
004607b0 8b55f8                   mov        edx, dword ptr [ebp - 8]
004607b3 8b4204                   mov        eax, dword ptr [edx + 4]
004607b6 8945cc                   mov        dword ptr [ebp - 0x34], eax
004607b9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004607bc 51                       push       ecx
004607bd 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004607c0 e89bd5ffff               call       0x45dd60
004607c5 8b55f8                   mov        edx, dword ptr [ebp - 8]
004607c8 8b4204                   mov        eax, dword ptr [edx + 4]
004607cb 8945c8                   mov        dword ptr [ebp - 0x38], eax
004607ce 6a05                     push       5
004607d0 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004607d3 e878fbfeff               call       0x450350
004607d8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004607db 8b5104                   mov        edx, dword ptr [ecx + 4]
004607de 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004607e1 6a00                     push       0
004607e3 68ff000000               push       0xff
004607e8 6a00                     push       0
004607ea 6a50                     push       0x50
004607ec 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004607ef e86c84fdff               call       0x438c60
004607f4 33c0                     xor        eax, eax
004607f6 5b                       pop        ebx
004607f7 8be5                     mov        esp, ebp
004607f9 5d                       pop        ebp
004607fa c20800                   ret        8
004607fd cc                       int3       
004607fe cc                       int3       
004607ff cc                       int3       
