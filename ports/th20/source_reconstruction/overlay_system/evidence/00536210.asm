00536210 55                       push       ebp
00536211 8bec                     mov        ebp, esp
00536213 83ec0c                   sub        esp, 0xc
00536216 894dfc                   mov        dword ptr [ebp - 4], ecx
00536219 e802a3ffff               call       0x530520
0053621e 0fb6c0                   movzx      eax, al
00536221 85c0                     test       eax, eax
00536223 753a                     jne        0x53625f
00536225 6a00                     push       0
00536227 e804a6f2ff               call       0x460830
0053622c 83c404                   add        esp, 4
0053622f 8bc8                     mov        ecx, eax
00536231 e87a94fcff               call       0x4ff6b0
00536236 8945f4                   mov        dword ptr [ebp - 0xc], eax
00536239 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053623c 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
00536240 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00536243 8d4c020a                 lea        ecx, [edx + eax + 0xa]
00536247 894df8                   mov        dword ptr [ebp - 8], ecx
0053624a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0053624d 52                       push       edx
0053624e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00536251 50                       push       eax
00536252 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00536255 51                       push       ecx
00536256 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00536259 e8e2eafcff               call       0x504d40
0053625e 90                       nop        
0053625f 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00536262 52                       push       edx
00536263 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536266 e8a5030000               call       0x536610
0053626b 90                       nop        
0053626c 8be5                     mov        esp, ebp
0053626e 5d                       pop        ebp
0053626f c20c00                   ret        0xc
00536272 cc                       int3       
00536273 cc                       int3       
00536274 cc                       int3       
00536275 cc                       int3       
00536276 cc                       int3       
00536277 cc                       int3       
00536278 cc                       int3       
00536279 cc                       int3       
0053627a cc                       int3       
0053627b cc                       int3       
0053627c cc                       int3       
0053627d cc                       int3       
0053627e cc                       int3       
0053627f cc                       int3       
