0051ed90 55                       push       ebp
0051ed91 8bec                     mov        ebp, esp
0051ed93 6aff                     push       -1
0051ed95 68bdae5600               push       0x56aebd
0051ed9a 64a100000000             mov        eax, dword ptr fs:[0]
0051eda0 50                       push       eax
0051eda1 83ec78                   sub        esp, 0x78
0051eda4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051eda9 33c5                     xor        eax, ebp
0051edab 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051edae 50                       push       eax
0051edaf 8d45f4                   lea        eax, [ebp - 0xc]
0051edb2 64a300000000             mov        dword ptr fs:[0], eax
0051edb8 894dc8                   mov        dword ptr [ebp - 0x38], ecx
0051edbb 6a09                     push       9
0051edbd b940025c00               mov        ecx, 0x5c0240
0051edc2 e849d5eeff               call       0x40c310
0051edc7 50                       push       eax
0051edc8 8d4de4                   lea        ecx, [ebp - 0x1c]
0051edcb e860c9eeff               call       0x40b730
0051edd0 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051edd7 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051edda 83b82459000000           cmp        dword ptr [eax + 0x5924], 0
0051ede1 0f8410040000             je         0x51f1f7
0051ede7 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051edea 8b9124590000             mov        edx, dword ptr [ecx + 0x5924]
0051edf0 8b4214                   mov        eax, dword ptr [edx + 0x14]
0051edf3 8945c0                   mov        dword ptr [ebp - 0x40], eax
0051edf6 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051edf9 8b9124590000             mov        edx, dword ptr [ecx + 0x5924]
0051edff 8b4218                   mov        eax, dword ptr [edx + 0x18]
0051ee02 89459c                   mov        dword ptr [ebp - 0x64], eax
0051ee05 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051ee08 8b9124590000             mov        edx, dword ptr [ecx + 0x5924]
0051ee0e 895594                   mov        dword ptr [ebp - 0x6c], edx
0051ee11 f30f2a05fc875b00         cvtsi2ss   xmm0, dword ptr [0x5b87fc]
0051ee19 51                       push       ecx
0051ee1a f30f110424               movss      dword ptr [esp], xmm0
0051ee1f f30f2a05f8875b00         cvtsi2ss   xmm0, dword ptr [0x5b87f8]
0051ee27 51                       push       ecx
0051ee28 f30f110424               movss      dword ptr [esp], xmm0
0051ee2d 51                       push       ecx
0051ee2e 0f57c0                   xorps      xmm0, xmm0
0051ee31 f30f110424               movss      dword ptr [esp], xmm0
0051ee36 51                       push       ecx
0051ee37 0f57c0                   xorps      xmm0, xmm0
0051ee3a f30f110424               movss      dword ptr [esp], xmm0
0051ee3f 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0051ee42 e869e7f7ff               call       0x49d5b0
0051ee47 8d4dd8                   lea        ecx, [ebp - 0x28]
0051ee4a e8c13ff0ff               call       0x422e10
0051ee4f 51                       push       ecx
0051ee50 0f57c0                   xorps      xmm0, xmm0
0051ee53 f30f110424               movss      dword ptr [esp], xmm0
0051ee58 b86c010000               mov        eax, 0x16c
0051ee5d d1e0                     shl        eax, 1
0051ee5f f30f2a80b8505c00         cvtsi2ss   xmm0, dword ptr [eax + 0x5c50b8]
0051ee67 f30f2a0dfc875b00         cvtsi2ss   xmm1, dword ptr [0x5b87fc]
0051ee6f f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
0051ee77 f30f58c1                 addss      xmm0, xmm1
0051ee7b 51                       push       ecx
0051ee7c f30f110424               movss      dword ptr [esp], xmm0
0051ee81 b96c010000               mov        ecx, 0x16c
0051ee86 d1e1                     shl        ecx, 1
0051ee88 f30f2a81b4505c00         cvtsi2ss   xmm0, dword ptr [ecx + 0x5c50b4]
0051ee90 f30f2a0df8875b00         cvtsi2ss   xmm1, dword ptr [0x5b87f8]
0051ee98 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
0051eea0 f30f58c1                 addss      xmm0, xmm1
0051eea4 51                       push       ecx
0051eea5 f30f110424               movss      dword ptr [esp], xmm0
0051eeaa 8d4dcc                   lea        ecx, [ebp - 0x34]
0051eead e81e3ff0ff               call       0x422dd0
0051eeb2 8d4de8                   lea        ecx, [ebp - 0x18]
0051eeb5 e8268ff2ff               call       0x447de0
0051eeba f30f1005b0cd5600         movss      xmm0, dword ptr [0x56cdb0]
0051eec2 f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
0051eec7 f30f100580f25600         movss      xmm0, dword ptr [0x56f280]
0051eecf f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
0051eed4 c745bc00000000           mov        dword ptr [ebp - 0x44], 0
0051eedb eb09                     jmp        0x51eee6
0051eedd 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0051eee0 83c201                   add        edx, 1
0051eee3 8955bc                   mov        dword ptr [ebp - 0x44], edx
0051eee6 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051eee9 8b8824590000             mov        ecx, dword ptr [eax + 0x5924]
0051eeef 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0051eef2 3b11                     cmp        edx, dword ptr [ecx]
0051eef4 0f8d50020000             jge        0x51f14a
0051eefa c745b800000000           mov        dword ptr [ebp - 0x48], 0
0051ef01 eb09                     jmp        0x51ef0c
0051ef03 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
0051ef06 83c001                   add        eax, 1
0051ef09 8945b8                   mov        dword ptr [ebp - 0x48], eax
0051ef0c 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051ef0f 8b9124590000             mov        edx, dword ptr [ecx + 0x5924]
0051ef15 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
0051ef18 3b4204                   cmp        eax, dword ptr [edx + 4]
0051ef1b 0f8d24020000             jge        0x51f145
0051ef21 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0051ef24 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0051ef27 f30f1001                 movss      xmm0, dword ptr [ecx]
0051ef2b f30f5c4204               subss      xmm0, dword ptr [edx + 4]
0051ef30 f30f1145ac               movss      dword ptr [ebp - 0x54], xmm0
0051ef35 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051ef38 f30f10802c590000         movss      xmm0, dword ptr [eax + 0x592c]
0051ef40 f30f5c45ac               subss      xmm0, dword ptr [ebp - 0x54]
0051ef45 51                       push       ecx
0051ef46 f30f110424               movss      dword ptr [esp], xmm0
0051ef4b e800f9fdff               call       0x4fe850
0051ef50 83c404                   add        esp, 4
0051ef53 d95dc4                   fstp       dword ptr [ebp - 0x3c]
0051ef56 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
0051ef5b 0f2f45b0                 comiss     xmm0, dword ptr [ebp - 0x50]
0051ef5f 760a                     jbe        0x51ef6b
0051ef61 0f57c0                   xorps      xmm0, xmm0
0051ef64 f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
0051ef69 eb14                     jmp        0x51ef7f
0051ef6b f30f1045b0               movss      xmm0, dword ptr [ebp - 0x50]
0051ef70 f30f5c45c4               subss      xmm0, dword ptr [ebp - 0x3c]
0051ef75 f30f5e45b0               divss      xmm0, dword ptr [ebp - 0x50]
0051ef7a f30f1145c4               movss      dword ptr [ebp - 0x3c], xmm0
0051ef7f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051ef82 f30f10812c590000         movss      xmm0, dword ptr [ecx + 0x592c]
0051ef8a f30f5c45ac               subss      xmm0, dword ptr [ebp - 0x54]
0051ef8f f30f5e0560055700         divss      xmm0, dword ptr [0x570560]
0051ef97 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
0051ef9c f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0051efa4 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0051efac f30f5945b4               mulss      xmm0, dword ptr [ebp - 0x4c]
0051efb1 f30f5e0548e05600         divss      xmm0, dword ptr [0x56e048]
0051efb9 51                       push       ecx
0051efba f30f110424               movss      dword ptr [esp], xmm0
0051efbf 8d4de8                   lea        ecx, [ebp - 0x18]
0051efc2 e8593ff3ff               call       0x452f20
0051efc7 8d4de8                   lea        ecx, [ebp - 0x18]
0051efca e811a3f0ff               call       0x4292e0
0051efcf 51                       push       ecx
0051efd0 d91c24                   fstp       dword ptr [esp]
0051efd3 e848a8f1ff               call       0x439820
0051efd8 83c404                   add        esp, 4
0051efdb d95d90                   fstp       dword ptr [ebp - 0x70]
0051efde f30f104590               movss      xmm0, dword ptr [ebp - 0x70]
0051efe3 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0051efea f30f1145a4               movss      dword ptr [ebp - 0x5c], xmm0
0051efef 8d4de8                   lea        ecx, [ebp - 0x18]
0051eff2 e8e9a2f0ff               call       0x4292e0
0051eff7 51                       push       ecx
0051eff8 d91c24                   fstp       dword ptr [esp]
0051effb e8c0a7f1ff               call       0x4397c0
0051f000 83c404                   add        esp, 4
0051f003 d95d8c                   fstp       dword ptr [ebp - 0x74]
0051f006 f30f10458c               movss      xmm0, dword ptr [ebp - 0x74]
0051f00b 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0051f012 f30f1145a0               movss      dword ptr [ebp - 0x60], xmm0
0051f017 837dbc00                 cmp        dword ptr [ebp - 0x44], 0
0051f01b 7462                     je         0x51f07f
0051f01d 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0051f020 8b8224590000             mov        eax, dword ptr [edx + 0x5924]
0051f026 8b08                     mov        ecx, dword ptr [eax]
0051f028 83e901                   sub        ecx, 1
0051f02b 394dbc                   cmp        dword ptr [ebp - 0x44], ecx
0051f02e 744f                     je         0x51f07f
0051f030 837db800                 cmp        dword ptr [ebp - 0x48], 0
0051f034 7449                     je         0x51f07f
0051f036 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0051f039 8b8224590000             mov        eax, dword ptr [edx + 0x5924]
0051f03f 8b4804                   mov        ecx, dword ptr [eax + 4]
0051f042 83e901                   sub        ecx, 1
0051f045 394db8                   cmp        dword ptr [ebp - 0x48], ecx
0051f048 7435                     je         0x51f07f
0051f04a f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
0051f04f f30f5945a8               mulss      xmm0, dword ptr [ebp - 0x58]
0051f054 f30f5945c4               mulss      xmm0, dword ptr [ebp - 0x3c]
0051f059 f30f1145d8               movss      dword ptr [ebp - 0x28], xmm0
0051f05e f30f1045a0               movss      xmm0, dword ptr [ebp - 0x60]
0051f063 f30f5945a8               mulss      xmm0, dword ptr [ebp - 0x58]
0051f068 f30f5945c4               mulss      xmm0, dword ptr [ebp - 0x3c]
0051f06d f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
0051f072 8d55d8                   lea        edx, [ebp - 0x28]
0051f075 52                       push       edx
0051f076 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0051f079 e862a6f0ff               call       0x4296e0
0051f07e 90                       nop        
0051f07f f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
0051f084 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
0051f08b f30f5845a0               addss      xmm0, dword ptr [ebp - 0x60]
0051f090 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
0051f095 51                       push       ecx
0051f096 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
0051f09b f30f110424               movss      dword ptr [esp], xmm0
0051f0a0 51                       push       ecx
0051f0a1 f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
0051f0a6 f30f110424               movss      dword ptr [esp], xmm0
0051f0ab 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051f0ae 8b8838590000             mov        ecx, dword ptr [eax + 0x5938]
0051f0b4 51                       push       ecx
0051f0b5 8d4def                   lea        ecx, [ebp - 0x11]
0051f0b8 e843f0ffff               call       0x51e100
0051f0bd 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0051f0c0 884212                   mov        byte ptr [edx + 0x12], al
0051f0c3 51                       push       ecx
0051f0c4 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
0051f0c9 f30f110424               movss      dword ptr [esp], xmm0
0051f0ce 51                       push       ecx
0051f0cf f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
0051f0d4 f30f110424               movss      dword ptr [esp], xmm0
0051f0d9 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051f0dc 8b8834590000             mov        ecx, dword ptr [eax + 0x5934]
0051f0e2 51                       push       ecx
0051f0e3 8d4def                   lea        ecx, [ebp - 0x11]
0051f0e6 e815f0ffff               call       0x51e100
0051f0eb 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0051f0ee 884211                   mov        byte ptr [edx + 0x11], al
0051f0f1 51                       push       ecx
0051f0f2 f30f1045b4               movss      xmm0, dword ptr [ebp - 0x4c]
0051f0f7 f30f110424               movss      dword ptr [esp], xmm0
0051f0fc 51                       push       ecx
0051f0fd f30f1045c4               movss      xmm0, dword ptr [ebp - 0x3c]
0051f102 f30f110424               movss      dword ptr [esp], xmm0
0051f107 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051f10a 8b8830590000             mov        ecx, dword ptr [eax + 0x5930]
0051f110 51                       push       ecx
0051f111 8d4def                   lea        ecx, [ebp - 0x11]
0051f114 e8e7efffff               call       0x51e100
0051f119 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0051f11c 884210                   mov        byte ptr [edx + 0x10], al
0051f11f 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
0051f122 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051f125 8a913c590000             mov        dl, byte ptr [ecx + 0x593c]
0051f12b 885013                   mov        byte ptr [eax + 0x13], dl
0051f12e 8b459c                   mov        eax, dword ptr [ebp - 0x64]
0051f131 83c00c                   add        eax, 0xc
0051f134 89459c                   mov        dword ptr [ebp - 0x64], eax
0051f137 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0051f13a 83c11c                   add        ecx, 0x1c
0051f13d 894dc0                   mov        dword ptr [ebp - 0x40], ecx
0051f140 e9befdffff               jmp        0x51ef03
0051f145 e993fdffff               jmp        0x51eedd
0051f14a 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0051f14d f30f10822c590000         movss      xmm0, dword ptr [edx + 0x592c]
0051f155 f30f5805bcd75600         addss      xmm0, dword ptr [0x56d7bc]
0051f15d 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051f160 f30f11802c590000         movss      dword ptr [eax + 0x592c], xmm0
0051f168 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051f16b f30f10812c590000         movss      xmm0, dword ptr [ecx + 0x592c]
0051f173 0f2f0598565700           comiss     xmm0, dword ptr [0x575698]
0051f17a 766c                     jbe        0x51f1e8
0051f17c b9c4a45b00               mov        ecx, 0x5ba4c4
0051f181 e85a4df0ff               call       0x423ee0
0051f186 33d2                     xor        edx, edx
0051f188 b9e8030000               mov        ecx, 0x3e8
0051f18d f7f1                     div        ecx
0051f18f 895598                   mov        dword ptr [ebp - 0x68], edx
0051f192 f20f2a4598               cvtsi2sd   xmm0, dword ptr [ebp - 0x68]
0051f197 8b5598                   mov        edx, dword ptr [ebp - 0x68]
0051f19a c1ea1f                   shr        edx, 0x1f
0051f19d f20f5804d5d0cd5600       addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
0051f1a6 f20f11857cffffff         movsd      qword ptr [ebp - 0x84], xmm0
0051f1ae f20f5a857cffffff         cvtsd2ss   xmm0, qword ptr [ebp - 0x84]
0051f1b6 f30f58059c565700         addss      xmm0, dword ptr [0x57569c]
0051f1be f30f114584               movss      dword ptr [ebp - 0x7c], xmm0
0051f1c3 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0051f1c6 f30f10802c590000         movss      xmm0, dword ptr [eax + 0x592c]
0051f1ce f30f114588               movss      dword ptr [ebp - 0x78], xmm0
0051f1d3 f30f104588               movss      xmm0, dword ptr [ebp - 0x78]
0051f1d8 f30f5c4584               subss      xmm0, dword ptr [ebp - 0x7c]
0051f1dd 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0051f1e0 f30f11812c590000         movss      dword ptr [ecx + 0x592c], xmm0
0051f1e8 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0051f1eb 8b8a24590000             mov        ecx, dword ptr [edx + 0x5924]
0051f1f1 e8caebf7ff               call       0x49ddc0
0051f1f6 90                       nop        
0051f1f7 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051f1fe 8d4de4                   lea        ecx, [ebp - 0x1c]
0051f201 e8fac6eeff               call       0x40b900
0051f206 90                       nop        
0051f207 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051f20a 64890d00000000           mov        dword ptr fs:[0], ecx
0051f211 59                       pop        ecx
0051f212 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051f215 33cd                     xor        ecx, ebp
0051f217 e8d2370200               call       0x5429ee
0051f21c 8be5                     mov        esp, ebp
0051f21e 5d                       pop        ebp
0051f21f c3                       ret        
