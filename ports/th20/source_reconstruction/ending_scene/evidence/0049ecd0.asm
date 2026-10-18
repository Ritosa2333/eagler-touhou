0049ecd0 55                       push       ebp
0049ecd1 8bec                     mov        ebp, esp
0049ecd3 6aff                     push       -1
0049ecd5 6873955600               push       0x569573
0049ecda 64a100000000             mov        eax, dword ptr fs:[0]
0049ece0 50                       push       eax
0049ece1 83ec44                   sub        esp, 0x44
0049ece4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049ece9 33c5                     xor        eax, ebp
0049eceb 50                       push       eax
0049ecec 8d45f4                   lea        eax, [ebp - 0xc]
0049ecef 64a300000000             mov        dword ptr fs:[0], eax
0049ecf5 894df0                   mov        dword ptr [ebp - 0x10], ecx
0049ecf8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ecfb c70000000000             mov        dword ptr [eax], 0
0049ed01 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed04 83c104                   add        ecx, 4
0049ed07 e88440f8ff               call       0x422d90
0049ed0c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed0f 83c114                   add        ecx, 0x14
0049ed12 e87940f8ff               call       0x422d90
0049ed17 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed1a 83c124                   add        ecx, 0x24
0049ed1d e86e40f8ff               call       0x422d90
0049ed22 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed25 83c134                   add        ecx, 0x34
0049ed28 e863feffff               call       0x49eb90
0049ed2d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed30 83c148                   add        ecx, 0x48
0049ed33 e858feffff               call       0x49eb90
0049ed38 68102e4200               push       0x422e10
0049ed3d 6a02                     push       2
0049ed3f 6a0c                     push       0xc
0049ed41 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed44 83c160                   add        ecx, 0x60
0049ed47 51                       push       ecx
0049ed48 e8d3cef6ff               call       0x40bc20
0049ed4d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049ed50 c7427800000000           mov        dword ptr [edx + 0x78], 0
0049ed57 33c0                     xor        eax, eax
0049ed59 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed5c 83c17c                   add        ecx, 0x7c
0049ed5f 8901                     mov        dword ptr [ecx], eax
0049ed61 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049ed64 c7828000000000000000     mov        dword ptr [edx + 0x80], 0
0049ed6e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ed71 c78084000000ffffff00     mov        dword ptr [eax + 0x84], 0xffffff
0049ed7b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ed7e c78188000000ffffff00     mov        dword ptr [ecx + 0x88], 0xffffff
0049ed88 33d2                     xor        edx, edx
0049ed8a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ed8d 058c000000               add        eax, 0x8c
0049ed92 8910                     mov        dword ptr [eax], edx
0049ed94 895004                   mov        dword ptr [eax + 4], edx
0049ed97 895008                   mov        dword ptr [eax + 8], edx
0049ed9a 89500c                   mov        dword ptr [eax + 0xc], edx
0049ed9d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049eda0 81c19c000000             add        ecx, 0x9c
0049eda6 e835feffff               call       0x49ebe0
0049edab 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049edae 81c1dc000000             add        ecx, 0xdc
0049edb4 e8c7c9f6ff               call       0x40b780
0049edb9 c745fc00000000           mov        dword ptr [ebp - 4], 0
0049edc0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049edc3 c781ec00000000000000     mov        dword ptr [ecx + 0xec], 0
0049edcd c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0049edd4 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049edd7 83c234                   add        edx, 0x34
0049edda 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0049eddd 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0049ede0 e8bbcff6ff               call       0x40bda0
0049ede5 8945ec                   mov        dword ptr [ebp - 0x14], eax
0049ede8 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0049edeb e8b01f0000               call       0x4a0da0
0049edf0 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0049edf3 eb09                     jmp        0x49edfe
0049edf5 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0049edf8 83c004                   add        eax, 4
0049edfb 8945ec                   mov        dword ptr [ebp - 0x14], eax
0049edfe 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0049ee01 3b4dc4                   cmp        ecx, dword ptr [ebp - 0x3c]
0049ee04 0f8483000000             je         0x49ee8d
0049ee0a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0049ee0d 8955e8                   mov        dword ptr [ebp - 0x18], edx
0049ee10 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0049ee16 e85518fdff               call       0x470670
0049ee1b 8945c0                   mov        dword ptr [ebp - 0x40], eax
0049ee1e 6a00                     push       0
0049ee20 6aff                     push       -1
0049ee22 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0049ee25 83c04c                   add        eax, 0x4c
0049ee28 50                       push       eax
0049ee29 681cfe5600               push       0x56fe1c
0049ee2e 8d4db4                   lea        ecx, [ebp - 0x4c]
0049ee31 51                       push       ecx
0049ee32 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0049ee35 e8361efbff               call       0x450c70
0049ee3a 8b10                     mov        edx, dword ptr [eax]
0049ee3c 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0049ee3f 8910                     mov        dword ptr [eax], edx
0049ee41 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0049ee44 e887e0faff               call       0x44ced0
0049ee49 c6807805000010           mov        byte ptr [eax + 0x578], 0x10
0049ee50 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0049ee53 e878e0faff               call       0x44ced0
0049ee58 c6807905000010           mov        byte ptr [eax + 0x579], 0x10
0049ee5f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0049ee62 e869e0faff               call       0x44ced0
0049ee67 059c040000               add        eax, 0x49c
0049ee6c 8945d0                   mov        dword ptr [ebp - 0x30], eax
0049ee6f 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0049ee72 8b11                     mov        edx, dword ptr [ecx]
0049ee74 81e2fffbffff             and        edx, 0xfffffbff
0049ee7a 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0049ee7d 8910                     mov        dword ptr [eax], edx
0049ee7f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0049ee82 83c101                   add        ecx, 1
0049ee85 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0049ee88 e968ffffff               jmp        0x49edf5
0049ee8d c745d800000000           mov        dword ptr [ebp - 0x28], 0
0049ee94 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049ee97 83c248                   add        edx, 0x48
0049ee9a 8955cc                   mov        dword ptr [ebp - 0x34], edx
0049ee9d 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0049eea0 e8fbcef6ff               call       0x40bda0
0049eea5 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0049eea8 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0049eeab e8f01e0000               call       0x4a0da0
0049eeb0 8945bc                   mov        dword ptr [ebp - 0x44], eax
0049eeb3 eb09                     jmp        0x49eebe
0049eeb5 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0049eeb8 83c004                   add        eax, 4
0049eebb 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0049eebe 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0049eec1 3b4dbc                   cmp        ecx, dword ptr [ebp - 0x44]
0049eec4 0f8483000000             je         0x49ef4d
0049eeca 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0049eecd 8955e0                   mov        dword ptr [ebp - 0x20], edx
0049eed0 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0049eed6 e89517fdff               call       0x470670
0049eedb 8945b8                   mov        dword ptr [ebp - 0x48], eax
0049eede 6a00                     push       0
0049eee0 6aff                     push       -1
0049eee2 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0049eee5 83c051                   add        eax, 0x51
0049eee8 50                       push       eax
0049eee9 681cfe5600               push       0x56fe1c
0049eeee 8d4db0                   lea        ecx, [ebp - 0x50]
0049eef1 51                       push       ecx
0049eef2 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0049eef5 e8761dfbff               call       0x450c70
0049eefa 8b10                     mov        edx, dword ptr [eax]
0049eefc 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0049eeff 8910                     mov        dword ptr [eax], edx
0049ef01 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0049ef04 e8c7dffaff               call       0x44ced0
0049ef09 c6807805000010           mov        byte ptr [eax + 0x578], 0x10
0049ef10 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0049ef13 e8b8dffaff               call       0x44ced0
0049ef18 c6807905000010           mov        byte ptr [eax + 0x579], 0x10
0049ef1f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0049ef22 e8a9dffaff               call       0x44ced0
0049ef27 059c040000               add        eax, 0x49c
0049ef2c 8945c8                   mov        dword ptr [ebp - 0x38], eax
0049ef2f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0049ef32 8b11                     mov        edx, dword ptr [ecx]
0049ef34 81e2fffbffff             and        edx, 0xfffffbff
0049ef3a 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0049ef3d 8910                     mov        dword ptr [eax], edx
0049ef3f 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0049ef42 83c101                   add        ecx, 1
0049ef45 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0049ef48 e968ffffff               jmp        0x49eeb5
0049ef4d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049ef50 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0049ef53 8902                     mov        dword ptr [edx], eax
0049ef55 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049ef58 8b5508                   mov        edx, dword ptr [ebp + 8]
0049ef5b 89515c                   mov        dword ptr [ecx + 0x5c], edx
0049ef5e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ef61 8b487c                   mov        ecx, dword ptr [eax + 0x7c]
0049ef64 83c901                   or         ecx, 1
0049ef67 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0049ef6a 894a7c                   mov        dword ptr [edx + 0x7c], ecx
0049ef6d c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0049ef74 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0049ef77 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0049ef7a 64890d00000000           mov        dword ptr fs:[0], ecx
0049ef81 59                       pop        ecx
0049ef82 8be5                     mov        esp, ebp
0049ef84 5d                       pop        ebp
0049ef85 c20800                   ret        8
0049ef88 cc                       int3       
0049ef89 cc                       int3       
0049ef8a cc                       int3       
0049ef8b cc                       int3       
0049ef8c cc                       int3       
0049ef8d cc                       int3       
0049ef8e cc                       int3       
0049ef8f cc                       int3       
