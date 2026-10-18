00530e80 55                       push       ebp
00530e81 8bec                     mov        ebp, esp
00530e83 83ec28                   sub        esp, 0x28
00530e86 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00530e8b 33c5                     xor        eax, ebp
00530e8d 8945fc                   mov        dword ptr [ebp - 4], eax
00530e90 894dec                   mov        dword ptr [ebp - 0x14], ecx
00530e93 8d4df0                   lea        ecx, [ebp - 0x10]
00530e96 e8751fefff               call       0x422e10
00530e9b 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530e9e 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
00530ea4 e8e7f0edff               call       0x40ff90
00530ea9 8bc8                     mov        ecx, eax
00530eab e830e7fcff               call       0x4ff5e0
00530eb0 0fb6c8                   movzx      ecx, al
00530eb3 85c9                     test       ecx, ecx
00530eb5 750f                     jne        0x530ec6
00530eb7 f30f100560055700         movss      xmm0, dword ptr [0x570560]
00530ebf f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00530ec4 eb0d                     jmp        0x530ed3
00530ec6 f30f100548fd5600         movss      xmm0, dword ptr [0x56fd48]
00530ece f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00530ed3 51                       push       ecx
00530ed4 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00530ed9 f30f110424               movss      dword ptr [esp], xmm0
00530ede 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530ee1 81c1d4000000             add        ecx, 0xd4
00530ee7 e8f483efff               call       0x4292e0
00530eec 51                       push       ecx
00530eed d91c24                   fstp       dword ptr [esp]
00530ef0 8d55f0                   lea        edx, [ebp - 0x10]
00530ef3 52                       push       edx
00530ef4 e83784f0ff               call       0x439330
00530ef9 83c40c                   add        esp, 0xc
00530efc 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530eff 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
00530f05 e886f0edff               call       0x40ff90
00530f0a 8bc8                     mov        ecx, eax
00530f0c e87fe7fcff               call       0x4ff690
00530f11 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00530f16 f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
00530f1e f30f2cc8                 cvttss2si  ecx, xmm0
00530f22 0308                     add        ecx, dword ptr [eax]
00530f24 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00530f27 894a70                   mov        dword ptr [edx + 0x70], ecx
00530f2a 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530f2d 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
00530f33 e858f0edff               call       0x40ff90
00530f38 8bc8                     mov        ecx, eax
00530f3a e851e7fcff               call       0x4ff690
00530f3f f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00530f44 f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
00530f4c f30f2cc8                 cvttss2si  ecx, xmm0
00530f50 034804                   add        ecx, dword ptr [eax + 4]
00530f53 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00530f56 894a74                   mov        dword ptr [edx + 0x74], ecx
00530f59 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530f5c 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
00530f62 e829f0edff               call       0x40ff90
00530f67 8bc8                     mov        ecx, eax
00530f69 e872e6fcff               call       0x4ff5e0
00530f6e 0fb6c8                   movzx      ecx, al
00530f71 85c9                     test       ecx, ecx
00530f73 7552                     jne        0x530fc7
00530f75 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00530f78 81c2d4000000             add        edx, 0xd4
00530f7e 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00530f81 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00530f89 f30f5e05d00a5700         divss      xmm0, dword ptr [0x570ad0]
00530f91 51                       push       ecx
00530f92 f30f110424               movss      dword ptr [esp], xmm0
00530f97 8d45dc                   lea        eax, [ebp - 0x24]
00530f9a 50                       push       eax
00530f9b 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00530f9e e8dd1ff2ff               call       0x452f80
00530fa3 8bc8                     mov        ecx, eax
00530fa5 e83683efff               call       0x4292e0
00530faa 51                       push       ecx
00530fab d91c24                   fstp       dword ptr [esp]
00530fae e88d75f0ff               call       0x438540
00530fb3 d91c24                   fstp       dword ptr [esp]
00530fb6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530fb9 81c1d4000000             add        ecx, 0xd4
00530fbf e85c1ff2ff               call       0x452f20
00530fc4 90                       nop        
00530fc5 eb50                     jmp        0x531017
00530fc7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530fca 81c1d4000000             add        ecx, 0xd4
00530fd0 894de0                   mov        dword ptr [ebp - 0x20], ecx
00530fd3 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00530fdb f30f5e0560055700         divss      xmm0, dword ptr [0x570560]
00530fe3 51                       push       ecx
00530fe4 f30f110424               movss      dword ptr [esp], xmm0
00530fe9 8d55d8                   lea        edx, [ebp - 0x28]
00530fec 52                       push       edx
00530fed 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00530ff0 e88b1ff2ff               call       0x452f80
00530ff5 8bc8                     mov        ecx, eax
00530ff7 e8e482efff               call       0x4292e0
00530ffc 51                       push       ecx
00530ffd d91c24                   fstp       dword ptr [esp]
00531000 e83b75f0ff               call       0x438540
00531005 d91c24                   fstp       dword ptr [esp]
00531008 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053100b 81c1d4000000             add        ecx, 0xd4
00531011 e80a1ff2ff               call       0x452f20
00531016 90                       nop        
00531017 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053101a 33cd                     xor        ecx, ebp
0053101c e8cd190100               call       0x5429ee
00531021 8be5                     mov        esp, ebp
00531023 5d                       pop        ebp
00531024 c3                       ret        
00531025 cc                       int3       
00531026 cc                       int3       
00531027 cc                       int3       
00531028 cc                       int3       
00531029 cc                       int3       
0053102a cc                       int3       
0053102b cc                       int3       
0053102c cc                       int3       
0053102d cc                       int3       
0053102e cc                       int3       
0053102f cc                       int3       
