004b6220 55                       push       ebp
004b6221 8bec                     mov        ebp, esp
004b6223 83ec1c                   sub        esp, 0x1c
004b6226 894dfc                   mov        dword ptr [ebp - 4], ecx
004b6229 b968a55b00               mov        ecx, 0x5ba568
004b622e e8cdebfbff               call       0x474e00
004b6233 85c0                     test       eax, eax
004b6235 751e                     jne        0x4b6255
004b6237 b968a55b00               mov        ecx, 0x5ba568
004b623c e82f210000               call       0x4b8370
004b6241 85c0                     test       eax, eax
004b6243 7510                     jne        0x4b6255
004b6245 6a06                     push       6
004b6247 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004b624d e8de61f9ff               call       0x44c430
004b6252 90                       nop        
004b6253 eb10                     jmp        0x4b6265
004b6255 6a00                     push       0
004b6257 6a06                     push       6
004b6259 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004b625f e88c01fdff               call       0x4863f0
004b6264 90                       nop        
004b6265 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6268 c7807001000000000000     mov        dword ptr [eax + 0x170], 0
004b6272 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b6275 e8e61efcff               call       0x478160
004b627a 85c0                     test       eax, eax
004b627c 742f                     je         0x4b62ad
004b627e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b6281 8b91bc010000             mov        edx, dword ptr [ecx + 0x1bc]
004b6287 52                       push       edx
004b6288 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004b628e e89d85ffff               call       0x4ae830
004b6293 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6296 c780bc01000000000000     mov        dword ptr [eax + 0x1bc], 0
004b62a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b62a3 c781bc01000000000000     mov        dword ptr [ecx + 0x1bc], 0
004b62ad b968a55b00               mov        ecx, 0x5ba568
004b62b2 e849ebfbff               call       0x474e00
004b62b7 85c0                     test       eax, eax
004b62b9 7555                     jne        0x4b6310
004b62bb b968a55b00               mov        ecx, 0x5ba568
004b62c0 e8ab200000               call       0x4b8370
004b62c5 85c0                     test       eax, eax
004b62c7 7547                     jne        0x4b6310
004b62c9 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b62cc 83bac001000000           cmp        dword ptr [edx + 0x1c0], 0
004b62d3 7422                     je         0x4b62f7
004b62d5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b62d8 8b88c0010000             mov        ecx, dword ptr [eax + 0x1c0]
004b62de 51                       push       ecx
004b62df 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004b62e5 e88693f6ff               call       0x41f670
004b62ea 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b62ed c782c001000000000000     mov        dword ptr [edx + 0x1c0], 0
004b62f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b62fa c780c001000000000000     mov        dword ptr [eax + 0x1c0], 0
004b6304 c705144a5c0000000000     mov        dword ptr [0x5c4a14], 0
004b630e eb0f                     jmp        0x4b631f
004b6310 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b6313 8b91c0010000             mov        edx, dword ptr [ecx + 0x1c0]
004b6319 8915144a5c00             mov        dword ptr [0x5c4a14], edx
004b631f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6322 83780800                 cmp        dword ptr [eax + 8], 0
004b6326 740c                     je         0x4b6334
004b6328 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b632b 8b4908                   mov        ecx, dword ptr [ecx + 8]
004b632e e8bdc4f5ff               call       0x4127f0
004b6333 90                       nop        
004b6334 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b6337 81c2bc000000             add        edx, 0xbc
004b633d 8955f0                   mov        dword ptr [ebp - 0x10], edx
004b6340 6a00                     push       0
004b6342 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b6345 e836e2f5ff               call       0x414580
004b634a 8bc8                     mov        ecx, eax
004b634c e87f99f9ff               call       0x44fcd0
004b6351 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6354 05bc000000               add        eax, 0xbc
004b6359 8945ec                   mov        dword ptr [ebp - 0x14], eax
004b635c 6a01                     push       1
004b635e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b6361 e81ae2f5ff               call       0x414580
004b6366 8bc8                     mov        ecx, eax
004b6368 e86399f9ff               call       0x44fcd0
004b636d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b6370 81c1cc000000             add        ecx, 0xcc
004b6376 e85599f9ff               call       0x44fcd0
004b637b 90                       nop        
004b637c c745f800000000           mov        dword ptr [ebp - 8], 0
004b6383 eb09                     jmp        0x4b638e
004b6385 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b6388 83c101                   add        ecx, 1
004b638b 894df8                   mov        dword ptr [ebp - 8], ecx
004b638e 837df80a                 cmp        dword ptr [ebp - 8], 0xa
004b6392 7d24                     jge        0x4b63b8
004b6394 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b6397 81c2d0000000             add        edx, 0xd0
004b639d 8955e8                   mov        dword ptr [ebp - 0x18], edx
004b63a0 6a00                     push       0
004b63a2 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b63a5 50                       push       eax
004b63a6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004b63a9 e8d2e1f5ff               call       0x414580
004b63ae 8bc8                     mov        ecx, eax
004b63b0 e8ebb3f5ff               call       0x4117a0
004b63b5 90                       nop        
004b63b6 ebcd                     jmp        0x4b6385
004b63b8 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b63bf eb09                     jmp        0x4b63ca
004b63c1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004b63c4 83c101                   add        ecx, 1
004b63c7 894df4                   mov        dword ptr [ebp - 0xc], ecx
004b63ca 837df40a                 cmp        dword ptr [ebp - 0xc], 0xa
004b63ce 7d22                     jge        0x4b63f2
004b63d0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b63d3 81c294000000             add        edx, 0x94
004b63d9 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004b63dc 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004b63df 50                       push       eax
004b63e0 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004b63e3 e898e1f5ff               call       0x414580
004b63e8 8bc8                     mov        ecx, eax
004b63ea e8e198f9ff               call       0x44fcd0
004b63ef 90                       nop        
004b63f0 ebcf                     jmp        0x4b63c1
004b63f2 6a00                     push       0
004b63f4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b63f7 e82463feff               call       0x49c720
004b63fc b954000000               mov        ecx, 0x54
004b6401 6bd100                   imul       edx, ecx, 0
004b6404 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b6407 c784101c02000000000000   mov        dword ptr [eax + edx + 0x21c], 0
004b6412 b954000000               mov        ecx, 0x54
004b6417 c1e100                   shl        ecx, 0
004b641a 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b641d c7840a1c02000000000000   mov        dword ptr [edx + ecx + 0x21c], 0
004b6428 b854000000               mov        eax, 0x54
004b642d d1e0                     shl        eax, 1
004b642f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b6432 c784011c02000000000000   mov        dword ptr [ecx + eax + 0x21c], 0
004b643d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b6440 8b82a4010000             mov        eax, dword ptr [edx + 0x1a4]
004b6446 0de0000000               or         eax, 0xe0
004b644b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b644e 8981a4010000             mov        dword ptr [ecx + 0x1a4], eax
004b6454 8be5                     mov        esp, ebp
004b6456 5d                       pop        ebp
004b6457 c3                       ret        
004b6458 cc                       int3       
004b6459 cc                       int3       
004b645a cc                       int3       
004b645b cc                       int3       
004b645c cc                       int3       
004b645d cc                       int3       
004b645e cc                       int3       
004b645f cc                       int3       
