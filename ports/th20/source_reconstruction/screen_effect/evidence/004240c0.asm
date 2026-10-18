004240c0 55                       push       ebp
004240c1 8bec                     mov        ebp, esp
004240c3 83ec0c                   sub        esp, 0xc
004240c6 833d18a55b0000           cmp        dword ptr [0x5ba518], 0
004240cd 740a                     je         0x4240d9
004240cf b807000000               mov        eax, 7
004240d4 e997000000               jmp        0x424170
004240d9 8b4508                   mov        eax, dword ptr [ebp + 8]
004240dc 83781c00                 cmp        dword ptr [eax + 0x1c], 0
004240e0 7450                     je         0x424132
004240e2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004240e5 83c130                   add        ecx, 0x30
004240e8 e8e3faffff               call       0x423bd0
004240ed d95dfc                   fstp       dword ptr [ebp - 4]
004240f0 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004240f5 f30f59059ccd5600         mulss      xmm0, dword ptr [0x56cd9c]
004240fd 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424100 f30f2a491c               cvtsi2ss   xmm1, dword ptr [ecx + 0x1c]
00424105 f30f5ec1                 divss      xmm0, xmm1
00424109 f30f100d9ccd5600         movss      xmm1, dword ptr [0x56cd9c]
00424111 f30f5cc8                 subss      xmm1, xmm0
00424115 f30f2cd1                 cvttss2si  edx, xmm1
00424119 8b4508                   mov        eax, dword ptr [ebp + 8]
0042411c 895018                   mov        dword ptr [eax + 0x18], edx
0042411f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424122 83791800                 cmp        dword ptr [ecx + 0x18], 0
00424126 7d0a                     jge        0x424132
00424128 8b5508                   mov        edx, dword ptr [ebp + 8]
0042412b c7421800000000           mov        dword ptr [edx + 0x18], 0
00424132 8b4508                   mov        eax, dword ptr [ebp + 8]
00424135 83c030                   add        eax, 0x30
00424138 8945f8                   mov        dword ptr [ebp - 8], eax
0042413b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042413e 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
00424141 52                       push       edx
00424142 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00424145 e876f4ffff               call       0x4235c0
0042414a 0fb6c0                   movzx      eax, al
0042414d 85c0                     test       eax, eax
0042414f 7407                     je         0x424158
00424151 b807000000               mov        eax, 7
00424156 eb18                     jmp        0x424170
00424158 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0042415b 83c130                   add        ecx, 0x30
0042415e 894df4                   mov        dword ptr [ebp - 0xc], ecx
00424161 6a00                     push       0
00424163 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00424166 e8d5f3ffff               call       0x423540
0042416b b801000000               mov        eax, 1
00424170 8be5                     mov        esp, ebp
00424172 5d                       pop        ebp
00424173 c3                       ret        
00424174 cc                       int3       
00424175 cc                       int3       
00424176 cc                       int3       
00424177 cc                       int3       
00424178 cc                       int3       
00424179 cc                       int3       
0042417a cc                       int3       
0042417b cc                       int3       
0042417c cc                       int3       
0042417d cc                       int3       
0042417e cc                       int3       
0042417f cc                       int3       
