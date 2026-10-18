004841f0 55                       push       ebp
004841f1 8bec                     mov        ebp, esp
004841f3 51                       push       ecx
004841f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004841f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004841fa 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004841fd 83e101                   and        ecx, 1
00484200 7521                     jne        0x484223
00484202 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484205 e846fdf9ff               call       0x423f50
0048420a 6a00                     push       0
0048420c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048420f e8ccfdf9ff               call       0x423fe0
00484214 8b55fc                   mov        edx, dword ptr [ebp - 4]
00484217 8b420c                   mov        eax, dword ptr [edx + 0xc]
0048421a 83c801                   or         eax, 1
0048421d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484220 89410c                   mov        dword ptr [ecx + 0xc], eax
00484223 f30f2c5508               cvttss2si  edx, dword ptr [ebp + 8]
00484228 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048422b 895004                   mov        dword ptr [eax + 4], edx
0048422e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00484231 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00484236 f30f114108               movss      dword ptr [ecx + 8], xmm0
0048423b f30f104508               movss      xmm0, dword ptr [ebp + 8]
00484240 f30f5c05ccc85600         subss      xmm0, dword ptr [0x56c8cc]
00484248 f30f2cd0                 cvttss2si  edx, xmm0
0048424c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048424f 8910                     mov        dword ptr [eax], edx
00484251 8be5                     mov        esp, ebp
00484253 5d                       pop        ebp
00484254 c20400                   ret        4
00484257 cc                       int3       
00484258 cc                       int3       
00484259 cc                       int3       
0048425a cc                       int3       
0048425b cc                       int3       
0048425c cc                       int3       
0048425d cc                       int3       
0048425e cc                       int3       
0048425f cc                       int3       
