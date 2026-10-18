0044d9d0 55                       push       ebp
0044d9d1 8bec                     mov        ebp, esp
0044d9d3 83ec60                   sub        esp, 0x60
0044d9d6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044d9db 33c5                     xor        eax, ebp
0044d9dd 8945fc                   mov        dword ptr [ebp - 4], eax
0044d9e0 894db0                   mov        dword ptr [ebp - 0x50], ecx
0044d9e3 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
0044d9ea 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0044d9ed 50                       push       eax
0044d9ee e8bdf5ffff               call       0x44cfb0
0044d9f3 83c404                   add        esp, 4
0044d9f6 894510                   mov        dword ptr [ebp + 0x10], eax
0044d9f9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044d9fc 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0044d9ff 83e2fc                   and        edx, 0xfffffffc
0044da02 8b4508                   mov        eax, dword ptr [ebp + 8]
0044da05 895014                   mov        dword ptr [eax + 0x14], edx
0044da08 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0044da0b 894dc8                   mov        dword ptr [ebp - 0x38], ecx
0044da0e c745a000000000           mov        dword ptr [ebp - 0x60], 0
0044da15 c745a400000000           mov        dword ptr [ebp - 0x5c], 0
0044da1c 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0044da1f 0fbf4208                 movsx      eax, word ptr [edx + 8]
0044da23 8945a8                   mov        dword ptr [ebp - 0x58], eax
0044da26 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0044da29 0fbf510a                 movsx      edx, word ptr [ecx + 0xa]
0044da2d 8955ac                   mov        dword ptr [ebp - 0x54], edx
0044da30 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0044da37 c745f000000000           mov        dword ptr [ebp - 0x10], 0
0044da3e 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0044da41 0fbf4808                 movsx      ecx, word ptr [eax + 8]
0044da45 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044da48 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0044da4b 0fbf420a                 movsx      eax, word ptr [edx + 0xa]
0044da4f 8945f8                   mov        dword ptr [ebp - 8], eax
0044da52 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044da55 8b5110                   mov        edx, dword ptr [ecx + 0x10]
0044da58 0fb64222                 movzx      eax, byte ptr [edx + 0x22]
0044da5c 85c0                     test       eax, eax
0044da5e 0f8481000000             je         0x44dae5
0044da64 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0044da6c 0f2f0518885b00           comiss     xmm0, dword ptr [0x5b8818]
0044da73 7670                     jbe        0x44dae5
0044da75 f30f2a45f4               cvtsi2ss   xmm0, dword ptr [ebp - 0xc]
0044da7a f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
0044da82 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
0044da8a f30f2cc8                 cvttss2si  ecx, xmm0
0044da8e 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044da91 f30f2a45f8               cvtsi2ss   xmm0, dword ptr [ebp - 8]
0044da96 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
0044da9e f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
0044daa6 f30f2cd0                 cvttss2si  edx, xmm0
0044daaa 8955f8                   mov        dword ptr [ebp - 8], edx
0044daad f30f2a4514               cvtsi2ss   xmm0, dword ptr [ebp + 0x14]
0044dab2 f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
0044daba f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
0044dac2 f30f2cc0                 cvttss2si  eax, xmm0
0044dac6 894514                   mov        dword ptr [ebp + 0x14], eax
0044dac9 f30f2a4518               cvtsi2ss   xmm0, dword ptr [ebp + 0x18]
0044dace f30f590518885b00         mulss      xmm0, dword ptr [0x5b8818]
0044dad6 f30f5905ece05600         mulss      xmm0, dword ptr [0x56e0ec]
0044dade f30f2cc8                 cvttss2si  ecx, xmm0
0044dae2 894d18                   mov        dword ptr [ebp + 0x18], ecx
0044dae5 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0044dae8 8b049508e25600           mov        eax, dword ptr [edx*4 + 0x56e208]
0044daef 8945c0                   mov        dword ptr [ebp - 0x40], eax
0044daf2 b9404d5c00               mov        ecx, 0x5c4d40
0044daf7 e8344cfcff               call       0x412730
0044dafc 8945bc                   mov        dword ptr [ebp - 0x44], eax
0044daff 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044db02 51                       push       ecx
0044db03 6a01                     push       1
0044db05 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0044db08 52                       push       edx
0044db09 6a00                     push       0
0044db0b 6a01                     push       1
0044db0d 8b4518                   mov        eax, dword ptr [ebp + 0x18]
0044db10 50                       push       eax
0044db11 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0044db14 51                       push       ecx
0044db15 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0044db18 52                       push       edx
0044db19 e8842b0f00               call       0x5406a2
0044db1e 85c0                     test       eax, eax
0044db20 740a                     je         0x44db2c
0044db22 e9be000000               jmp        0x44dbe5
0044db27 e9b9000000               jmp        0x44dbe5
0044db2c 8b4508                   mov        eax, dword ptr [ebp + 8]
0044db2f 8b08                     mov        ecx, dword ptr [eax]
0044db31 894db8                   mov        dword ptr [ebp - 0x48], ecx
0044db34 8b5508                   mov        edx, dword ptr [ebp + 8]
0044db37 8b02                     mov        eax, dword ptr [edx]
0044db39 8b08                     mov        ecx, dword ptr [eax]
0044db3b 8b5148                   mov        edx, dword ptr [ecx + 0x48]
0044db3e 8955b4                   mov        dword ptr [ebp - 0x4c], edx
0044db41 8d45cc                   lea        eax, [ebp - 0x34]
0044db44 50                       push       eax
0044db45 6a00                     push       0
0044db47 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
0044db4a 51                       push       ecx
0044db4b ff55b4                   call       dword ptr [ebp - 0x4c]
0044db4e 8b5508                   mov        edx, dword ptr [ebp + 8]
0044db51 8b4210                   mov        eax, dword ptr [edx + 0x10]
0044db54 0fb64822                 movzx      ecx, byte ptr [eax + 0x22]
0044db58 85c9                     test       ecx, ecx
0044db5a 741a                     je         0x44db76
0044db5c f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
0044db64 0f2f0518885b00           comiss     xmm0, dword ptr [0x5b8818]
0044db6b 7609                     jbe        0x44db76
0044db6d c745c404000300           mov        dword ptr [ebp - 0x3c], 0x30004
0044db74 eb07                     jmp        0x44db7d
0044db76 c745c401000000           mov        dword ptr [ebp - 0x3c], 1
0044db7d 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044db80 83c210                   add        edx, 0x10
0044db83 8d45d0                   lea        eax, [ebp - 0x30]
0044db86 50                       push       eax
0044db87 6a00                     push       0
0044db89 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0044db8c 51                       push       ecx
0044db8d 6a00                     push       0
0044db8f 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0044db92 8b480c                   mov        ecx, dword ptr [eax + 0xc]
0044db95 51                       push       ecx
0044db96 52                       push       edx
0044db97 8d55ec                   lea        edx, [ebp - 0x14]
0044db9a 52                       push       edx
0044db9b 6a00                     push       0
0044db9d 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0044dba0 50                       push       eax
0044dba1 e8f02a0f00               call       0x540696
0044dba6 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044dba9 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0044dbac 8b04952ce25600           mov        eax, dword ptr [edx*4 + 0x56e22c]
0044dbb3 89410c                   mov        dword ptr [ecx + 0xc], eax
0044dbb6 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
0044dbba 7415                     je         0x44dbd1
0044dbbc 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0044dbbf 8b11                     mov        edx, dword ptr [ecx]
0044dbc1 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0044dbc4 50                       push       eax
0044dbc5 8b4a08                   mov        ecx, dword ptr [edx + 8]
0044dbc8 ffd1                     call       ecx
0044dbca c745cc00000000           mov        dword ptr [ebp - 0x34], 0
0044dbd1 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044dbd4 0faf4518                 imul       eax, dword ptr [ebp + 0x18]
0044dbd8 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0044dbdb 0faf04952ce25600         imul       eax, dword ptr [edx*4 + 0x56e22c]
0044dbe3 eb1e                     jmp        0x44dc03
0044dbe5 837dcc00                 cmp        dword ptr [ebp - 0x34], 0
0044dbe9 7415                     je         0x44dc00
0044dbeb 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
0044dbee 8b08                     mov        ecx, dword ptr [eax]
0044dbf0 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
0044dbf3 52                       push       edx
0044dbf4 8b4108                   mov        eax, dword ptr [ecx + 8]
0044dbf7 ffd0                     call       eax
0044dbf9 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
0044dc00 83c8ff                   or         eax, 0xffffffff
0044dc03 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044dc06 33cd                     xor        ecx, ebp
0044dc08 e8e14d0f00               call       0x5429ee
0044dc0d 8be5                     mov        esp, ebp
0044dc0f 5d                       pop        ebp
0044dc10 c21400                   ret        0x14
0044dc13 cc                       int3       
0044dc14 cc                       int3       
0044dc15 cc                       int3       
0044dc16 cc                       int3       
0044dc17 cc                       int3       
0044dc18 cc                       int3       
0044dc19 cc                       int3       
0044dc1a cc                       int3       
0044dc1b cc                       int3       
0044dc1c cc                       int3       
0044dc1d cc                       int3       
0044dc1e cc                       int3       
0044dc1f cc                       int3       
