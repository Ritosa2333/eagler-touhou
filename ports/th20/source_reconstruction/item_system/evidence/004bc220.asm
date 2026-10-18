004bc220 55                       push       ebp
004bc221 8bec                     mov        ebp, esp
004bc223 83ec58                   sub        esp, 0x58
004bc226 56                       push       esi
004bc227 894dfc                   mov        dword ptr [ebp - 4], ecx
004bc22a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bc22d c78068c8490000000000     mov        dword ptr [eax + 0x49c868], 0
004bc237 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc23a c7816cc8490000000000     mov        dword ptr [ecx + 0x49c86c], 0
004bc244 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc247 c78274c8490000000000     mov        dword ptr [edx + 0x49c874], 0
004bc251 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc254 83c118                   add        ecx, 0x18
004bc257 e8042a0000               call       0x4bec60
004bc25c 69c04c0c0000             imul       eax, eax, 0xc4c
004bc262 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bc265 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc268 83c118                   add        ecx, 0x18
004bc26b e830fbf4ff               call       0x40bda0
004bc270 8945ec                   mov        dword ptr [ebp - 0x14], eax
004bc273 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bc276 51                       push       ecx
004bc277 6a00                     push       0
004bc279 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004bc27c 52                       push       edx
004bc27d e8de840800               call       0x544760
004bc282 83c40c                   add        esp, 0xc
004bc285 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bc288 0518c84900               add        eax, 0x49c818
004bc28d 8945e8                   mov        dword ptr [ebp - 0x18], eax
004bc290 6a00                     push       0
004bc292 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004bc295 e8a6e3f8ff               call       0x44a640
004bc29a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc29d 81c130c84900             add        ecx, 0x49c830
004bc2a3 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004bc2a6 6a00                     push       0
004bc2a8 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004bc2ab e890e3f8ff               call       0x44a640
004bc2b0 90                       nop        
004bc2b1 c745f800000000           mov        dword ptr [ebp - 8], 0
004bc2b8 eb09                     jmp        0x4bc2c3
004bc2ba 8b55f8                   mov        edx, dword ptr [ebp - 8]
004bc2bd 83c201                   add        edx, 1
004bc2c0 8955f8                   mov        dword ptr [ebp - 8], edx
004bc2c3 817df800020000           cmp        dword ptr [ebp - 8], 0x200
004bc2ca 0f8d8d000000             jge        0x4bc35d
004bc2d0 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bc2d3 83c018                   add        eax, 0x18
004bc2d6 8945e0                   mov        dword ptr [ebp - 0x20], eax
004bc2d9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bc2dc 51                       push       ecx
004bc2dd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004bc2e0 e8dbe0ffff               call       0x4ba3c0
004bc2e5 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004bc2e8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc2eb 83c218                   add        edx, 0x18
004bc2ee 8955dc                   mov        dword ptr [ebp - 0x24], edx
004bc2f1 8b45f8                   mov        eax, dword ptr [ebp - 8]
004bc2f4 50                       push       eax
004bc2f5 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004bc2f8 e8c3e0ffff               call       0x4ba3c0
004bc2fd 8945d8                   mov        dword ptr [ebp - 0x28], eax
004bc300 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004bc303 51                       push       ecx
004bc304 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004bc307 e864e3f8ff               call       0x44a670
004bc30c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc30f 83c218                   add        edx, 0x18
004bc312 8955d0                   mov        dword ptr [ebp - 0x30], edx
004bc315 8b75fc                   mov        esi, dword ptr [ebp - 4]
004bc318 81c630c84900             add        esi, 0x49c830
004bc31e 8b45f8                   mov        eax, dword ptr [ebp - 8]
004bc321 50                       push       eax
004bc322 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004bc325 e896e0ffff               call       0x4ba3c0
004bc32a 897014                   mov        dword ptr [eax + 0x14], esi
004bc32d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc330 81c130c84900             add        ecx, 0x49c830
004bc336 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004bc339 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc33c 83c218                   add        edx, 0x18
004bc33f 8955cc                   mov        dword ptr [ebp - 0x34], edx
004bc342 8b45f8                   mov        eax, dword ptr [ebp - 8]
004bc345 50                       push       eax
004bc346 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004bc349 e872e0ffff               call       0x4ba3c0
004bc34e 50                       push       eax
004bc34f 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004bc352 e8495bf5ff               call       0x411ea0
004bc357 90                       nop        
004bc358 e95dffffff               jmp        0x4bc2ba
004bc35d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc360 81c148c84900             add        ecx, 0x49c848
004bc366 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004bc369 6a00                     push       0
004bc36b 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004bc36e e8cde2f8ff               call       0x44a640
004bc373 90                       nop        
004bc374 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004bc37b eb09                     jmp        0x4bc386
004bc37d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004bc380 83c201                   add        edx, 1
004bc383 8955f4                   mov        dword ptr [ebp - 0xc], edx
004bc386 817df400040000           cmp        dword ptr [ebp - 0xc], 0x400
004bc38d 0f8da2000000             jge        0x4bc435
004bc393 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bc396 83c018                   add        eax, 0x18
004bc399 8945c0                   mov        dword ptr [ebp - 0x40], eax
004bc39c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bc39f 81c100020000             add        ecx, 0x200
004bc3a5 51                       push       ecx
004bc3a6 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004bc3a9 e812e0ffff               call       0x4ba3c0
004bc3ae 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004bc3b1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc3b4 83c218                   add        edx, 0x18
004bc3b7 8955bc                   mov        dword ptr [ebp - 0x44], edx
004bc3ba 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004bc3bd 0500020000               add        eax, 0x200
004bc3c2 50                       push       eax
004bc3c3 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004bc3c6 e8f5dfffff               call       0x4ba3c0
004bc3cb 8945b8                   mov        dword ptr [ebp - 0x48], eax
004bc3ce 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004bc3d1 51                       push       ecx
004bc3d2 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004bc3d5 e896e2f8ff               call       0x44a670
004bc3da 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc3dd 83c218                   add        edx, 0x18
004bc3e0 8955b0                   mov        dword ptr [ebp - 0x50], edx
004bc3e3 8b75fc                   mov        esi, dword ptr [ebp - 4]
004bc3e6 81c648c84900             add        esi, 0x49c848
004bc3ec 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004bc3ef 0500020000               add        eax, 0x200
004bc3f4 50                       push       eax
004bc3f5 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004bc3f8 e8c3dfffff               call       0x4ba3c0
004bc3fd 897014                   mov        dword ptr [eax + 0x14], esi
004bc400 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc403 81c148c84900             add        ecx, 0x49c848
004bc409 894da8                   mov        dword ptr [ebp - 0x58], ecx
004bc40c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc40f 83c218                   add        edx, 0x18
004bc412 8955ac                   mov        dword ptr [ebp - 0x54], edx
004bc415 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004bc418 0500020000               add        eax, 0x200
004bc41d 50                       push       eax
004bc41e 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004bc421 e89adfffff               call       0x4ba3c0
004bc426 50                       push       eax
004bc427 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004bc42a e8715af5ff               call       0x411ea0
004bc42f 90                       nop        
004bc430 e948ffffff               jmp        0x4bc37d
004bc435 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc438 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004bc440 f30f118160c84900         movss      dword ptr [ecx + 0x49c860], xmm0
004bc448 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc44b c78280c8490000000000     mov        dword ptr [edx + 0x49c880], 0
004bc455 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bc458 c7807cc8490064000000     mov        dword ptr [eax + 0x49c87c], 0x64
004bc462 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bc465 c78178c8490000000000     mov        dword ptr [ecx + 0x49c878], 0
004bc46f 5e                       pop        esi
004bc470 8be5                     mov        esp, ebp
004bc472 5d                       pop        ebp
004bc473 c3                       ret        
004bc474 cc                       int3       
004bc475 cc                       int3       
004bc476 cc                       int3       
004bc477 cc                       int3       
004bc478 cc                       int3       
004bc479 cc                       int3       
004bc47a cc                       int3       
004bc47b cc                       int3       
004bc47c cc                       int3       
004bc47d cc                       int3       
004bc47e cc                       int3       
004bc47f cc                       int3       
