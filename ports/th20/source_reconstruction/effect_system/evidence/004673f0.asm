004673f0 55                       push       ebp
004673f1 8bec                     mov        ebp, esp
004673f3 51                       push       ecx
004673f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004673f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004673fa e8a14effff               call       0x45c2a0
004673ff 8b4508                   mov        eax, dword ptr [ebp + 8]
00467402 83e001                   and        eax, 1
00467405 7411                     je         0x467418
00467407 689c010000               push       0x19c
0046740c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046740f 51                       push       ecx
00467410 e878b30d00               call       0x54278d
00467415 83c408                   add        esp, 8
00467418 8b45fc                   mov        eax, dword ptr [ebp - 4]
0046741b 8be5                     mov        esp, ebp
0046741d 5d                       pop        ebp
0046741e c20400                   ret        4
00467421 cc                       int3       
00467422 cc                       int3       
00467423 cc                       int3       
00467424 cc                       int3       
00467425 cc                       int3       
00467426 cc                       int3       
00467427 cc                       int3       
00467428 cc                       int3       
00467429 cc                       int3       
0046742a cc                       int3       
0046742b cc                       int3       
0046742c cc                       int3       
0046742d cc                       int3       
0046742e cc                       int3       
0046742f cc                       int3       
00467430 53                       push       ebx
00467431 8bdc                     mov        ebx, esp
00467433 83ec08                   sub        esp, 8
00467436 83e4f8                   and        esp, 0xfffffff8
00467439 83c404                   add        esp, 4
0046743c 55                       push       ebp
0046743d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
00467440 896c2404                 mov        dword ptr [esp + 4], ebp
00467444 8bec                     mov        ebp, esp
00467446 81ecb0000000             sub        esp, 0xb0
0046744c a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00467451 33c5                     xor        eax, ebp
00467453 8945fc                   mov        dword ptr [ebp - 4], eax
00467456 894de8                   mov        dword ptr [ebp - 0x18], ecx
00467459 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0046745c 0578010000               add        eax, 0x178
00467461 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00467467 6a3c                     push       0x3c
00467469 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
0046746f e8ecc0fbff               call       0x423560
00467474 0fb6c8                   movzx      ecx, al
00467477 85c9                     test       ecx, ecx
00467479 0f8462030000             je         0x4677e1
0046747f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00467482 81c178010000             add        ecx, 0x178
00467488 e8a35bffff               call       0x45d030
0046748d 85c0                     test       eax, eax
0046748f 0f84be020000             je         0x467753
00467495 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00467498 83c208                   add        edx, 8
0046749b 899578ffffff             mov        dword ptr [ebp - 0x88], edx
004674a1 8d8574ffffff             lea        eax, [ebp - 0x8c]
004674a7 50                       push       eax
004674a8 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004674ae e89d070000               call       0x467c50
004674b3 8b08                     mov        ecx, dword ptr [eax]
004674b5 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004674b8 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004674bb 83c208                   add        edx, 8
004674be 8955cc                   mov        dword ptr [ebp - 0x34], edx
004674c1 8d8570ffffff             lea        eax, [ebp - 0x90]
004674c7 50                       push       eax
004674c8 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004674cb e8b088ffff               call       0x45fd80
004674d0 8b08                     mov        ecx, dword ptr [eax]
004674d2 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004674d5 6a01                     push       1
004674d7 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004674da 52                       push       edx
004674db 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
004674de 50                       push       eax
004674df 8d8d6cffffff             lea        ecx, [ebp - 0x94]
004674e5 51                       push       ecx
004674e6 e845fdffff               call       0x467230
004674eb 83c410                   add        esp, 0x10
004674ee 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004674f1 81c278010000             add        edx, 0x178
004674f7 8955c0                   mov        dword ptr [ebp - 0x40], edx
004674fa 6a1e                     push       0x1e
004674fc 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004674ff e88cc0fbff               call       0x423590
00467504 0fb6c0                   movzx      eax, al
00467507 85c0                     test       eax, eax
00467509 7440                     je         0x46754b
0046750b 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0046750e 81c18c010000             add        ecx, 0x18c
00467514 894dbc                   mov        dword ptr [ebp - 0x44], ecx
00467517 51                       push       ecx
00467518 f30f100580f25600         movss      xmm0, dword ptr [0x56f280]
00467520 f30f110424               movss      dword ptr [esp], xmm0
00467525 8d9564ffffff             lea        edx, [ebp - 0x9c]
0046752b 52                       push       edx
0046752c 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
0046752f e87cfeffff               call       0x4673b0
00467534 50                       push       eax
00467535 8d855cffffff             lea        eax, [ebp - 0xa4]
0046753b 50                       push       eax
0046753c 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0046753f 81c194010000             add        ecx, 0x194
00467545 e81624fdff               call       0x439960
0046754a 90                       nop        
0046754b 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0046754e 51                       push       ecx
0046754f f30f10818c010000         movss      xmm0, dword ptr [ecx + 0x18c]
00467557 f30f110424               movss      dword ptr [esp], xmm0
0046755c 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0046755f 51                       push       ecx
00467560 f30f108290010000         movss      xmm0, dword ptr [edx + 0x190]
00467568 f30f110424               movss      dword ptr [esp], xmm0
0046756d 8d4dec                   lea        ecx, [ebp - 0x14]
00467570 e8db1cfcff               call       0x429250
00467575 8d4df4                   lea        ecx, [ebp - 0xc]
00467578 e82323fdff               call       0x4398a0
0046757d 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00467580 0578010000               add        eax, 0x178
00467585 8945b8                   mov        dword ptr [ebp - 0x48], eax
00467588 6a06                     push       6
0046758a 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0046758d e8cebffbff               call       0x423560
00467592 0fb6c8                   movzx      ecx, al
00467595 85c9                     test       ecx, ecx
00467597 742a                     je         0x4675c3
00467599 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0046759c 81c278010000             add        edx, 0x178
004675a2 8955b4                   mov        dword ptr [ebp - 0x4c], edx
004675a5 51                       push       ecx
004675a6 f30f10050cf05600         movss      xmm0, dword ptr [0x56f00c]
004675ae f30f110424               movss      dword ptr [esp], xmm0
004675b3 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004675b6 e8c5fdffff               call       0x467380
004675bb d95dd8                   fstp       dword ptr [ebp - 0x28]
004675be e98c000000               jmp        0x46764f
004675c3 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004675c6 0578010000               add        eax, 0x178
004675cb 8945b0                   mov        dword ptr [ebp - 0x50], eax
004675ce 6a1e                     push       0x1e
004675d0 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004675d3 e8989affff               call       0x461070
004675d8 0fb6c8                   movzx      ecx, al
004675db 85c9                     test       ecx, ecx
004675dd 740a                     je         0x4675e9
004675df 0f57c0                   xorps      xmm0, xmm0
004675e2 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
004675e7 eb5c                     jmp        0x467645
004675e9 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004675ec 81c278010000             add        edx, 0x178
004675f2 8955ac                   mov        dword ptr [ebp - 0x54], edx
004675f5 6a18                     push       0x18
004675f7 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004675fa e8c1bffbff               call       0x4235c0
004675ff 0fb6c0                   movzx      eax, al
00467602 85c0                     test       eax, eax
00467604 7428                     je         0x46762e
00467606 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00467609 81c178010000             add        ecx, 0x178
0046760f e87c89faff               call       0x40ff90
00467614 b91e000000               mov        ecx, 0x1e
00467619 2bc8                     sub        ecx, eax
0046761b f30f2ac1                 cvtsi2ss   xmm0, ecx
0046761f f30f5e050cf05600         divss      xmm0, dword ptr [0x56f00c]
00467627 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
0046762c eb0d                     jmp        0x46763b
0046762e f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00467636 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
0046763b f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
00467640 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00467645 f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
0046764a f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
0046764f f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
00467654 f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
00467659 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0046765c 81c170010000             add        ecx, 0x170
00467662 e8791cfcff               call       0x4292e0
00467667 51                       push       ecx
00467668 d91c24                   fstp       dword ptr [esp]
0046766b e8b021fdff               call       0x439820
00467670 83c404                   add        esp, 4
00467673 d95da8                   fstp       dword ptr [ebp - 0x58]
00467676 f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
0046767b 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0046767e f30f598274010000         mulss      xmm0, dword ptr [edx + 0x174]
00467686 f30f5945a4               mulss      xmm0, dword ptr [ebp - 0x5c]
0046768b b958675b00               mov        ecx, 0x5b6758
00467690 f30f11459c               movss      dword ptr [ebp - 0x64], xmm0
00467695 e8f60bfdff               call       0x438290
0046769a d95da0                   fstp       dword ptr [ebp - 0x60]
0046769d f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
004676a2 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
004676aa f30f104d9c               movss      xmm1, dword ptr [ebp - 0x64]
004676af f30f59c8                 mulss      xmm1, xmm0
004676b3 51                       push       ecx
004676b4 f30f110c24               movss      dword ptr [esp], xmm1
004676b9 8d45ec                   lea        eax, [ebp - 0x14]
004676bc 50                       push       eax
004676bd 8d4df4                   lea        ecx, [ebp - 0xc]
004676c0 51                       push       ecx
004676c1 e8fa010000               call       0x4678c0
004676c6 83c40c                   add        esp, 0xc
004676c9 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004676cc 81c278010000             add        edx, 0x178
004676d2 895598                   mov        dword ptr [ebp - 0x68], edx
004676d5 6a1e                     push       0x1e
004676d7 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004676da e8b1befbff               call       0x423590
004676df 0fb6c0                   movzx      eax, al
004676e2 85c0                     test       eax, eax
004676e4 7425                     je         0x46770b
004676e6 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004676ee f30f5e05fcf25600         divss      xmm0, dword ptr [0x56f2fc]
004676f6 51                       push       ecx
004676f7 f30f110424               movss      dword ptr [esp], xmm0
004676fc 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004676ff 81c170010000             add        ecx, 0x170
00467705 e8b6b9feff               call       0x4530c0
0046770a 90                       nop        
0046770b 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0046770e 81c194010000             add        ecx, 0x194
00467714 894d94                   mov        dword ptr [ebp - 0x6c], ecx
00467717 8d55f4                   lea        edx, [ebp - 0xc]
0046771a 52                       push       edx
0046771b 8d8554ffffff             lea        eax, [ebp - 0xac]
00467721 50                       push       eax
00467722 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00467725 e8161ffcff               call       0x429640
0046772a 8b08                     mov        ecx, dword ptr [eax]
0046772c 8b5004                   mov        edx, dword ptr [eax + 4]
0046772f 894d80                   mov        dword ptr [ebp - 0x80], ecx
00467732 895584                   mov        dword ptr [ebp - 0x7c], edx
00467735 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00467738 83c008                   add        eax, 8
0046773b 894590                   mov        dword ptr [ebp - 0x70], eax
0046773e 6a00                     push       0
00467740 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00467743 e8285fffff               call       0x45d670
00467748 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0046774b 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
0046774e 8908                     mov        dword ptr [eax], ecx
00467750 895004                   mov        dword ptr [eax + 4], edx
00467753 6a00                     push       0
00467755 e8d690ffff               call       0x460830
0046775a 83c404                   add        esp, 4
0046775d 8bc8                     mov        ecx, eax
0046775f e8ec90ffff               call       0x460850
00467764 f30f100500f35600         movss      xmm0, dword ptr [0x56f300]
0046776c 0f2f00                   comiss     xmm0, dword ptr [eax]
0046776f 766e                     jbe        0x4677df
00467771 6a00                     push       0
00467773 e8b890ffff               call       0x460830
00467778 83c404                   add        esp, 4
0046777b 8bc8                     mov        ecx, eax
0046777d e8ce90ffff               call       0x460850
00467782 f30f104004               movss      xmm0, dword ptr [eax + 4]
00467787 0f2f050cf15600           comiss     xmm0, dword ptr [0x56f10c]
0046778e 764f                     jbe        0x4677df
00467790 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00467793 05f8000000               add        eax, 0xf8
00467798 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0046779b 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0046779e e8fd45faff               call       0x40bda0
004677a3 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004677a6 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004677a9 e802f8ffff               call       0x466fb0
004677ae 89458c                   mov        dword ptr [ebp - 0x74], eax
004677b1 eb09                     jmp        0x4677bc
004677b3 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004677b6 83c104                   add        ecx, 4
004677b9 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004677bc 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004677bf 3b558c                   cmp        edx, dword ptr [ebp - 0x74]
004677c2 741b                     je         0x4677df
004677c4 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004677c7 8945d0                   mov        dword ptr [ebp - 0x30], eax
004677ca 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004677cd 0fb65103                 movzx      edx, byte ptr [ecx + 3]
004677d1 83fa20                   cmp        edx, 0x20
004677d4 7c07                     jl         0x4677dd
004677d6 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004677d9 c6400320                 mov        byte ptr [eax + 3], 0x20
004677dd ebd4                     jmp        0x4677b3
004677df eb07                     jmp        0x4677e8
004677e1 b801000000               mov        eax, 1
004677e6 eb18                     jmp        0x467800
004677e8 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004677eb 81c178010000             add        ecx, 0x178
004677f1 894d88                   mov        dword ptr [ebp - 0x78], ecx
004677f4 6a00                     push       0
004677f6 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004677f9 e842bdfbff               call       0x423540
004677fe 33c0                     xor        eax, eax
00467800 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00467803 33cd                     xor        ecx, ebp
00467805 e8e4b10d00               call       0x5429ee
0046780a 8be5                     mov        esp, ebp
0046780c 5d                       pop        ebp
0046780d 8be3                     mov        esp, ebx
0046780f 5b                       pop        ebx
00467810 c3                       ret        
00467811 cc                       int3       
00467812 cc                       int3       
00467813 cc                       int3       
00467814 cc                       int3       
00467815 cc                       int3       
00467816 cc                       int3       
00467817 cc                       int3       
00467818 cc                       int3       
00467819 cc                       int3       
0046781a cc                       int3       
0046781b cc                       int3       
0046781c cc                       int3       
0046781d cc                       int3       
0046781e cc                       int3       
0046781f cc                       int3       
