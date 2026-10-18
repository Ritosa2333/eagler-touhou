0045c1f0 55                       push       ebp
0045c1f1 8bec                     mov        ebp, esp
0045c1f3 51                       push       ecx
0045c1f4 894dfc                   mov        dword ptr [ebp - 4], ecx
0045c1f7 8b4508                   mov        eax, dword ptr [ebp + 8]
0045c1fa 50                       push       eax
0045c1fb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c1fe e85da9feff               call       0x446b60
0045c203 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c206 c70114ec5600             mov        dword ptr [ecx], 0x56ec14
0045c20c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c20f 83c108                   add        ecx, 8
0045c212 e879ffffff               call       0x45c190
0045c217 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c21a 81c128030000             add        ecx, 0x328
0045c220 e80bffffff               call       0x45c130
0045c225 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c228 81c1880c0000             add        ecx, 0xc88
0045c22e e8fdfeffff               call       0x45c130
0045c233 6820030000               push       0x320
0045c238 6a00                     push       0
0045c23a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045c23d 81c2e8150000             add        edx, 0x15e8
0045c243 52                       push       edx
0045c244 e817850e00               call       0x544760
0045c249 83c40c                   add        esp, 0xc
0045c24c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c24f 81c108190000             add        ecx, 0x1908
0045c255 e8b66bfcff               call       0x422e10
0045c25a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c25d 81c114190000             add        ecx, 0x1914
0045c263 e8a86bfcff               call       0x422e10
0045c268 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c26b 81c120190000             add        ecx, 0x1920
0045c271 e89a6bfcff               call       0x422e10
0045c276 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045c279 0f57c0                   xorps      xmm0, xmm0
0045c27c f30f11802c190000         movss      dword ptr [eax + 0x192c], xmm0
0045c284 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045c287 81c130190000             add        ecx, 0x1930
0045c28d e8fe6afcff               call       0x422d90
0045c292 90                       nop        
0045c293 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045c296 8be5                     mov        esp, ebp
0045c298 5d                       pop        ebp
0045c299 c20400                   ret        4
0045c29c cc                       int3       
0045c29d cc                       int3       
0045c29e cc                       int3       
0045c29f cc                       int3       
