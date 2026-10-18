004c4420 55                       push       ebp
004c4421 8bec                     mov        ebp, esp
004c4423 83ec24                   sub        esp, 0x24
004c4426 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4429 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c442c c780240c000002000000     mov        dword ptr [eax + 0xc24], 2
004c4436 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4439 83b9280c00000d           cmp        dword ptr [ecx + 0xc28], 0xd
004c4440 7579                     jne        0x4c44bb
004c4442 b9a8a45b00               mov        ecx, 0x5ba4a8
004c4447 e894faf5ff               call       0x423ee0
004c444c 33d2                     xor        edx, edx
004c444e b908000000               mov        ecx, 8
004c4453 f7f1                     div        ecx
004c4455 83fa04                   cmp        edx, 4
004c4458 7742                     ja         0x4c449c
004c445a 6a00                     push       0
004c445c e81ffcf9ff               call       0x464080
004c4461 83c404                   add        esp, 4
004c4464 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c4467 b9a8a45b00               mov        ecx, 0x5ba4a8
004c446c e86ffaf5ff               call       0x423ee0
004c4471 33d2                     xor        edx, edx
004c4473 b904000000               mov        ecx, 4
004c4478 f7f1                     div        ecx
004c447a 8955f4                   mov        dword ptr [ebp - 0xc], edx
004c447d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c4480 52                       push       edx
004c4481 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c4484 e847fdf9ff               call       0x4641d0
004c4489 99                       cdq        
004c448a 2bc2                     sub        eax, edx
004c448c d1f8                     sar        eax, 1
004c448e 83c009                   add        eax, 9
004c4491 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4494 8981280c0000             mov        dword ptr [ecx + 0xc28], eax
004c449a eb1f                     jmp        0x4c44bb
004c449c b9a8a45b00               mov        ecx, 0x5ba4a8
004c44a1 e83afaf5ff               call       0x423ee0
004c44a6 33d2                     xor        edx, edx
004c44a8 b904000000               mov        ecx, 4
004c44ad f7f1                     div        ecx
004c44af 83c209                   add        edx, 9
004c44b2 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c44b5 8990280c0000             mov        dword ptr [eax + 0xc28], edx
004c44bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c44be 8b91280c0000             mov        edx, dword ptr [ecx + 0xc28]
004c44c4 8955f8                   mov        dword ptr [ebp - 8], edx
004c44c7 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c44ca 83e809                   sub        eax, 9
004c44cd 8945f8                   mov        dword ptr [ebp - 8], eax
004c44d0 837df803                 cmp        dword ptr [ebp - 8], 3
004c44d4 0f8794000000             ja         0x4c456e
004c44da 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c44dd ff248da0454c00           jmp        dword ptr [ecx*4 + 0x4c45a0]
004c44e4 e877740500               call       0x51b960
004c44e9 8bc8                     mov        ecx, eax
004c44eb e810d2f4ff               call       0x411700
004c44f0 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c44f3 6a25                     push       0x25
004c44f5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c44f8 83c218                   add        edx, 0x18
004c44fb 52                       push       edx
004c44fc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c44ff e87c3ef7ff               call       0x438380
004c4504 90                       nop        
004c4505 eb67                     jmp        0x4c456e
004c4507 e854740500               call       0x51b960
004c450c 8bc8                     mov        ecx, eax
004c450e e8edd1f4ff               call       0x411700
004c4513 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c4516 6a26                     push       0x26
004c4518 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c451b 83c018                   add        eax, 0x18
004c451e 50                       push       eax
004c451f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c4522 e8593ef7ff               call       0x438380
004c4527 90                       nop        
004c4528 eb44                     jmp        0x4c456e
004c452a e831740500               call       0x51b960
004c452f 8bc8                     mov        ecx, eax
004c4531 e8cad1f4ff               call       0x411700
004c4536 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c4539 6a27                     push       0x27
004c453b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c453e 83c118                   add        ecx, 0x18
004c4541 51                       push       ecx
004c4542 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c4545 e8363ef7ff               call       0x438380
004c454a 90                       nop        
004c454b eb21                     jmp        0x4c456e
004c454d e80e740500               call       0x51b960
004c4552 8bc8                     mov        ecx, eax
004c4554 e8a7d1f4ff               call       0x411700
004c4559 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c455c 6a28                     push       0x28
004c455e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4561 83c218                   add        edx, 0x18
004c4564 52                       push       edx
004c4565 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c4568 e8133ef7ff               call       0x438380
004c456d 90                       nop        
004c456e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4571 83c018                   add        eax, 0x18
004c4574 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c4577 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c457a 81c1e40b0000             add        ecx, 0xbe4
004c4580 51                       push       ecx
004c4581 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c4584 e8d797f9ff               call       0x45dd60
004c4589 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c458c 81c1fc050000             add        ecx, 0x5fc
004c4592 e869b7f8ff               call       0x44fd00
004c4597 33c0                     xor        eax, eax
004c4599 8be5                     mov        esp, ebp
004c459b 5d                       pop        ebp
004c459c c3                       ret        
004c459d 0f1f00                   nop        dword ptr [eax]
004c45a0 e444                     in         al, 0x44
004c45a2 4c                       dec        esp
004c45a3 0007                     add        byte ptr [edi], al
004c45a5 45                       inc        ebp
004c45a6 4c                       dec        esp
004c45a7 002a                     add        byte ptr [edx], ch
004c45a9 45                       inc        ebp
004c45aa 4c                       dec        esp
004c45ab 004d45                   add        byte ptr [ebp + 0x45], cl
004c45ae 4c                       dec        esp
