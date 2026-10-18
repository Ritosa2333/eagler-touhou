004840e0 55                       push       ebp
004840e1 8bec                     mov        ebp, esp
004840e3 83ec28                   sub        esp, 0x28
004840e6 894dfc                   mov        dword ptr [ebp - 4], ecx
004840e9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004840ec 05a0000000               add        eax, 0xa0
004840f1 8945f8                   mov        dword ptr [ebp - 8], eax
004840f4 6a00                     push       0
004840f6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004840f9 e88285ffff               call       0x47c680
004840fe 8945f4                   mov        dword ptr [ebp - 0xc], eax
00484101 6a10                     push       0x10
00484103 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00484106 e885f4f9ff               call       0x423590
0048410b 0fb6c8                   movzx      ecx, al
0048410e 85c9                     test       ecx, ecx
00484110 0f8490000000             je         0x4841a6
00484116 8b55fc                   mov        edx, dword ptr [ebp - 4]
00484119 81c2a0000000             add        edx, 0xa0
0048411f 8955f0                   mov        dword ptr [ebp - 0x10], edx
00484122 6a00                     push       0
00484124 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00484127 e85485ffff               call       0x47c680
0048412c 8bc8                     mov        ecx, eax
0048412e e89dfaf9ff               call       0x423bd0
00484133 d95dec                   fstp       dword ptr [ebp - 0x14]
00484136 f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
0048413b f30f5905d4ed5600         mulss      xmm0, dword ptr [0x56edd4]
00484143 f30f5e0590cd5600         divss      xmm0, dword ptr [0x56cd90]
0048414b f30f100dd4ed5600         movss      xmm1, dword ptr [0x56edd4]
00484153 f30f5cc8                 subss      xmm1, xmm0
00484157 f30f114de8               movss      dword ptr [ebp - 0x18], xmm1
0048415c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048415f f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00484164 f30f584020               addss      xmm0, dword ptr [eax + 0x20]
00484169 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
0048416e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484171 81c184000000             add        ecx, 0x84
00484177 e86451faff               call       0x4292e0
0048417c d95de0                   fstp       dword ptr [ebp - 0x20]
0048417f 51                       push       ecx
00484180 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00484185 f30f110424               movss      dword ptr [esp], xmm0
0048418a 51                       push       ecx
0048418b f30f1045e0               movss      xmm0, dword ptr [ebp - 0x20]
00484190 f30f110424               movss      dword ptr [esp], xmm0
00484195 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484198 83c170                   add        ecx, 0x70
0048419b 51                       push       ecx
0048419c e88f51fbff               call       0x439330
004841a1 83c40c                   add        esp, 0xc
004841a4 eb1c                     jmp        0x4841c2
004841a6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004841a9 8b8290000000             mov        eax, dword ptr [edx + 0x90]
004841af 83e0fe                   and        eax, 0xfffffffe
004841b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004841b5 898190000000             mov        dword ptr [ecx + 0x90], eax
004841bb b801000000               mov        eax, 1
004841c0 eb25                     jmp        0x4841e7
004841c2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004841c5 81c2a0000000             add        edx, 0xa0
004841cb 8955dc                   mov        dword ptr [ebp - 0x24], edx
004841ce 6a00                     push       0
004841d0 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004841d3 e8a884ffff               call       0x47c680
004841d8 8945d8                   mov        dword ptr [ebp - 0x28], eax
004841db 6a00                     push       0
004841dd 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004841e0 e85bf3f9ff               call       0x423540
004841e5 33c0                     xor        eax, eax
004841e7 8be5                     mov        esp, ebp
004841e9 5d                       pop        ebp
004841ea c3                       ret        
004841eb cc                       int3       
004841ec cc                       int3       
004841ed cc                       int3       
004841ee cc                       int3       
004841ef cc                       int3       
