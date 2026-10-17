00483920 55                       push       ebp
00483921 8bec                     mov        ebp, esp
00483923 83ec44                   sub        esp, 0x44
00483926 56                       push       esi
00483927 894dfc                   mov        dword ptr [ebp - 4], ecx
0048392a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048392d 05a0000000               add        eax, 0xa0
00483932 8945f8                   mov        dword ptr [ebp - 8], eax
00483935 6a02                     push       2
00483937 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0048393a e8418dffff               call       0x47c680
0048393f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00483942 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483945 81c1a0000000             add        ecx, 0xa0
0048394b 894df4                   mov        dword ptr [ebp - 0xc], ecx
0048394e 6a02                     push       2
00483950 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00483953 e8288dffff               call       0x47c680
00483958 8b5030                   mov        edx, dword ptr [eax + 0x30]
0048395b 8955f0                   mov        dword ptr [ebp - 0x10], edx
0048395e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00483961 50                       push       eax
00483962 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00483965 e856fcf9ff               call       0x4235c0
0048396a 0fb6c8                   movzx      ecx, al
0048396d 85c9                     test       ecx, ecx
0048396f 7424                     je         0x483995
00483971 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483974 8b8290000000             mov        eax, dword ptr [edx + 0x90]
0048397a 83e0f7                   and        eax, 0xfffffff7
0048397d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483980 898190000000             mov        dword ptr [ecx + 0x90], eax
00483986 b801000000               mov        eax, 1
0048398b e933010000               jmp        0x483ac3
00483990 e909010000               jmp        0x483a9e
00483995 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483998 81c2a0000000             add        edx, 0xa0
0048399e 8955e8                   mov        dword ptr [ebp - 0x18], edx
004839a1 6a02                     push       2
004839a3 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004839a6 e8d58cffff               call       0x47c680
004839ab 8bf0                     mov        esi, eax
004839ad b9e4ef5a00               mov        ecx, 0x5aefe4
004839b2 e82959faff               call       0x4292e0
004839b7 d95de4                   fstp       dword ptr [ebp - 0x1c]
004839ba f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004839bf f30f594614               mulss      xmm0, dword ptr [esi + 0x14]
004839c4 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
004839c9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004839cc 81c184000000             add        ecx, 0x84
004839d2 e80959faff               call       0x4292e0
004839d7 d95ddc                   fstp       dword ptr [ebp - 0x24]
004839da 51                       push       ecx
004839db f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
004839e0 f30f110424               movss      dword ptr [esp], xmm0
004839e5 51                       push       ecx
004839e6 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004839eb f30f110424               movss      dword ptr [esp], xmm0
004839f0 e80b4cfbff               call       0x438600
004839f5 83c404                   add        esp, 4
004839f8 d91c24                   fstp       dword ptr [esp]
004839fb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004839fe 81c184000000             add        ecx, 0x84
00483a04 e817f5fcff               call       0x452f20
00483a09 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483a0c 05a0000000               add        eax, 0xa0
00483a11 8945d8                   mov        dword ptr [ebp - 0x28], eax
00483a14 6a02                     push       2
00483a16 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00483a19 e8628cffff               call       0x47c680
00483a1e 8bf0                     mov        esi, eax
00483a20 b9e4ef5a00               mov        ecx, 0x5aefe4
00483a25 e8b658faff               call       0x4292e0
00483a2a d95dd4                   fstp       dword ptr [ebp - 0x2c]
00483a2d f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00483a32 f30f594610               mulss      xmm0, dword ptr [esi + 0x10]
00483a37 f30f1145cc               movss      dword ptr [ebp - 0x34], xmm0
00483a3c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483a3f f30f104120               movss      xmm0, dword ptr [ecx + 0x20]
00483a44 f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
00483a49 f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
00483a4e f30f5845cc               addss      xmm0, dword ptr [ebp - 0x34]
00483a53 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483a56 f30f114220               movss      dword ptr [edx + 0x20], xmm0
00483a5b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00483a5e f30f104020               movss      xmm0, dword ptr [eax + 0x20]
00483a63 f30f1145c8               movss      dword ptr [ebp - 0x38], xmm0
00483a68 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483a6b 81c184000000             add        ecx, 0x84
00483a71 e86a58faff               call       0x4292e0
00483a76 d95dc4                   fstp       dword ptr [ebp - 0x3c]
00483a79 51                       push       ecx
00483a7a f30f1045c8               movss      xmm0, dword ptr [ebp - 0x38]
00483a7f f30f110424               movss      dword ptr [esp], xmm0
00483a84 51                       push       ecx
00483a85 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
00483a8a f30f110424               movss      dword ptr [esp], xmm0
00483a8f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483a92 83c170                   add        ecx, 0x70
00483a95 51                       push       ecx
00483a96 e89558fbff               call       0x439330
00483a9b 83c40c                   add        esp, 0xc
00483a9e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00483aa1 81c2a0000000             add        edx, 0xa0
00483aa7 8955c0                   mov        dword ptr [ebp - 0x40], edx
00483aaa 6a02                     push       2
00483aac 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00483aaf e8cc8bffff               call       0x47c680
00483ab4 8945bc                   mov        dword ptr [ebp - 0x44], eax
00483ab7 6a00                     push       0
00483ab9 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00483abc e87ffaf9ff               call       0x423540
00483ac1 33c0                     xor        eax, eax
00483ac3 5e                       pop        esi
00483ac4 8be5                     mov        esp, ebp
00483ac6 5d                       pop        ebp
00483ac7 c3                       ret        
00483ac8 cc                       int3       
00483ac9 cc                       int3       
00483aca cc                       int3       
00483acb cc                       int3       
00483acc cc                       int3       
00483acd cc                       int3       
00483ace cc                       int3       
00483acf cc                       int3       
