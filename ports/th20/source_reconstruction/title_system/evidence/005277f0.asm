005277f0 55                       push       ebp
005277f1 8bec                     mov        ebp, esp
005277f3 81ecc4000000             sub        esp, 0xc4
005277f9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005277fe 33c5                     xor        eax, ebp
00527800 8945fc                   mov        dword ptr [ebp - 4], eax
00527803 894d80                   mov        dword ptr [ebp - 0x80], ecx
00527806 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00527809 8b4820                   mov        ecx, dword ptr [eax + 0x20]
0052780c 898d68ffffff             mov        dword ptr [ebp - 0x98], ecx
00527812 83bd68ffffff02           cmp        dword ptr [ebp - 0x98], 2
00527819 7412                     je         0x52782d
0052781b 83bd68ffffff03           cmp        dword ptr [ebp - 0x98], 3
00527822 0f844b020000             je         0x527a73
00527828 e9b1060000               jmp        0x527ede
0052782d 51                       push       ecx
0052782e 0f57c0                   xorps      xmm0, xmm0
00527831 f30f110424               movss      dword ptr [esp], xmm0
00527836 51                       push       ecx
00527837 f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
0052783f f30f110424               movss      dword ptr [esp], xmm0
00527844 51                       push       ecx
00527845 f30f100510ed5600         movss      xmm0, dword ptr [0x56ed10]
0052784d f30f110424               movss      dword ptr [esp], xmm0
00527852 8d4dd8                   lea        ecx, [ebp - 0x28]
00527855 e876b5efff               call       0x422dd0
0052785a 6a01                     push       1
0052785c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527862 e8b9f1fbff               call       0x4e6a20
00527867 90                       nop        
00527868 c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
00527872 eb0f                     jmp        0x527883
00527874 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
0052787a 83c201                   add        edx, 1
0052787d 899578ffffff             mov        dword ptr [ebp - 0x88], edx
00527883 83bd78ffffff19           cmp        dword ptr [ebp - 0x88], 0x19
0052788a 0f8dc3010000             jge        0x527a53
00527890 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00527893 83c024                   add        eax, 0x24
00527896 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
0052789c 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
005278a2 51                       push       ecx
005278a3 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
005278a9 e80239f4ff               call       0x46b1b0
005278ae 85c0                     test       eax, eax
005278b0 7413                     je         0x5278c5
005278b2 6800ffffff               push       0xffffff00
005278b7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005278bd e80e92f4ff               call       0x470ad0
005278c2 90                       nop        
005278c3 eb11                     jmp        0x5278d6
005278c5 68808080ff               push       0xff808080
005278ca 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
005278d0 e8fb91f4ff               call       0x470ad0
005278d5 90                       nop        
005278d6 8b5580                   mov        edx, dword ptr [ebp - 0x80]
005278d9 81c240570000             add        edx, 0x5740
005278df 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
005278e5 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
005278eb 50                       push       eax
005278ec 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
005278f2 e889cceeff               call       0x414580
005278f7 833800                   cmp        dword ptr [eax], 0
005278fa 0f841a010000             je         0x527a1a
00527900 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00527903 81c140570000             add        ecx, 0x5740
00527909 898d54ffffff             mov        dword ptr [ebp - 0xac], ecx
0052790f 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00527915 52                       push       edx
00527916 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
0052791c e85fcceeff               call       0x414580
00527921 8b08                     mov        ecx, dword ptr [eax]
00527923 e8e89eefff               call       0x421810
00527928 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
0052792e 33c0                     xor        eax, eax
00527930 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00527933 8945b8                   mov        dword ptr [ebp - 0x48], eax
00527936 8945bc                   mov        dword ptr [ebp - 0x44], eax
00527939 8945c0                   mov        dword ptr [ebp - 0x40], eax
0052793c 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0052793f 8945c8                   mov        dword ptr [ebp - 0x38], eax
00527942 8945cc                   mov        dword ptr [ebp - 0x34], eax
00527945 8945d0                   mov        dword ptr [ebp - 0x30], eax
00527948 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052794b 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00527951 83c110                   add        ecx, 0x10
00527954 51                       push       ecx
00527955 8d55b4                   lea        edx, [ebp - 0x4c]
00527958 52                       push       edx
00527959 e81249f8ff               call       0x4ac270
0052795e 83c408                   add        esp, 8
00527961 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
00527967 f30f5a80d0000000         cvtss2sd   xmm0, dword ptr [eax + 0xd0]
0052796f 83ec08                   sub        esp, 8
00527972 f20f110424               movsd      qword ptr [esp], xmm0
00527977 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
0052797d 8b91f4000000             mov        edx, dword ptr [ecx + 0xf4]
00527983 8b0495d0ff5a00           mov        eax, dword ptr [edx*4 + 0x5affd0]
0052798a 50                       push       eax
0052798b 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00527991 8b91f0000000             mov        edx, dword ptr [ecx + 0xf0]
00527997 8b049590ff5a00           mov        eax, dword ptr [edx*4 + 0x5aff90]
0052799e 50                       push       eax
0052799f b904000000               mov        ecx, 4
005279a4 6bd100                   imul       edx, ecx, 0
005279a7 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
005279ad 8b8c10dc000000           mov        ecx, dword ptr [eax + edx + 0xdc]
005279b4 8b148dfcff5a00           mov        edx, dword ptr [ecx*4 + 0x5afffc]
005279bb 52                       push       edx
005279bc 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
005279c2 8b88d8000000             mov        ecx, dword ptr [eax + 0xd8]
005279c8 8b148d80ff5a00           mov        edx, dword ptr [ecx*4 + 0x5aff80]
005279cf 52                       push       edx
005279d0 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
005279d3 50                       push       eax
005279d4 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005279d7 51                       push       ecx
005279d8 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
005279db 52                       push       edx
005279dc 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
005279df 83c001                   add        eax, 1
005279e2 50                       push       eax
005279e3 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
005279e6 99                       cdq        
005279e7 b964000000               mov        ecx, 0x64
005279ec f7f9                     idiv       ecx
005279ee 52                       push       edx
005279ef 8b9574ffffff             mov        edx, dword ptr [ebp - 0x8c]
005279f5 52                       push       edx
005279f6 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
005279fc 83c001                   add        eax, 1
005279ff 50                       push       eax
00527a00 68a8535700               push       0x5753a8
00527a05 8d4dd8                   lea        ecx, [ebp - 0x28]
00527a08 51                       push       ecx
00527a09 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00527a0f 52                       push       edx
00527a10 e87b4ff4ff               call       0x46c990
00527a15 83c440                   add        esp, 0x40
00527a18 eb22                     jmp        0x527a3c
00527a1a 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00527a20 83c001                   add        eax, 1
00527a23 50                       push       eax
00527a24 6880515700               push       0x575180
00527a29 8d4dd8                   lea        ecx, [ebp - 0x28]
00527a2c 51                       push       ecx
00527a2d 8b1598065c00             mov        edx, dword ptr [0x5c0698]
00527a33 52                       push       edx
00527a34 e8574ff4ff               call       0x46c990
00527a39 83c410                   add        esp, 0x10
00527a3c f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00527a41 f30f5805fcf25600         addss      xmm0, dword ptr [0x56f2fc]
00527a49 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00527a4e e921feffff               jmp        0x527874
00527a53 6aff                     push       -1
00527a55 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527a5b e87090f4ff               call       0x470ad0
00527a60 6a00                     push       0
00527a62 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527a68 e8b3effbff               call       0x4e6a20
00527a6d 90                       nop        
00527a6e e96b040000               jmp        0x527ede
00527a73 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00527a76 8b8838570000             mov        ecx, dword ptr [eax + 0x5738]
00527a7c 898d44ffffff             mov        dword ptr [ebp - 0xbc], ecx
00527a82 51                       push       ecx
00527a83 0f57c0                   xorps      xmm0, xmm0
00527a86 f30f110424               movss      dword ptr [esp], xmm0
00527a8b 51                       push       ecx
00527a8c f30f100508f15600         movss      xmm0, dword ptr [0x56f108]
00527a94 f30f110424               movss      dword ptr [esp], xmm0
00527a99 51                       push       ecx
00527a9a f30f100510ed5600         movss      xmm0, dword ptr [0x56ed10]
00527aa2 f30f110424               movss      dword ptr [esp], xmm0
00527aa7 8d4da8                   lea        ecx, [ebp - 0x58]
00527aaa e821b3efff               call       0x422dd0
00527aaf 90                       nop        
00527ab0 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00527ab3 81c254010000             add        edx, 0x154
00527ab9 899550ffffff             mov        dword ptr [ebp - 0xb0], edx
00527abf 6a0a                     push       0xa
00527ac1 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
00527ac7 e894baefff               call       0x423560
00527acc 0fb6c0                   movzx      eax, al
00527acf 85c0                     test       eax, eax
00527ad1 7466                     je         0x527b39
00527ad3 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00527ad6 6b91385700000f           imul       edx, dword ptr [ecx + 0x5738], 0xf
00527add 83c250                   add        edx, 0x50
00527ae0 f30f2ac2                 cvtsi2ss   xmm0, edx
00527ae4 f30f5c0508f15600         subss      xmm0, dword ptr [0x56f108]
00527aec 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00527aef 81c154010000             add        ecx, 0x154
00527af5 f30f118548ffffff         movss      dword ptr [ebp - 0xb8], xmm0
00527afd e8cec0efff               call       0x423bd0
00527b02 d99d4cffffff             fstp       dword ptr [ebp - 0xb4]
00527b08 f30f100534e75600         movss      xmm0, dword ptr [0x56e734]
00527b10 f30f5c854cffffff         subss      xmm0, dword ptr [ebp - 0xb4]
00527b18 f30f108d48ffffff         movss      xmm1, dword ptr [ebp - 0xb8]
00527b20 f30f59c8                 mulss      xmm1, xmm0
00527b24 f30f5e0d34e75600         divss      xmm1, dword ptr [0x56e734]
00527b2c f30f580d08f15600         addss      xmm1, dword ptr [0x56f108]
00527b34 f30f114dac               movss      dword ptr [ebp - 0x54], xmm1
00527b39 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
00527b3f e8cc9cefff               call       0x421810
00527b44 898570ffffff             mov        dword ptr [ebp - 0x90], eax
00527b4a 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
00527b50 83c010                   add        eax, 0x10
00527b53 50                       push       eax
00527b54 8d4d84                   lea        ecx, [ebp - 0x7c]
00527b57 51                       push       ecx
00527b58 e81347f8ff               call       0x4ac270
00527b5d 83c408                   add        esp, 8
00527b60 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00527b66 f30f5a82d0000000         cvtss2sd   xmm0, dword ptr [edx + 0xd0]
00527b6e 83ec08                   sub        esp, 8
00527b71 f20f110424               movsd      qword ptr [esp], xmm0
00527b76 b804000000               mov        eax, 4
00527b7b c1e003                   shl        eax, 3
00527b7e 8b88d0ff5a00             mov        ecx, dword ptr [eax + 0x5affd0]
00527b84 51                       push       ecx
00527b85 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00527b8b 8b82f0000000             mov        eax, dword ptr [edx + 0xf0]
00527b91 8b0c8590ff5a00           mov        ecx, dword ptr [eax*4 + 0x5aff90]
00527b98 51                       push       ecx
00527b99 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
00527b9f 6b82d800000009           imul       eax, dword ptr [edx + 0xd8], 9
00527ba6 b904000000               mov        ecx, 4
00527bab 6bd100                   imul       edx, ecx, 0
00527bae 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00527bb4 038411dc000000           add        eax, dword ptr [ecx + edx + 0xdc]
00527bbb 8b148580ff5a00           mov        edx, dword ptr [eax*4 + 0x5aff80]
00527bc2 52                       push       edx
00527bc3 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00527bc6 50                       push       eax
00527bc7 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00527bca 51                       push       ecx
00527bcb 8b5590                   mov        edx, dword ptr [ebp - 0x70]
00527bce 52                       push       edx
00527bcf 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
00527bd2 83c001                   add        eax, 1
00527bd5 50                       push       eax
00527bd6 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00527bd9 99                       cdq        
00527bda b964000000               mov        ecx, 0x64
00527bdf f7f9                     idiv       ecx
00527be1 52                       push       edx
00527be2 68ac245700               push       0x5724ac
00527be7 8b9544ffffff             mov        edx, dword ptr [ebp - 0xbc]
00527bed 83c201                   add        edx, 1
00527bf0 52                       push       edx
00527bf1 68a8535700               push       0x5753a8
00527bf6 8d45a8                   lea        eax, [ebp - 0x58]
00527bf9 50                       push       eax
00527bfa 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527c00 51                       push       ecx
00527c01 e88a4df4ff               call       0x46c990
00527c06 83c43c                   add        esp, 0x3c
00527c09 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00527c0c 81c254010000             add        edx, 0x154
00527c12 899540ffffff             mov        dword ptr [ebp - 0xc0], edx
00527c18 6a0a                     push       0xa
00527c1a 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
00527c20 e89bb9efff               call       0x4235c0
00527c25 0fb6c0                   movzx      eax, al
00527c28 85c0                     test       eax, eax
00527c2a 0f8493020000             je         0x527ec3
00527c30 8b0d7cff5a00             mov        ecx, dword ptr [0x5aff7c]
00527c36 51                       push       ecx
00527c37 e8e4810200               call       0x54fe20
00527c3c 83c404                   add        esp, 4
00527c3f 89856cffffff             mov        dword ptr [ebp - 0x94], eax
00527c45 51                       push       ecx
00527c46 0f57c0                   xorps      xmm0, xmm0
00527c49 f30f110424               movss      dword ptr [esp], xmm0
00527c4e 51                       push       ecx
00527c4f f30f100508f15600         movss      xmm0, dword ptr [0x56f108]
00527c57 f30f110424               movss      dword ptr [esp], xmm0
00527c5c 51                       push       ecx
00527c5d f30f100590f05600         movss      xmm0, dword ptr [0x56f090]
00527c65 f30f110424               movss      dword ptr [esp], xmm0
00527c6a 8d4df0                   lea        ecx, [ebp - 0x10]
00527c6d e85eb1efff               call       0x422dd0
00527c72 6aff                     push       -1
00527c74 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527c7a e8518ef4ff               call       0x470ad0
00527c7f 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00527c82 81c2d4560000             add        edx, 0x56d4
00527c88 52                       push       edx
00527c89 68e4e05600               push       0x56e0e4
00527c8e 8d45f0                   lea        eax, [ebp - 0x10]
00527c91 50                       push       eax
00527c92 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527c98 51                       push       ecx
00527c99 e8f24cf4ff               call       0x46c990
00527c9e 83c410                   add        esp, 0x10
00527ca1 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00527ca4 6b82e056000009           imul       eax, dword ptr [edx + 0x56e0], 9
00527cab f30f2ac0                 cvtsi2ss   xmm0, eax
00527caf f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
00527cb4 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00527cb9 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00527cbc 83b9e056000008           cmp        dword ptr [ecx + 0x56e0], 8
00527cc3 7512                     jne        0x527cd7
00527cc5 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00527cca f30f5c05a4f75600         subss      xmm0, dword ptr [0x56f7a4]
00527cd2 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00527cd7 6800ffffff               push       0xffffff00
00527cdc 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527ce2 e8e98df4ff               call       0x470ad0
00527ce7 6824255700               push       0x572524
00527cec 8d55f0                   lea        edx, [ebp - 0x10]
00527cef 52                       push       edx
00527cf0 a198065c00               mov        eax, dword ptr [0x5c0698]
00527cf5 50                       push       eax
00527cf6 e8954cf4ff               call       0x46c990
00527cfb 83c40c                   add        esp, 0xc
00527cfe 6aff                     push       -1
00527d00 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527d06 e8c58df4ff               call       0x470ad0
00527d0b f30f10057cfb5600         movss      xmm0, dword ptr [0x56fb7c]
00527d13 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00527d18 51                       push       ecx
00527d19 0f57c0                   xorps      xmm0, xmm0
00527d1c f30f110424               movss      dword ptr [esp], xmm0
00527d21 51                       push       ecx
00527d22 f30f100588fe5600         movss      xmm0, dword ptr [0x56fe88]
00527d2a f30f110424               movss      dword ptr [esp], xmm0
00527d2f 51                       push       ecx
00527d30 f30f100574565700         movss      xmm0, dword ptr [0x575674]
00527d38 f30f110424               movss      dword ptr [esp], xmm0
00527d3d 8d4de4                   lea        ecx, [ebp - 0x1c]
00527d40 e88bb0efff               call       0x422dd0
00527d45 90                       nop        
00527d46 c7857cffffff00000000     mov        dword ptr [ebp - 0x84], 0
00527d50 eb0f                     jmp        0x527d61
00527d52 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00527d58 83c101                   add        ecx, 1
00527d5b 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
00527d61 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
00527d67 3b956cffffff             cmp        edx, dword ptr [ebp - 0x94]
00527d6d 0f8d42010000             jge        0x527eb5
00527d73 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00527d76 05e8560000               add        eax, 0x56e8
00527d7b 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
00527d81 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
00527d87 51                       push       ecx
00527d88 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
00527d8e e81d34f4ff               call       0x46b1b0
00527d93 85c0                     test       eax, eax
00527d95 7413                     je         0x527daa
00527d97 6800ffffff               push       0xffffff00
00527d9c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527da2 e8298df4ff               call       0x470ad0
00527da7 90                       nop        
00527da8 eb11                     jmp        0x527dbb
00527daa 68808080ff               push       0xff808080
00527daf 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527db5 e8168df4ff               call       0x470ad0
00527dba 90                       nop        
00527dbb 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00527dc1 83ea03                   sub        edx, 3
00527dc4 39957cffffff             cmp        dword ptr [ebp - 0x84], edx
00527dca 7d28                     jge        0x527df4
00527dcc a17cff5a00               mov        eax, dword ptr [0x5aff7c]
00527dd1 03857cffffff             add        eax, dword ptr [ebp - 0x84]
00527dd7 0fbe08                   movsx      ecx, byte ptr [eax]
00527dda 51                       push       ecx
00527ddb 68a4535700               push       0x5753a4
00527de0 8d55e4                   lea        edx, [ebp - 0x1c]
00527de3 52                       push       edx
00527de4 a198065c00               mov        eax, dword ptr [0x5c0698]
00527de9 50                       push       eax
00527dea e8a14bf4ff               call       0x46c990
00527def 83c410                   add        esp, 0x10
00527df2 eb6e                     jmp        0x527e62
00527df4 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00527dfa 83e903                   sub        ecx, 3
00527dfd 398d7cffffff             cmp        dword ptr [ebp - 0x84], ecx
00527e03 750c                     jne        0x527e11
00527e05 c78560ffffff81000000     mov        dword ptr [ebp - 0xa0], 0x81
00527e0f eb33                     jmp        0x527e44
00527e11 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00527e17 83ea02                   sub        edx, 2
00527e1a 39957cffffff             cmp        dword ptr [ebp - 0x84], edx
00527e20 750c                     jne        0x527e2e
00527e22 c78564ffffff7f000000     mov        dword ptr [ebp - 0x9c], 0x7f
00527e2c eb0a                     jmp        0x527e38
00527e2e c78564ffffff80000000     mov        dword ptr [ebp - 0x9c], 0x80
00527e38 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
00527e3e 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
00527e44 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
00527e4a 51                       push       ecx
00527e4b 68a4535700               push       0x5753a4
00527e50 8d55e4                   lea        edx, [ebp - 0x1c]
00527e53 52                       push       edx
00527e54 a198065c00               mov        eax, dword ptr [0x5c0698]
00527e59 50                       push       eax
00527e5a e8314bf4ff               call       0x46c990
00527e5f 83c410                   add        esp, 0x10
00527e62 8b857cffffff             mov        eax, dword ptr [ebp - 0x84]
00527e68 99                       cdq        
00527e69 b90d000000               mov        ecx, 0xd
00527e6e f7f9                     idiv       ecx
00527e70 83fa0c                   cmp        edx, 0xc
00527e73 7529                     jne        0x527e9e
00527e75 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00527e7d f30f5c0558265700         subss      xmm0, dword ptr [0x572658]
00527e85 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00527e8a f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00527e8f f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
00527e97 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00527e9c eb12                     jmp        0x527eb0
00527e9e f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00527ea3 f30f580544265700         addss      xmm0, dword ptr [0x572644]
00527eab f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
00527eb0 e99dfeffff               jmp        0x527d52
00527eb5 6aff                     push       -1
00527eb7 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527ebd e80e8cf4ff               call       0x470ad0
00527ec2 90                       nop        
00527ec3 6aff                     push       -1
00527ec5 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527ecb e8008cf4ff               call       0x470ad0
00527ed0 6a00                     push       0
00527ed2 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00527ed8 e843ebfbff               call       0x4e6a20
00527edd 90                       nop        
00527ede b801000000               mov        eax, 1
00527ee3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00527ee6 33cd                     xor        ecx, ebp
00527ee8 e801ab0100               call       0x5429ee
00527eed 8be5                     mov        esp, ebp
00527eef 5d                       pop        ebp
00527ef0 c3                       ret        
00527ef1 cc                       int3       
00527ef2 cc                       int3       
00527ef3 cc                       int3       
00527ef4 cc                       int3       
00527ef5 cc                       int3       
00527ef6 cc                       int3       
00527ef7 cc                       int3       
00527ef8 cc                       int3       
00527ef9 cc                       int3       
00527efa cc                       int3       
00527efb cc                       int3       
00527efc cc                       int3       
00527efd cc                       int3       
00527efe cc                       int3       
00527eff cc                       int3       
