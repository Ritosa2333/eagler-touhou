004b4490 55                       push       ebp
004b4491 8bec                     mov        ebp, esp
004b4493 83ec54                   sub        esp, 0x54
004b4496 56                       push       esi
004b4497 894dfc                   mov        dword ptr [ebp - 4], ecx
004b449a b968a55b00               mov        ecx, 0x5ba568
004b449f e8ec3d0000               call       0x4b8290
004b44a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b44a7 8b8960010000             mov        ecx, dword ptr [ecx + 0x160]
004b44ad 33f6                     xor        esi, esi
004b44af 8945dc                   mov        dword ptr [ebp - 0x24], eax
004b44b2 8955e0                   mov        dword ptr [ebp - 0x20], edx
004b44b5 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004b44b8 8975d8                   mov        dword ptr [ebp - 0x28], esi
004b44bb 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004b44be 3b55d8                   cmp        edx, dword ptr [ebp - 0x28]
004b44c1 7723                     ja         0x4b44e6
004b44c3 7208                     jb         0x4b44cd
004b44c5 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004b44c8 3b45d4                   cmp        eax, dword ptr [ebp - 0x2c]
004b44cb 7319                     jae        0x4b44e6
004b44cd b968a55b00               mov        ecx, 0x5ba568
004b44d2 e8b93d0000               call       0x4b8290
004b44d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b44da 898160010000             mov        dword ptr [ecx + 0x160], eax
004b44e0 899164010000             mov        dword ptr [ecx + 0x164], edx
004b44e6 b968a55b00               mov        ecx, 0x5ba568
004b44eb e8a03d0000               call       0x4b8290
004b44f0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b44f3 8b8960010000             mov        ecx, dword ptr [ecx + 0x160]
004b44f9 33f6                     xor        esi, esi
004b44fb 8945cc                   mov        dword ptr [ebp - 0x34], eax
004b44fe 8955d0                   mov        dword ptr [ebp - 0x30], edx
004b4501 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004b4504 8975c8                   mov        dword ptr [ebp - 0x38], esi
004b4507 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
004b450a 3b55c8                   cmp        edx, dword ptr [ebp - 0x38]
004b450d 0f8284010000             jb         0x4b4697
004b4513 770c                     ja         0x4b4521
004b4515 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004b4518 3b45c4                   cmp        eax, dword ptr [ebp - 0x3c]
004b451b 0f8276010000             jb         0x4b4697
004b4521 b968a55b00               mov        ecx, 0x5ba568
004b4526 e8653d0000               call       0x4b8290
004b452b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b452e 2b8160010000             sub        eax, dword ptr [ecx + 0x160]
004b4534 1b9164010000             sbb        edx, dword ptr [ecx + 0x164]
004b453a 6a00                     push       0
004b453c 6a20                     push       0x20
004b453e 52                       push       edx
004b453f 50                       push       eax
004b4540 e8bbec0800               call       0x543200
004b4545 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004b4548 8955e8                   mov        dword ptr [ebp - 0x18], edx
004b454b 837de800                 cmp        dword ptr [ebp - 0x18], 0
004b454f 7709                     ja         0x4b455a
004b4551 817de45ed50800           cmp        dword ptr [ebp - 0x1c], 0x8d55e
004b4558 720e                     jb         0x4b4568
004b455a 33d2                     xor        edx, edx
004b455c c745e45ed50800           mov        dword ptr [ebp - 0x1c], 0x8d55e
004b4563 8955e8                   mov        dword ptr [ebp - 0x18], edx
004b4566 eb14                     jmp        0x4b457c
004b4568 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004b456b 0b45e8                   or         eax, dword ptr [ebp - 0x18]
004b456e 750c                     jne        0x4b457c
004b4570 33c9                     xor        ecx, ecx
004b4572 c745e401000000           mov        dword ptr [ebp - 0x1c], 1
004b4579 894de8                   mov        dword ptr [ebp - 0x18], ecx
004b457c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b457f 8955f8                   mov        dword ptr [ebp - 8], edx
004b4582 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b4585 8b886c010000             mov        ecx, dword ptr [eax + 0x16c]
004b458b 3b4de8                   cmp        ecx, dword ptr [ebp - 0x18]
004b458e 7725                     ja         0x4b45b5
004b4590 720e                     jb         0x4b45a0
004b4592 8b55f8                   mov        edx, dword ptr [ebp - 8]
004b4595 8b8268010000             mov        eax, dword ptr [edx + 0x168]
004b459b 3b45e4                   cmp        eax, dword ptr [ebp - 0x1c]
004b459e 7315                     jae        0x4b45b5
004b45a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b45a3 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004b45a6 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b45a9 899168010000             mov        dword ptr [ecx + 0x168], edx
004b45af 89816c010000             mov        dword ptr [ecx + 0x16c], eax
004b45b5 b968a55b00               mov        ecx, 0x5ba568
004b45ba e8d13c0000               call       0x4b8290
004b45bf 99                       cdq        
004b45c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b45c3 2b8160010000             sub        eax, dword ptr [ecx + 0x160]
004b45c9 1b9164010000             sbb        edx, dword ptr [ecx + 0x164]
004b45cf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b45d2 894df4                   mov        dword ptr [ebp - 0xc], ecx
004b45d5 8945bc                   mov        dword ptr [ebp - 0x44], eax
004b45d8 8955c0                   mov        dword ptr [ebp - 0x40], edx
004b45db 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004b45de 8b826c010000             mov        eax, dword ptr [edx + 0x16c]
004b45e4 3b45c0                   cmp        eax, dword ptr [ebp - 0x40]
004b45e7 7239                     jb         0x4b4622
004b45e9 770e                     ja         0x4b45f9
004b45eb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004b45ee 8b9168010000             mov        edx, dword ptr [ecx + 0x168]
004b45f4 3b55bc                   cmp        edx, dword ptr [ebp - 0x44]
004b45f7 7629                     jbe        0x4b4622
004b45f9 b968a55b00               mov        ecx, 0x5ba568
004b45fe e88d3c0000               call       0x4b8290
004b4603 99                       cdq        
004b4604 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b4607 2b8160010000             sub        eax, dword ptr [ecx + 0x160]
004b460d 1b9164010000             sbb        edx, dword ptr [ecx + 0x164]
004b4613 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b4616 898168010000             mov        dword ptr [ecx + 0x168], eax
004b461c 89916c010000             mov        dword ptr [ecx + 0x16c], edx
004b4622 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b4625 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b4628 8b8a60010000             mov        ecx, dword ptr [edx + 0x160]
004b462e 038868010000             add        ecx, dword ptr [eax + 0x168]
004b4634 8b9264010000             mov        edx, dword ptr [edx + 0x164]
004b463a 13906c010000             adc        edx, dword ptr [eax + 0x16c]
004b4640 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b4643 898860010000             mov        dword ptr [eax + 0x160], ecx
004b4649 899064010000             mov        dword ptr [eax + 0x164], edx
004b464f b968a55b00               mov        ecx, 0x5ba568
004b4654 e8373c0000               call       0x4b8290
004b4659 99                       cdq        
004b465a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b465d 894df0                   mov        dword ptr [ebp - 0x10], ecx
004b4660 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004b4663 8955b8                   mov        dword ptr [ebp - 0x48], edx
004b4666 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b4669 8b8264010000             mov        eax, dword ptr [edx + 0x164]
004b466f 3b45b8                   cmp        eax, dword ptr [ebp - 0x48]
004b4672 7223                     jb         0x4b4697
004b4674 770e                     ja         0x4b4684
004b4676 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b4679 8b9160010000             mov        edx, dword ptr [ecx + 0x160]
004b467f 3b55b4                   cmp        edx, dword ptr [ebp - 0x4c]
004b4682 7213                     jb         0x4b4697
004b4684 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b4687 33c9                     xor        ecx, ecx
004b4689 33d2                     xor        edx, edx
004b468b 898868010000             mov        dword ptr [eax + 0x168], ecx
004b4691 89906c010000             mov        dword ptr [eax + 0x16c], edx
004b4697 b968a55b00               mov        ecx, 0x5ba568
004b469c e88f390000               call       0x4b8030
004b46a1 99                       cdq        
004b46a2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b46a5 8945ac                   mov        dword ptr [ebp - 0x54], eax
004b46a8 8955b0                   mov        dword ptr [ebp - 0x50], edx
004b46ab 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b46ae 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b46b1 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
004b46b4 3b8264010000             cmp        eax, dword ptr [edx + 0x164]
004b46ba 7769                     ja         0x4b4725
004b46bc 720e                     jb         0x4b46cc
004b46be 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b46c1 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
004b46c4 3b9160010000             cmp        edx, dword ptr [ecx + 0x160]
004b46ca 7359                     jae        0x4b4725
004b46cc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b46cf 8b8864010000             mov        ecx, dword ptr [eax + 0x164]
004b46d5 51                       push       ecx
004b46d6 8b9060010000             mov        edx, dword ptr [eax + 0x160]
004b46dc 52                       push       edx
004b46dd b968a55b00               mov        ecx, 0x5ba568
004b46e2 e879420000               call       0x4b8960
004b46e7 b968a55b00               mov        ecx, 0x5ba568
004b46ec e8af380000               call       0x4b7fa0
004b46f1 50                       push       eax
004b46f2 b968a55b00               mov        ecx, 0x5ba568
004b46f7 e844420000               call       0x4b8940
004b46fc b968a55b00               mov        ecx, 0x5ba568
004b4701 e87a420000               call       0x4b8980
004b4706 90                       nop        
004b4707 b968a55b00               mov        ecx, 0x5ba568
004b470c e8ff3c0000               call       0x4b8410
004b4711 85c0                     test       eax, eax
004b4713 7510                     jne        0x4b4725
004b4715 6a00                     push       0
004b4717 6a03                     push       3
004b4719 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004b471f e8bc490000               call       0x4b90e0
004b4724 90                       nop        
004b4725 5e                       pop        esi
004b4726 8be5                     mov        esp, ebp
004b4728 5d                       pop        ebp
004b4729 c3                       ret        
004b472a cc                       int3       
004b472b cc                       int3       
004b472c cc                       int3       
004b472d cc                       int3       
004b472e cc                       int3       
004b472f cc                       int3       
