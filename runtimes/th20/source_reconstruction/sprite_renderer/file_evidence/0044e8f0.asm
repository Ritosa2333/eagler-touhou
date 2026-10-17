0044e8f0 55                       push       ebp
0044e8f1 8bec                     mov        ebp, esp
0044e8f3 83ec50                   sub        esp, 0x50
0044e8f6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044e8fb 33c5                     xor        eax, ebp
0044e8fd 8945fc                   mov        dword ptr [ebp - 4], eax
0044e900 894db0                   mov        dword ptr [ebp - 0x50], ecx
0044e903 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044e906 50                       push       eax
0044e907 e8a4e6ffff               call       0x44cfb0
0044e90c 83c404                   add        esp, 4
0044e90f 89450c                   mov        dword ptr [ebp + 0xc], eax
0044e912 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e915 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0044e918 83e2fc                   and        edx, 0xfffffffc
0044e91b 8b4508                   mov        eax, dword ptr [ebp + 8]
0044e91e 895014                   mov        dword ptr [eax + 0x14], edx
0044e921 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0044e924 3b4d24                   cmp        ecx, dword ptr [ebp + 0x24]
0044e927 7e08                     jle        0x44e931
0044e929 8b5524                   mov        edx, dword ptr [ebp + 0x24]
0044e92c 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0044e92f eb06                     jmp        0x44e937
0044e931 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044e934 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0044e937 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0044e93a 894dc8                   mov        dword ptr [ebp - 0x38], ecx
0044e93d 8b5518                   mov        edx, dword ptr [ebp + 0x18]
0044e940 3b5528                   cmp        edx, dword ptr [ebp + 0x28]
0044e943 7e08                     jle        0x44e94d
0044e945 8b4528                   mov        eax, dword ptr [ebp + 0x28]
0044e948 8945d0                   mov        dword ptr [ebp - 0x30], eax
0044e94b eb06                     jmp        0x44e953
0044e94d 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
0044e950 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0044e953 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
0044e956 8955c4                   mov        dword ptr [ebp - 0x3c], edx
0044e959 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
0044e95c 8945ec                   mov        dword ptr [ebp - 0x14], eax
0044e95f 8b4d20                   mov        ecx, dword ptr [ebp + 0x20]
0044e962 894df0                   mov        dword ptr [ebp - 0x10], ecx
0044e965 8b551c                   mov        edx, dword ptr [ebp + 0x1c]
0044e968 0355c8                   add        edx, dword ptr [ebp - 0x38]
0044e96b 8955f4                   mov        dword ptr [ebp - 0xc], edx
0044e96e 8b4520                   mov        eax, dword ptr [ebp + 0x20]
0044e971 0345c4                   add        eax, dword ptr [ebp - 0x3c]
0044e974 8945f8                   mov        dword ptr [ebp - 8], eax
0044e977 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044e97a 8b5110                   mov        edx, dword ptr [ecx + 0x10]
0044e97d 0fb64222                 movzx      eax, byte ptr [edx + 0x22]
0044e981 85c0                     test       eax, eax
0044e983 7449                     je         0x44e9ce
0044e985 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0044e98d 0f2f0518885b00           comiss     xmm0, dword ptr [0x5b8818]
0044e994 7638                     jbe        0x44e9ce
0044e996 f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
0044e99b f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
0044e9a3 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
0044e9ab f30f2cc8                 cvttss2si  ecx, xmm0
0044e9af 894d14                   mov        dword ptr [ebp + 0x14], ecx
0044e9b2 f30f2a4518               cvtsi2ss   xmm0, dword ptr [ebp + 0x18]
0044e9b7 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
0044e9bf f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
0044e9c7 f30f2cd0                 cvttss2si  edx, xmm0
0044e9cb 895518                   mov        dword ptr [ebp + 0x18], edx
0044e9ce c745dc00000000           mov        dword ptr [ebp - 0x24], 0
0044e9d5 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0044e9dc 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044e9df 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0044e9e2 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
0044e9e5 894de8                   mov        dword ptr [ebp - 0x18], ecx
0044e9e8 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044e9eb 8b049508e25600           mov        eax, dword ptr [edx*4 + 0x56e208]
0044e9f2 8945c0                   mov        dword ptr [ebp - 0x40], eax
0044e9f5 b9404d5c00               mov        ecx, 0x5c4d40
0044e9fa e8313dfcff               call       0x412730
0044e9ff 8945bc                   mov        dword ptr [ebp - 0x44], eax
0044ea02 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044ea05 51                       push       ecx
0044ea06 6a01                     push       1
0044ea08 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0044ea0b 52                       push       edx
0044ea0c 6a00                     push       0
0044ea0e 6a01                     push       1
0044ea10 8b4518                   mov        eax, dword ptr [ebp + 0x18]
0044ea13 50                       push       eax
0044ea14 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0044ea17 51                       push       ecx
0044ea18 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0044ea1b 52                       push       edx
0044ea1c e8811c0f00               call       0x5406a2
0044ea21 c745d800000000           mov        dword ptr [ebp - 0x28], 0
0044ea28 8b4508                   mov        eax, dword ptr [ebp + 8]
0044ea2b 8b08                     mov        ecx, dword ptr [eax]
0044ea2d 894db8                   mov        dword ptr [ebp - 0x48], ecx
0044ea30 8b5508                   mov        edx, dword ptr [ebp + 8]
0044ea33 8b02                     mov        eax, dword ptr [edx]
0044ea35 8b08                     mov        ecx, dword ptr [eax]
0044ea37 8b5148                   mov        edx, dword ptr [ecx + 0x48]
0044ea3a 8955b4                   mov        dword ptr [ebp - 0x4c], edx
0044ea3d 8d45d8                   lea        eax, [ebp - 0x28]
0044ea40 50                       push       eax
0044ea41 6a00                     push       0
0044ea43 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0044ea46 51                       push       ecx
0044ea47 ff55b4                   call       dword ptr [ebp - 0x4c]
0044ea4a 90                       nop        
0044ea4b 8b5508                   mov        edx, dword ptr [ebp + 8]
0044ea4e 8b4210                   mov        eax, dword ptr [edx + 0x10]
0044ea51 0fb64822                 movzx      ecx, byte ptr [eax + 0x22]
0044ea55 85c9                     test       ecx, ecx
0044ea57 741a                     je         0x44ea73
0044ea59 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0044ea61 0f2f0518885b00           comiss     xmm0, dword ptr [0x5b8818]
0044ea68 7609                     jbe        0x44ea73
0044ea6a c745cc04000300           mov        dword ptr [ebp - 0x34], 0x30004
0044ea71 eb07                     jmp        0x44ea7a
0044ea73 c745cc01000000           mov        dword ptr [ebp - 0x34], 1
0044ea7a 6a00                     push       0
0044ea7c 6a00                     push       0
0044ea7e 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0044ea81 52                       push       edx
0044ea82 8d45ec                   lea        eax, [ebp - 0x14]
0044ea85 50                       push       eax
0044ea86 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044ea89 8b5108                   mov        edx, dword ptr [ecx + 8]
0044ea8c 52                       push       edx
0044ea8d 8b4508                   mov        eax, dword ptr [ebp + 8]
0044ea90 8b4804                   mov        ecx, dword ptr [eax + 4]
0044ea93 51                       push       ecx
0044ea94 8d55dc                   lea        edx, [ebp - 0x24]
0044ea97 52                       push       edx
0044ea98 6a00                     push       0
0044ea9a 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0044ea9d 50                       push       eax
0044ea9e e8f31b0f00               call       0x540696
0044eaa3 85c0                     test       eax, eax
0044eaa5 7420                     je         0x44eac7
0044eaa7 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0044eaab 7415                     je         0x44eac2
0044eaad 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0044eab0 8b11                     mov        edx, dword ptr [ecx]
0044eab2 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0044eab5 50                       push       eax
0044eab6 8b4a08                   mov        ecx, dword ptr [edx + 8]
0044eab9 ffd1                     call       ecx
0044eabb c745d800000000           mov        dword ptr [ebp - 0x28], 0
0044eac2 83c8ff                   or         eax, 0xffffffff
0044eac5 eb4b                     jmp        0x44eb12
0044eac7 837dd800                 cmp        dword ptr [ebp - 0x28], 0
0044eacb 7415                     je         0x44eae2
0044eacd 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0044ead0 8b02                     mov        eax, dword ptr [edx]
0044ead2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0044ead5 51                       push       ecx
0044ead6 8b5008                   mov        edx, dword ptr [eax + 8]
0044ead9 ffd2                     call       edx
0044eadb c745d800000000           mov        dword ptr [ebp - 0x28], 0
0044eae2 8b4508                   mov        eax, dword ptr [ebp + 8]
0044eae5 8b08                     mov        ecx, dword ptr [eax]
0044eae7 51                       push       ecx
0044eae8 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0044eaeb e8202e0000               call       0x451910
0044eaf0 8b5508                   mov        edx, dword ptr [ebp + 8]
0044eaf3 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044eaf6 8b0c852ce25600           mov        ecx, dword ptr [eax*4 + 0x56e22c]
0044eafd 894a0c                   mov        dword ptr [edx + 0xc], ecx
0044eb00 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044eb03 0faf4518                 imul       eax, dword ptr [ebp + 0x18]
0044eb07 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044eb0a 0faf04952ce25600         imul       eax, dword ptr [edx*4 + 0x56e22c]
0044eb12 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044eb15 33cd                     xor        ecx, ebp
0044eb17 e8d23e0f00               call       0x5429ee
0044eb1c 8be5                     mov        esp, ebp
0044eb1e 5d                       pop        ebp
0044eb1f c22400                   ret        0x24
0044eb22 cc                       int3       
0044eb23 cc                       int3       
0044eb24 cc                       int3       
0044eb25 cc                       int3       
0044eb26 cc                       int3       
0044eb27 cc                       int3       
0044eb28 cc                       int3       
0044eb29 cc                       int3       
0044eb2a cc                       int3       
0044eb2b cc                       int3       
0044eb2c cc                       int3       
0044eb2d cc                       int3       
0044eb2e cc                       int3       
0044eb2f cc                       int3       
