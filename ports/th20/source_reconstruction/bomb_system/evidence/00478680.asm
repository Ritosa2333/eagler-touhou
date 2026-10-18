00478680 55                       push       ebp
00478681 8bec                     mov        ebp, esp
00478683 81ecac000000             sub        esp, 0xac
00478689 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047868e 33c5                     xor        eax, ebp
00478690 8945fc                   mov        dword ptr [ebp - 4], eax
00478693 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00478696 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478699 81c1c8000000             add        ecx, 0xc8
0047869f e82c48fdff               call       0x44ced0
004786a4 8945cc                   mov        dword ptr [ebp - 0x34], eax
004786a7 6a00                     push       0
004786a9 e88281feff               call       0x460830
004786ae 83c404                   add        esp, 4
004786b1 8945bc                   mov        dword ptr [ebp - 0x44], eax
004786b4 6a28                     push       0x28
004786b6 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004786b9 e8a2090000               call       0x479060
004786be c745c42c010000           mov        dword ptr [ebp - 0x3c], 0x12c
004786c5 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
004786c9 7518                     jne        0x4786e3
004786cb 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004786ce 83c174                   add        ecx, 0x74
004786d1 e86a090000               call       0x479040
004786d6 83c8ff                   or         eax, 0xffffffff
004786d9 e902050000               jmp        0x478be0
004786de e9c1030000               jmp        0x478aa4
004786e3 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004786e6 83c008                   add        eax, 8
004786e9 8945b8                   mov        dword ptr [ebp - 0x48], eax
004786ec 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004786ef 51                       push       ecx
004786f0 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004786f3 e898aefaff               call       0x423590
004786f8 0fb6d0                   movzx      edx, al
004786fb 85d2                     test       edx, edx
004786fd 0f84a1030000             je         0x478aa4
00478703 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00478706 83c008                   add        eax, 8
00478709 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0047870c 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0047870f 51                       push       ecx
00478710 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
00478713 e8c8feffff               call       0x4785e0
00478718 0fb6d0                   movzx      edx, al
0047871b 85d2                     test       edx, edx
0047871d 744e                     je         0x47876d
0047871f 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478722 81c1c8000000             add        ecx, 0xc8
00478728 e813090000               call       0x479040
0047872d 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478730 83c174                   add        ecx, 0x74
00478733 e808090000               call       0x479040
00478738 6a00                     push       0
0047873a e8f180feff               call       0x460830
0047873f 83c404                   add        esp, 4
00478742 8945b0                   mov        dword ptr [ebp - 0x50], eax
00478745 51                       push       ecx
00478746 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0047874e f30f110424               movss      dword ptr [esp], xmm0
00478753 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00478756 e825090000               call       0x479080
0047875b 6a00                     push       0
0047875d e8ce80feff               call       0x460830
00478762 83c404                   add        esp, 4
00478765 8bc8                     mov        ecx, eax
00478767 e854090000               call       0x4790c0
0047876c 90                       nop        
0047876d 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478770 81c1c4000000             add        ecx, 0xc4
00478776 e8650bfbff               call       0x4292e0
0047877b 51                       push       ecx
0047877c d91c24                   fstp       dword ptr [esp]
0047877f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00478782 e8097efdff               call       0x450590
00478787 90                       nop        
00478788 6a00                     push       0
0047878a e8a180feff               call       0x460830
0047878f 83c404                   add        esp, 4
00478792 8bc8                     mov        ecx, eax
00478794 e887080000               call       0x479020
00478799 0f57c0                   xorps      xmm0, xmm0
0047879c 0f2f00                   comiss     xmm0, dword ptr [eax]
0047879f 7627                     jbe        0x4787c8
004787a1 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004787a9 f30f5e0540fa5600         divss      xmm0, dword ptr [0x56fa40]
004787b1 51                       push       ecx
004787b2 f30f110424               movss      dword ptr [esp], xmm0
004787b7 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004787ba 81c1c4000000             add        ecx, 0xc4
004787c0 e89b98ffff               call       0x472060
004787c5 90                       nop        
004787c6 eb43                     jmp        0x47880b
004787c8 6a00                     push       0
004787ca e86180feff               call       0x460830
004787cf 83c404                   add        esp, 4
004787d2 8bc8                     mov        ecx, eax
004787d4 e847080000               call       0x479020
004787d9 f30f1000                 movss      xmm0, dword ptr [eax]
004787dd 0f2f05e8e05600           comiss     xmm0, dword ptr [0x56e0e8]
004787e4 7625                     jbe        0x47880b
004787e6 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004787ee f30f5e0540fa5600         divss      xmm0, dword ptr [0x56fa40]
004787f6 51                       push       ecx
004787f7 f30f110424               movss      dword ptr [esp], xmm0
004787fc 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004787ff 81c1c4000000             add        ecx, 0xc4
00478805 e8b6a8fdff               call       0x4530c0
0047880a 90                       nop        
0047880b 6a00                     push       0
0047880d e81e80feff               call       0x460830
00478812 83c404                   add        esp, 4
00478815 8945ac                   mov        dword ptr [ebp - 0x54], eax
00478818 51                       push       ecx
00478819 f30f1005ece05600         movss      xmm0, dword ptr [0x56e0ec]
00478821 f30f110424               movss      dword ptr [esp], xmm0
00478826 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00478829 e852080000               call       0x479080
0047882e 6a00                     push       0
00478830 e8fb7ffeff               call       0x460830
00478835 83c404                   add        esp, 4
00478838 8bc8                     mov        ecx, eax
0047883a e81180feff               call       0x460850
0047883f 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478842 81c1b8000000             add        ecx, 0xb8
00478848 8b10                     mov        edx, dword ptr [eax]
0047884a 8911                     mov        dword ptr [ecx], edx
0047884c 8b5004                   mov        edx, dword ptr [eax + 4]
0047884f 895104                   mov        dword ptr [ecx + 4], edx
00478852 8b4008                   mov        eax, dword ptr [eax + 8]
00478855 894108                   mov        dword ptr [ecx + 8], eax
00478858 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0047885b 83c108                   add        ecx, 8
0047885e 894da8                   mov        dword ptr [ebp - 0x58], ecx
00478861 6a03                     push       3
00478863 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00478866 e895050000               call       0x478e00
0047886b 85c0                     test       eax, eax
0047886d 0f8431020000             je         0x478aa4
00478873 8d4dec                   lea        ecx, [ebp - 0x14]
00478876 e895a5faff               call       0x422e10
0047887b 0f57c0                   xorps      xmm0, xmm0
0047887e f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00478883 51                       push       ecx
00478884 f30f100584fb5600         movss      xmm0, dword ptr [0x56fb84]
0047888c f30f110424               movss      dword ptr [esp], xmm0
00478891 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478894 81c1c4000000             add        ecx, 0xc4
0047889a e8410afbff               call       0x4292e0
0047889f 51                       push       ecx
004788a0 d91c24                   fstp       dword ptr [esp]
004788a3 8d55ec                   lea        edx, [ebp - 0x14]
004788a6 52                       push       edx
004788a7 e8840afcff               call       0x439330
004788ac 83c40c                   add        esp, 0xc
004788af 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004788b2 05b8000000               add        eax, 0xb8
004788b7 50                       push       eax
004788b8 8d4dec                   lea        ecx, [ebp - 0x14]
004788bb e8200efbff               call       0x4296e0
004788c0 8d4df8                   lea        ecx, [ebp - 8]
004788c3 e8f8d3faff               call       0x425cc0
004788c8 90                       nop        
004788c9 6a00                     push       0
004788cb e8f0050000               call       0x478ec0
004788d0 83c404                   add        esp, 4
004788d3 8945a0                   mov        dword ptr [ebp - 0x60], eax
004788d6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004788d9 81c1c4000000             add        ecx, 0xc4
004788df e8fc09fbff               call       0x4292e0
004788e4 d95da4                   fstp       dword ptr [ebp - 0x5c]
004788e7 6a32                     push       0x32
004788e9 6a00                     push       0
004788eb 51                       push       ecx
004788ec f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
004788f1 f30f110424               movss      dword ptr [esp], xmm0
004788f6 51                       push       ecx
004788f7 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
004788ff f30f110424               movss      dword ptr [esp], xmm0
00478904 51                       push       ecx
00478905 f30f100514f15600         movss      xmm0, dword ptr [0x56f114]
0047890d f30f110424               movss      dword ptr [esp], xmm0
00478912 8d4dec                   lea        ecx, [ebp - 0x14]
00478915 51                       push       ecx
00478916 8d9568ffffff             lea        edx, [ebp - 0x98]
0047891c 52                       push       edx
0047891d 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00478920 e8bb910400               call       0x4c1ae0
00478925 8b00                     mov        eax, dword ptr [eax]
00478927 8945f8                   mov        dword ptr [ebp - 8], eax
0047892a 8d8d64ffffff             lea        ecx, [ebp - 0x9c]
00478930 51                       push       ecx
00478931 8d4df8                   lea        ecx, [ebp - 8]
00478934 e8d7920400               call       0x4c1c10
00478939 90                       nop        
0047893a 51                       push       ecx
0047893b f30f100508f15600         movss      xmm0, dword ptr [0x56f108]
00478943 f30f110424               movss      dword ptr [esp], xmm0
00478948 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0047894b 81c1c4000000             add        ecx, 0xc4
00478951 e88a09fbff               call       0x4292e0
00478956 51                       push       ecx
00478957 d91c24                   fstp       dword ptr [esp]
0047895a 8d55ec                   lea        edx, [ebp - 0x14]
0047895d 52                       push       edx
0047895e e8cd09fcff               call       0x439330
00478963 83c40c                   add        esp, 0xc
00478966 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00478969 05b8000000               add        eax, 0xb8
0047896e 50                       push       eax
0047896f 8d4dec                   lea        ecx, [ebp - 0x14]
00478972 e8690dfbff               call       0x4296e0
00478977 90                       nop        
00478978 6a00                     push       0
0047897a e841050000               call       0x478ec0
0047897f 83c404                   add        esp, 4
00478982 894598                   mov        dword ptr [ebp - 0x68], eax
00478985 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478988 81c1c4000000             add        ecx, 0xc4
0047898e e84d09fbff               call       0x4292e0
00478993 d95d9c                   fstp       dword ptr [ebp - 0x64]
00478996 6a0f                     push       0xf
00478998 6a00                     push       0
0047899a 51                       push       ecx
0047899b f30f10459c               movss      xmm0, dword ptr [ebp - 0x64]
004789a0 f30f110424               movss      dword ptr [esp], xmm0
004789a5 51                       push       ecx
004789a6 f30f100598cd5600         movss      xmm0, dword ptr [0x56cd98]
004789ae f30f110424               movss      dword ptr [esp], xmm0
004789b3 51                       push       ecx
004789b4 f30f100514f15600         movss      xmm0, dword ptr [0x56f114]
004789bc f30f110424               movss      dword ptr [esp], xmm0
004789c1 8d4dec                   lea        ecx, [ebp - 0x14]
004789c4 51                       push       ecx
004789c5 8d9560ffffff             lea        edx, [ebp - 0xa0]
004789cb 52                       push       edx
004789cc 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004789cf e80c910400               call       0x4c1ae0
004789d4 8b00                     mov        eax, dword ptr [eax]
004789d6 8945f8                   mov        dword ptr [ebp - 8], eax
004789d9 8d8d5cffffff             lea        ecx, [ebp - 0xa4]
004789df 51                       push       ecx
004789e0 8d4df8                   lea        ecx, [ebp - 8]
004789e3 e828920400               call       0x4c1c10
004789e8 90                       nop        
004789e9 51                       push       ecx
004789ea f30f100588fb5600         movss      xmm0, dword ptr [0x56fb88]
004789f2 f30f110424               movss      dword ptr [esp], xmm0
004789f7 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004789fa 81c1c4000000             add        ecx, 0xc4
00478a00 e8db08fbff               call       0x4292e0
00478a05 51                       push       ecx
00478a06 d91c24                   fstp       dword ptr [esp]
00478a09 8d55ec                   lea        edx, [ebp - 0x14]
00478a0c 52                       push       edx
00478a0d e81e09fcff               call       0x439330
00478a12 83c40c                   add        esp, 0xc
00478a15 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
00478a18 05b8000000               add        eax, 0xb8
00478a1d 50                       push       eax
00478a1e 8d4dec                   lea        ecx, [ebp - 0x14]
00478a21 e8ba0cfbff               call       0x4296e0
00478a26 90                       nop        
00478a27 6a00                     push       0
00478a29 e892040000               call       0x478ec0
00478a2e 83c404                   add        esp, 4
00478a31 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
00478a37 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478a3a 81c1c4000000             add        ecx, 0xc4
00478a40 e89b08fbff               call       0x4292e0
00478a45 d99d70ffffff             fstp       dword ptr [ebp - 0x90]
00478a4b 6a0f                     push       0xf
00478a4d 6a00                     push       0
00478a4f 51                       push       ecx
00478a50 f30f108570ffffff         movss      xmm0, dword ptr [ebp - 0x90]
00478a58 f30f110424               movss      dword ptr [esp], xmm0
00478a5d 51                       push       ecx
00478a5e f30f1005a0cd5600         movss      xmm0, dword ptr [0x56cda0]
00478a66 f30f110424               movss      dword ptr [esp], xmm0
00478a6b 51                       push       ecx
00478a6c f30f100514f15600         movss      xmm0, dword ptr [0x56f114]
00478a74 f30f110424               movss      dword ptr [esp], xmm0
00478a79 8d4dec                   lea        ecx, [ebp - 0x14]
00478a7c 51                       push       ecx
00478a7d 8d9558ffffff             lea        edx, [ebp - 0xa8]
00478a83 52                       push       edx
00478a84 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00478a8a e851900400               call       0x4c1ae0
00478a8f 8b00                     mov        eax, dword ptr [eax]
00478a91 8945f8                   mov        dword ptr [ebp - 8], eax
00478a94 8d8d54ffffff             lea        ecx, [ebp - 0xac]
00478a9a 51                       push       ecx
00478a9b 8d4df8                   lea        ecx, [ebp - 8]
00478a9e e86d910400               call       0x4c1c10
00478aa3 90                       nop        
00478aa4 8d4dd4                   lea        ecx, [ebp - 0x2c]
00478aa7 e864a3faff               call       0x422e10
00478aac 8d4de0                   lea        ecx, [ebp - 0x20]
00478aaf e85ca3faff               call       0x422e10
00478ab4 c745c800000000           mov        dword ptr [ebp - 0x38], 0
00478abb c7856cffffff39000000     mov        dword ptr [ebp - 0x94], 0x39
00478ac5 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00478ac8 81c2c8000000             add        edx, 0xc8
00478ace 895594                   mov        dword ptr [ebp - 0x6c], edx
00478ad1 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
00478ad4 50                       push       eax
00478ad5 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00478adb 51                       push       ecx
00478adc 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00478adf e88c3bfdff               call       0x44c670
00478ae4 8945c0                   mov        dword ptr [ebp - 0x40], eax
00478ae7 837dc000                 cmp        dword ptr [ebp - 0x40], 0
00478aeb 0f84d4000000             je         0x478bc5
00478af1 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00478af4 e8c7eefbff               call       0x4379c0
00478af9 d95d90                   fstp       dword ptr [ebp - 0x70]
00478afc f30f104590               movss      xmm0, dword ptr [ebp - 0x70]
00478b01 f30f59057cfb5600         mulss      xmm0, dword ptr [0x56fb7c]
00478b09 f30f1145e0               movss      dword ptr [ebp - 0x20], xmm0
00478b0e 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00478b11 e88aeefbff               call       0x4379a0
00478b16 d95d8c                   fstp       dword ptr [ebp - 0x74]
00478b19 f30f10458c               movss      xmm0, dword ptr [ebp - 0x74]
00478b1e f30f590580fb5600         mulss      xmm0, dword ptr [0x56fb80]
00478b26 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00478b2b 8d55d4                   lea        edx, [ebp - 0x2c]
00478b2e 52                       push       edx
00478b2f 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00478b32 e8c9030000               call       0x478f00
00478b37 6a00                     push       0
00478b39 e842030000               call       0x478e80
00478b3e 83c404                   add        esp, 4
00478b41 894584                   mov        dword ptr [ebp - 0x7c], eax
00478b44 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478b47 81c1c4000000             add        ecx, 0xc4
00478b4d e88e07fbff               call       0x4292e0
00478b52 d95d88                   fstp       dword ptr [ebp - 0x78]
00478b55 6a00                     push       0
00478b57 6a00                     push       0
00478b59 51                       push       ecx
00478b5a f30f104588               movss      xmm0, dword ptr [ebp - 0x78]
00478b5f f30f110424               movss      dword ptr [esp], xmm0
00478b64 8d45e0                   lea        eax, [ebp - 0x20]
00478b67 50                       push       eax
00478b68 8d4dd4                   lea        ecx, [ebp - 0x2c]
00478b6b 51                       push       ecx
00478b6c 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
00478b6f e81c410000               call       0x47cc90
00478b74 6a00                     push       0
00478b76 e865030000               call       0x478ee0
00478b7b 83c404                   add        esp, 4
00478b7e 89857cffffff             mov        dword ptr [ebp - 0x84], eax
00478b84 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00478b87 81c1c4000000             add        ecx, 0xc4
00478b8d e84e07fbff               call       0x4292e0
00478b92 d95d80                   fstp       dword ptr [ebp - 0x80]
00478b95 6a01                     push       1
00478b97 6a00                     push       0
00478b99 51                       push       ecx
00478b9a f30f104580               movss      xmm0, dword ptr [ebp - 0x80]
00478b9f f30f110424               movss      dword ptr [esp], xmm0
00478ba4 8d55e0                   lea        edx, [ebp - 0x20]
00478ba7 52                       push       edx
00478ba8 8d45d4                   lea        eax, [ebp - 0x2c]
00478bab 50                       push       eax
00478bac 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00478bb2 e8f9110500               call       0x4c9db0
00478bb7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
00478bba 83c101                   add        ecx, 1
00478bbd 894dc8                   mov        dword ptr [ebp - 0x38], ecx
00478bc0 e900ffffff               jmp        0x478ac5
00478bc5 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00478bc8 83c208                   add        edx, 8
00478bcb 899578ffffff             mov        dword ptr [ebp - 0x88], edx
00478bd1 6a00                     push       0
00478bd3 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
00478bd9 e862a9faff               call       0x423540
00478bde 33c0                     xor        eax, eax
00478be0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478be3 33cd                     xor        ecx, ebp
00478be5 e8049e0c00               call       0x5429ee
00478bea 8be5                     mov        esp, ebp
00478bec 5d                       pop        ebp
00478bed c3                       ret        
00478bee cc                       int3       
00478bef cc                       int3       
