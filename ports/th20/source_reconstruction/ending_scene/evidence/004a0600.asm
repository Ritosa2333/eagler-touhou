004a0600 55                       push       ebp
004a0601 8bec                     mov        ebp, esp
004a0603 83ec4c                   sub        esp, 0x4c
004a0606 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0609 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a060c a3e8495c00               mov        dword ptr [0x5c49e8], eax
004a0611 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0614 51                       push       ecx
004a0615 68e00d4a00               push       0x4a0de0
004a061a 6a2f                     push       0x2f
004a061c e89f1cf7ff               call       0x4122c0
004a0621 83c40c                   add        esp, 0xc
004a0624 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0627 894208                   mov        dword ptr [edx + 8], eax
004a062a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a062d 50                       push       eax
004a062e 68a0de4900               push       0x49dea0
004a0633 6a57                     push       0x57
004a0635 e8261df7ff               call       0x412360
004a063a 83c40c                   add        esp, 0xc
004a063d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0640 89410c                   mov        dword ptr [ecx + 0xc], eax
004a0643 51                       push       ecx
004a0644 f30f100588035700         movss      xmm0, dword ptr [0x570388]
004a064c f30f110424               movss      dword ptr [esp], xmm0
004a0651 51                       push       ecx
004a0652 f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
004a065a f30f110424               movss      dword ptr [esp], xmm0
004a065f 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004a0665 e836040000               call       0x4a0aa0
004a066a b968a55b00               mov        ecx, 0x5ba568
004a066f e86cce0100               call       0x4bd4e0
004a0674 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0677 89421c                   mov        dword ptr [edx + 0x1c], eax
004a067a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a067d 8b4820                   mov        ecx, dword ptr [eax + 0x20]
004a0680 83e1fb                   and        ecx, 0xfffffffb
004a0683 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0686 894a20                   mov        dword ptr [edx + 0x20], ecx
004a0689 833d98fc5a0000           cmp        dword ptr [0x5afc98], 0
004a0690 7c2a                     jl         0x4a06bc
004a0692 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0695 8b0d98fc5a00             mov        ecx, dword ptr [0x5afc98]
004a069b 89481c                   mov        dword ptr [eax + 0x1c], ecx
004a069e c70598fc5a00ffffffff     mov        dword ptr [0x5afc98], 0xffffffff
004a06a8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a06ab 8b4220                   mov        eax, dword ptr [edx + 0x20]
004a06ae 83c804                   or         eax, 4
004a06b1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a06b4 894120                   mov        dword ptr [ecx + 0x20], eax
004a06b7 e900020000               jmp        0x4a08bc
004a06bc 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a06bf 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004a06c2 50                       push       eax
004a06c3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a06c9 e822070000               call       0x4a0df0
004a06ce 85c0                     test       eax, eax
004a06d0 750f                     jne        0x4a06e1
004a06d2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a06d5 8b5120                   mov        edx, dword ptr [ecx + 0x20]
004a06d8 83ca01                   or         edx, 1
004a06db 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a06de 895020                   mov        dword ptr [eax + 0x20], edx
004a06e1 6a12                     push       0x12
004a06e3 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a06e9 e802070000               call       0x4a0df0
004a06ee 85c0                     test       eax, eax
004a06f0 7433                     je         0x4a0725
004a06f2 6a13                     push       0x13
004a06f4 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a06fa e8f1060000               call       0x4a0df0
004a06ff 85c0                     test       eax, eax
004a0701 7422                     je         0x4a0725
004a0703 6a14                     push       0x14
004a0705 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a070b e8e0060000               call       0x4a0df0
004a0710 85c0                     test       eax, eax
004a0712 7411                     je         0x4a0725
004a0714 6a15                     push       0x15
004a0716 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a071c e8cf060000               call       0x4a0df0
004a0721 85c0                     test       eax, eax
004a0723 750f                     jne        0x4a0734
004a0725 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0728 8b5120                   mov        edx, dword ptr [ecx + 0x20]
004a072b 83ca02                   or         edx, 2
004a072e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0731 895020                   mov        dword ptr [eax + 0x20], edx
004a0734 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a073a e8a139fcff               call       0x4640e0
004a073f 83c016                   add        eax, 0x16
004a0742 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004a0745 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0748 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004a074b 8955e8                   mov        dword ptr [ebp - 0x18], edx
004a074e 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004a0751 50                       push       eax
004a0752 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a0755 e8b614f7ff               call       0x411c10
004a075a 8945f8                   mov        dword ptr [ebp - 8], eax
004a075d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004a0760 0fbe11                   movsx      edx, byte ptr [ecx]
004a0763 83ca01                   or         edx, 1
004a0766 8b45f8                   mov        eax, dword ptr [ebp - 8]
004a0769 8810                     mov        byte ptr [eax], dl
004a076b b968a55b00               mov        ecx, 0x5ba568
004a0770 e82b39fcff               call       0x4640a0
004a0775 83f801                   cmp        eax, 1
004a0778 753c                     jne        0x4a07b6
004a077a 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a0780 e85b39fcff               call       0x4640e0
004a0785 83c016                   add        eax, 0x16
004a0788 8945dc                   mov        dword ptr [ebp - 0x24], eax
004a078b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a078e 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004a0791 8955e0                   mov        dword ptr [ebp - 0x20], edx
004a0794 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004a0797 50                       push       eax
004a0798 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a079b e87014f7ff               call       0x411c10
004a07a0 8945f4                   mov        dword ptr [ebp - 0xc], eax
004a07a3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a07a6 0fbe11                   movsx      edx, byte ptr [ecx]
004a07a9 83ca02                   or         edx, 2
004a07ac 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004a07af 8810                     mov        byte ptr [eax], dl
004a07b1 e98e000000               jmp        0x4a0844
004a07b6 b968a55b00               mov        ecx, 0x5ba568
004a07bb e8e038fcff               call       0x4640a0
004a07c0 83f802                   cmp        eax, 2
004a07c3 7539                     jne        0x4a07fe
004a07c5 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a07cb e81039fcff               call       0x4640e0
004a07d0 83c016                   add        eax, 0x16
004a07d3 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004a07d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a07d9 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004a07dc 8955d8                   mov        dword ptr [ebp - 0x28], edx
004a07df 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004a07e2 50                       push       eax
004a07e3 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004a07e6 e82514f7ff               call       0x411c10
004a07eb 8945f0                   mov        dword ptr [ebp - 0x10], eax
004a07ee 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a07f1 0fbe11                   movsx      edx, byte ptr [ecx]
004a07f4 83ca04                   or         edx, 4
004a07f7 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004a07fa 8810                     mov        byte ptr [eax], dl
004a07fc eb46                     jmp        0x4a0844
004a07fe b968a55b00               mov        ecx, 0x5ba568
004a0803 e89838fcff               call       0x4640a0
004a0808 83f803                   cmp        eax, 3
004a080b 7537                     jne        0x4a0844
004a080d 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a0813 e8c838fcff               call       0x4640e0
004a0818 83c016                   add        eax, 0x16
004a081b 8945cc                   mov        dword ptr [ebp - 0x34], eax
004a081e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0821 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004a0824 8955d0                   mov        dword ptr [ebp - 0x30], edx
004a0827 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004a082a 50                       push       eax
004a082b 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004a082e e8dd13f7ff               call       0x411c10
004a0833 8945ec                   mov        dword ptr [ebp - 0x14], eax
004a0836 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004a0839 0fbe11                   movsx      edx, byte ptr [ecx]
004a083c 83ca08                   or         edx, 8
004a083f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004a0842 8810                     mov        byte ptr [eax], dl
004a0844 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a084a e89138fcff               call       0x4640e0
004a084f 83c016                   add        eax, 0x16
004a0852 8945c8                   mov        dword ptr [ebp - 0x38], eax
004a0855 6a12                     push       0x12
004a0857 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004a085a e8b113f7ff               call       0x411c10
004a085f c60001                   mov        byte ptr [eax], 1
004a0862 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a0868 e87338fcff               call       0x4640e0
004a086d 83c016                   add        eax, 0x16
004a0870 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004a0873 6a13                     push       0x13
004a0875 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004a0878 e89313f7ff               call       0x411c10
004a087d c60001                   mov        byte ptr [eax], 1
004a0880 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a0886 e85538fcff               call       0x4640e0
004a088b 83c016                   add        eax, 0x16
004a088e 8945c0                   mov        dword ptr [ebp - 0x40], eax
004a0891 6a14                     push       0x14
004a0893 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004a0896 e87513f7ff               call       0x411c10
004a089b c60001                   mov        byte ptr [eax], 1
004a089e 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004a08a4 e83738fcff               call       0x4640e0
004a08a9 83c016                   add        eax, 0x16
004a08ac 8945bc                   mov        dword ptr [ebp - 0x44], eax
004a08af 6a15                     push       0x15
004a08b1 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004a08b4 e85713f7ff               call       0x411c10
004a08b9 c60001                   mov        byte ptr [eax], 1
004a08bc 6a00                     push       0
004a08be 6a00                     push       0
004a08c0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a08c3 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004a08c6 8b0495a0fc5a00           mov        eax, dword ptr [edx*4 + 0x5afca0]
004a08cd 50                       push       eax
004a08ce e8cdcb0100               call       0x4bd4a0
004a08d3 83c404                   add        esp, 4
004a08d6 50                       push       eax
004a08d7 e8c401f7ff               call       0x410aa0
004a08dc 83c40c                   add        esp, 0xc
004a08df 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a08e2 894114                   mov        dword ptr [ecx + 0x14], eax
004a08e5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a08e8 837a1400                 cmp        dword ptr [edx + 0x14], 0
004a08ec 7517                     jne        0x4a0905
004a08ee 6810f65600               push       0x56f610
004a08f3 6878065c00               push       0x5c0678
004a08f8 e85338fbff               call       0x454150
004a08fd 83c408                   add        esp, 8
004a0900 83c8ff                   or         eax, 0xffffffff
004a0903 eb63                     jmp        0x4a0968
004a0905 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0908 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004a090b 894db8                   mov        dword ptr [ebp - 0x48], ecx
004a090e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a0911 8b4214                   mov        eax, dword ptr [edx + 0x14]
004a0914 50                       push       eax
004a0915 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a0918 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004a091b b808000000               mov        eax, 8
004a0920 6bc800                   imul       ecx, eax, 0
004a0923 8b540a04                 mov        edx, dword ptr [edx + ecx + 4]
004a0927 52                       push       edx
004a0928 e83308fdff               call       0x471160
004a092d 83c408                   add        esp, 8
004a0930 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004a0933 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
004a0936 50                       push       eax
004a0937 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004a093a 51                       push       ecx
004a093b 6868025700               push       0x570268
004a0940 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004a0946 e835e0ffff               call       0x49e980
004a094b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a094e 894218                   mov        dword ptr [edx + 0x18], eax
004a0951 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a0954 83781c12                 cmp        dword ptr [eax + 0x1c], 0x12
004a0958 7c0c                     jl         0x4a0966
004a095a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a095d 8b4918                   mov        ecx, dword ptr [ecx + 0x18]
004a0960 e88b060000               call       0x4a0ff0
004a0965 90                       nop        
004a0966 33c0                     xor        eax, eax
004a0968 8be5                     mov        esp, ebp
004a096a 5d                       pop        ebp
004a096b c3                       ret        
004a096c cc                       int3       
004a096d cc                       int3       
004a096e cc                       int3       
004a096f cc                       int3       
