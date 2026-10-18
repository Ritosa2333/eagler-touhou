004657d0 55                       push       ebp
004657d1 8bec                     mov        ebp, esp
004657d3 83ec48                   sub        esp, 0x48
004657d6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004657db 33c5                     xor        eax, ebp
004657dd 8945fc                   mov        dword ptr [ebp - 4], eax
004657e0 894de0                   mov        dword ptr [ebp - 0x20], ecx
004657e3 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004657e6 83c008                   add        eax, 8
004657e9 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004657ec 6a00                     push       0
004657ee 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004657f1 e85aabfeff               call       0x450350
004657f6 90                       nop        
004657f7 51                       push       ecx
004657f8 0f57c0                   xorps      xmm0, xmm0
004657fb f30f110424               movss      dword ptr [esp], xmm0
00465800 51                       push       ecx
00465801 f30f100508f15600         movss      xmm0, dword ptr [0x56f108]
00465809 f30f110424               movss      dword ptr [esp], xmm0
0046580e 51                       push       ecx
0046580f f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00465817 f30f110424               movss      dword ptr [esp], xmm0
0046581c 8d4df0                   lea        ecx, [ebp - 0x10]
0046581f e8acd5fbff               call       0x422dd0
00465824 90                       nop        
00465825 c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0046582c eb09                     jmp        0x465837
0046582e 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00465831 83c101                   add        ecx, 1
00465834 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00465837 837ddc04                 cmp        dword ptr [ebp - 0x24], 4
0046583b 7d5a                     jge        0x465897
0046583d 6a00                     push       0
0046583f e8dc1cfdff               call       0x437520
00465844 83c404                   add        esp, 4
00465847 8945d0                   mov        dword ptr [ebp - 0x30], eax
0046584a 6a02                     push       2
0046584c 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0046584f e8ac78ffff               call       0x45d100
00465854 8945cc                   mov        dword ptr [ebp - 0x34], eax
00465857 6a00                     push       0
00465859 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0046585c 83c203                   add        edx, 3
0046585f 52                       push       edx
00465860 6945dce4050000           imul       eax, dword ptr [ebp - 0x24], 0x5e4
00465867 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0046586a 8d540108                 lea        edx, [ecx + eax + 8]
0046586e 52                       push       edx
0046586f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00465872 e8392afdff               call       0x4382b0
00465877 6945dce4050000           imul       eax, dword ptr [ebp - 0x24], 0x5e4
0046587e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00465881 8d540108                 lea        edx, [ecx + eax + 8]
00465885 8955c8                   mov        dword ptr [ebp - 0x38], edx
00465888 8d45f0                   lea        eax, [ebp - 0x10]
0046588b 50                       push       eax
0046588c 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0046588f e8cc84ffff               call       0x45dd60
00465894 90                       nop        
00465895 eb97                     jmp        0x46582e
00465897 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0046589a c7817c1d000000000000     mov        dword ptr [ecx + 0x1d7c], 0
004658a4 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004658a7 83c208                   add        edx, 8
004658aa 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004658ad 6a34                     push       0x34
004658af 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004658b2 e899aafeff               call       0x450350
004658b7 90                       nop        
004658b8 51                       push       ecx
004658b9 0f57c0                   xorps      xmm0, xmm0
004658bc f30f110424               movss      dword ptr [esp], xmm0
004658c1 51                       push       ecx
004658c2 f30f100508f15600         movss      xmm0, dword ptr [0x56f108]
004658ca f30f110424               movss      dword ptr [esp], xmm0
004658cf 51                       push       ecx
004658d0 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
004658d8 f30f110424               movss      dword ptr [esp], xmm0
004658dd 8d4de4                   lea        ecx, [ebp - 0x1c]
004658e0 e8ebd4fbff               call       0x422dd0
004658e5 90                       nop        
004658e6 c745d800000000           mov        dword ptr [ebp - 0x28], 0
004658ed eb09                     jmp        0x4658f8
004658ef 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004658f2 83c001                   add        eax, 1
004658f5 8945d8                   mov        dword ptr [ebp - 0x28], eax
004658f8 837dd804                 cmp        dword ptr [ebp - 0x28], 4
004658fc 7d20                     jge        0x46591e
004658fe 694dd8e4050000           imul       ecx, dword ptr [ebp - 0x28], 0x5e4
00465905 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00465908 8d440a08                 lea        eax, [edx + ecx + 8]
0046590c 8945c0                   mov        dword ptr [ebp - 0x40], eax
0046590f 8d4de4                   lea        ecx, [ebp - 0x1c]
00465912 51                       push       ecx
00465913 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00465916 e84584ffff               call       0x45dd60
0046591b 90                       nop        
0046591c ebd1                     jmp        0x4658ef
0046591e 6a00                     push       0
00465920 e8fb1bfdff               call       0x437520
00465925 83c404                   add        esp, 4
00465928 8945bc                   mov        dword ptr [ebp - 0x44], eax
0046592b 6a02                     push       2
0046592d 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00465930 e8cb77ffff               call       0x45d100
00465935 8945b8                   mov        dword ptr [ebp - 0x48], eax
00465938 6a00                     push       0
0046593a 6a0b                     push       0xb
0046593c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0046593f 81c298170000             add        edx, 0x1798
00465945 52                       push       edx
00465946 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00465949 e86229fdff               call       0x4382b0
0046594e 33c0                     xor        eax, eax
00465950 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00465953 33cd                     xor        ecx, ebp
00465955 e894d00d00               call       0x5429ee
0046595a 8be5                     mov        esp, ebp
0046595c 5d                       pop        ebp
0046595d c20800                   ret        8
