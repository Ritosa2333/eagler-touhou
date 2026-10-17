004878b0 55                       push       ebp
004878b1 8bec                     mov        ebp, esp
004878b3 83ec30                   sub        esp, 0x30
004878b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004878b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004878bc 8b4878                   mov        ecx, dword ptr [eax + 0x78]
004878bf 83e101                   and        ecx, 1
004878c2 7505                     jne        0x4878c9
004878c4 e93d020000               jmp        0x487b06
004878c9 8b0d9c065c00             mov        ecx, dword ptr [0x5c069c]
004878cf e8cc0a0000               call       0x4883a0
004878d4 ba04000000               mov        edx, 4
004878d9 6bc200                   imul       eax, edx, 0
004878dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004878df 8d4c0114                 lea        ecx, [ecx + eax + 0x14]
004878e3 e85817ffff               call       0x479040
004878e8 ba04000000               mov        edx, 4
004878ed c1e200                   shl        edx, 0
004878f0 8b45fc                   mov        eax, dword ptr [ebp - 4]
004878f3 8d4c1014                 lea        ecx, [eax + edx + 0x14]
004878f7 e84417ffff               call       0x479040
004878fc b904000000               mov        ecx, 4
00487901 d1e1                     shl        ecx, 1
00487903 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487906 8d4c0a14                 lea        ecx, [edx + ecx + 0x14]
0048790a e83117ffff               call       0x479040
0048790f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487912 8b4878                   mov        ecx, dword ptr [eax + 0x78]
00487915 83e1fe                   and        ecx, 0xfffffffe
00487918 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048791b 894a78                   mov        dword ptr [edx + 0x78], ecx
0048791e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487921 83c110                   add        ecx, 0x10
00487924 e8a783fcff               call       0x44fcd0
00487929 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048792c 8b4878                   mov        ecx, dword ptr [eax + 0x78]
0048792f 83e1df                   and        ecx, 0xffffffdf
00487932 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487935 894a78                   mov        dword ptr [edx + 0x78], ecx
00487938 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
0048793e e86d110000               call       0x488ab0
00487943 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487946 83c120                   add        ecx, 0x20
00487949 e88283fcff               call       0x44fcd0
0048794e 90                       nop        
0048794f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487952 8b4878                   mov        ecx, dword ptr [eax + 0x78]
00487955 d1e9                     shr        ecx, 1
00487957 83e101                   and        ecx, 1
0048795a 0f847b010000             je         0x487adb
00487960 8b55fc                   mov        edx, dword ptr [ebp - 4]
00487963 8b427c                   mov        eax, dword ptr [edx + 0x7c]
00487966 50                       push       eax
00487967 b968a55b00               mov        ecx, 0x5ba568
0048796c e8df0b0000               call       0x488550
00487971 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487974 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
00487977 52                       push       edx
00487978 6a00                     push       0
0048797a 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00487980 e85b170300               call       0x4b90e0
00487985 90                       nop        
00487986 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
0048798c e86f0e0000               call       0x488800
00487991 85c0                     test       eax, eax
00487993 0f8531010000             jne        0x487aca
00487999 b968a55b00               mov        ecx, 0x5ba568
0048799e e8ad0e0000               call       0x488850
004879a3 0fb6c0                   movzx      eax, al
004879a6 85c0                     test       eax, eax
004879a8 7409                     je         0x4879b3
004879aa c745f001000000           mov        dword ptr [ebp - 0x10], 1
004879b1 eb07                     jmp        0x4879ba
004879b3 c745f000000000           mov        dword ptr [ebp - 0x10], 0
004879ba 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004879bd 894df8                   mov        dword ptr [ebp - 8], ecx
004879c0 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004879c6 e885820800               call       0x50fc50
004879cb 05080b0000               add        eax, 0xb08
004879d0 8945e8                   mov        dword ptr [ebp - 0x18], eax
004879d3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004879d6 8b4274                   mov        eax, dword ptr [edx + 0x74]
004879d9 8945ec                   mov        dword ptr [ebp - 0x14], eax
004879dc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004879df 51                       push       ecx
004879e0 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004879e3 e828f4ffff               call       0x486e10
004879e8 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004879eb 8b55f8                   mov        edx, dword ptr [ebp - 8]
004879ee 52                       push       edx
004879ef 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004879f2 e8190b0000               call       0x488510
004879f7 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004879fd e8fe0c0000               call       0x488700
00487a02 05080b0000               add        eax, 0xb08
00487a07 8945dc                   mov        dword ptr [ebp - 0x24], eax
00487a0a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00487a0d 8b4874                   mov        ecx, dword ptr [eax + 0x74]
00487a10 894de0                   mov        dword ptr [ebp - 0x20], ecx
00487a13 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00487a16 52                       push       edx
00487a17 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00487a1a e8f1f3ffff               call       0x486e10
00487a1f 8945d8                   mov        dword ptr [ebp - 0x28], eax
00487a22 8b45f8                   mov        eax, dword ptr [ebp - 8]
00487a25 50                       push       eax
00487a26 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00487a29 e8e20a0000               call       0x488510
00487a2e c745f800000000           mov        dword ptr [ebp - 8], 0
00487a35 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00487a3c eb09                     jmp        0x487a47
00487a3e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00487a41 83c101                   add        ecx, 1
00487a44 894df4                   mov        dword ptr [ebp - 0xc], ecx
00487a47 837df471                 cmp        dword ptr [ebp - 0xc], 0x71
00487a4b 7d6d                     jge        0x487aba
00487a4d 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487a53 e8a80c0000               call       0x488700
00487a58 05080b0000               add        eax, 0xb08
00487a5d 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00487a60 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00487a63 52                       push       edx
00487a64 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00487a67 e8a4f3ffff               call       0x486e10
00487a6c b904000000               mov        ecx, 4
00487a71 6bd100                   imul       edx, ecx, 0
00487a74 83bc10c000000000         cmp        dword ptr [eax + edx + 0xc0], 0
00487a7c 7531                     jne        0x487aaf
00487a7e 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00487a84 e8770c0000               call       0x488700
00487a89 05080b0000               add        eax, 0xb08
00487a8e 8945d0                   mov        dword ptr [ebp - 0x30], eax
00487a91 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00487a94 50                       push       eax
00487a95 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00487a98 e873f3ffff               call       0x486e10
00487a9d b904000000               mov        ecx, 4
00487aa2 c1e100                   shl        ecx, 0
00487aa5 83bc08c000000000         cmp        dword ptr [eax + ecx + 0xc0], 0
00487aad 7409                     je         0x487ab8
00487aaf 8b55f8                   mov        edx, dword ptr [ebp - 8]
00487ab2 83c201                   add        edx, 1
00487ab5 8955f8                   mov        dword ptr [ebp - 8], edx
00487ab8 eb84                     jmp        0x487a3e
00487aba 837df871                 cmp        dword ptr [ebp - 8], 0x71
00487abe 7c0a                     jl         0x487aca
00487ac0 6a28                     push       0x28
00487ac2 e8397e0a00               call       0x52f900
00487ac7 83c404                   add        esp, 4
00487aca 6a00                     push       0
00487acc 6a2e                     push       0x2e
00487ace b930a85b00               mov        ecx, 0x5ba830
00487ad3 e898f2f9ff               call       0x426d70
00487ad8 90                       nop        
00487ad9 eb10                     jmp        0x487aeb
00487adb 6a00                     push       0
00487add 6a01                     push       1
00487adf 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
00487ae5 e8f6150300               call       0x4b90e0
00487aea 90                       nop        
00487aeb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00487aee e88d0d0000               call       0x488880
00487af3 85c0                     test       eax, eax
00487af5 740f                     je         0x487b06
00487af7 6a00                     push       0
00487af9 6a45                     push       0x45
00487afb b930a85b00               mov        ecx, 0x5ba830
00487b00 e86bf2f9ff               call       0x426d70
00487b05 90                       nop        
00487b06 8be5                     mov        esp, ebp
00487b08 5d                       pop        ebp
00487b09 c3                       ret        
00487b0a cc                       int3       
00487b0b cc                       int3       
00487b0c cc                       int3       
00487b0d cc                       int3       
00487b0e cc                       int3       
00487b0f cc                       int3       
