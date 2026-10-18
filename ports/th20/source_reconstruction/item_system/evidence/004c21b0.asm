004c21b0 55                       push       ebp
004c21b1 8bec                     mov        ebp, esp
004c21b3 83ec08                   sub        esp, 8
004c21b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c21b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c21bc 8945f8                   mov        dword ptr [ebp - 8], eax
004c21bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c21c2 51                       push       ecx
004c21c3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c21c6 e8a5f7f4ff               call       0x411970
004c21cb 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c21ce c7421400000000           mov        dword ptr [edx + 0x14], 0
004c21d5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c21d8 83c118                   add        ecx, 0x18
004c21db e86069f8ff               call       0x448b40
004c21e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c21e3 81c1fc050000             add        ecx, 0x5fc
004c21e9 e85269f8ff               call       0x448b40
004c21ee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c21f1 81c1e00b0000             add        ecx, 0xbe0
004c21f7 e8c43af6ff               call       0x425cc0
004c21fc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c21ff 81c1e40b0000             add        ecx, 0xbe4
004c2205 e8060cf6ff               call       0x422e10
004c220a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c220d 81c1f00b0000             add        ecx, 0xbf0
004c2213 e8f80bf6ff               call       0x422e10
004c2218 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c221b 0f57c0                   xorps      xmm0, xmm0
004c221e f30f1180fc0b0000         movss      dword ptr [eax + 0xbfc], xmm0
004c2226 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2229 0f57c0                   xorps      xmm0, xmm0
004c222c f30f1181000c0000         movss      dword ptr [ecx + 0xc00], xmm0
004c2234 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2237 81c1040c0000             add        ecx, 0xc04
004c223d e84e0bf6ff               call       0x422d90
004c2242 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2245 81c1140c0000             add        ecx, 0xc14
004c224b e8400bf6ff               call       0x422d90
004c2250 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c2253 c782240c000000000000     mov        dword ptr [edx + 0xc24], 0
004c225d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c2260 c780280c000000000000     mov        dword ptr [eax + 0xc28], 0
004c226a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c226d c7812c0c000000000000     mov        dword ptr [ecx + 0xc2c], 0
004c2277 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c227a 0f57c0                   xorps      xmm0, xmm0
004c227d f30f1182300c0000         movss      dword ptr [edx + 0xc30], xmm0
004c2285 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c2288 c780340c000000000000     mov        dword ptr [eax + 0xc34], 0
004c2292 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c2295 c781380c000000000000     mov        dword ptr [ecx + 0xc38], 0
004c229f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c22a2 c7823c0c000000000000     mov        dword ptr [edx + 0xc3c], 0
004c22ac 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c22af c780400c000000000000     mov        dword ptr [eax + 0xc40], 0
004c22b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c22bc c781440c000000000000     mov        dword ptr [ecx + 0xc44], 0
004c22c6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c22c9 c782480c000000000000     mov        dword ptr [edx + 0xc48], 0
004c22d3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c22d6 8be5                     mov        esp, ebp
004c22d8 5d                       pop        ebp
004c22d9 c3                       ret        
004c22da cc                       int3       
004c22db cc                       int3       
004c22dc cc                       int3       
004c22dd cc                       int3       
004c22de cc                       int3       
004c22df cc                       int3       
