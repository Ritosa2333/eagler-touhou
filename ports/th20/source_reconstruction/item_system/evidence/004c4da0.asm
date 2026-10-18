004c4da0 55                       push       ebp
004c4da1 8bec                     mov        ebp, esp
004c4da3 83ec20                   sub        esp, 0x20
004c4da6 56                       push       esi
004c4da7 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4daa 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4dad 8b88480c0000             mov        ecx, dword ptr [eax + 0xc48]
004c4db3 e8387df5ff               call       0x41caf0
004c4db8 8bc8                     mov        ecx, eax
004c4dba e8f145fdff               call       0x4993b0
004c4dbf 8bf0                     mov        esi, eax
004c4dc1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4dc4 8b89480c0000             mov        ecx, dword ptr [ecx + 0xc48]
004c4dca e8217df5ff               call       0x41caf0
004c4dcf 8bc8                     mov        ecx, eax
004c4dd1 e83a34ffff               call       0x4b8210
004c4dd6 3bf0                     cmp        esi, eax
004c4dd8 7c64                     jl         0x4c4e3e
004c4dda c745f8204e0000           mov        dword ptr [ebp - 8], 0x4e20
004c4de1 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c4de4 52                       push       edx
004c4de5 b968a55b00               mov        ecx, 0x5ba568
004c4dea e86137fcff               call       0x488550
004c4def 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4df2 8b88480c0000             mov        ecx, dword ptr [eax + 0xc48]
004c4df8 e813caf5ff               call       0x421810
004c4dfd 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c4e00 68808080ff               push       0xff808080
004c4e05 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c4e08 51                       push       ecx
004c4e09 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4e0c 81c2e40b0000             add        edx, 0xbe4
004c4e12 52                       push       edx
004c4e13 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c4e16 e8f5b80400               call       0x510710
004c4e1b 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4e1e 51                       push       ecx
004c4e1f f30f1080e40b0000         movss      xmm0, dword ptr [eax + 0xbe4]
004c4e27 f30f110424               movss      dword ptr [esp], xmm0
004c4e2c 6a0d                     push       0xd
004c4e2e b930a85b00               mov        ecx, 0x5ba830
004c4e33 e87820f6ff               call       0x426eb0
004c4e38 90                       nop        
004c4e39 e9a8000000               jmp        0x4c4ee6
004c4e3e c745f864000000           mov        dword ptr [ebp - 8], 0x64
004c4e45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4e48 8b89480c0000             mov        ecx, dword ptr [ecx + 0xc48]
004c4e4e e89d7cf5ff               call       0x41caf0
004c4e53 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c4e56 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4e59 8b8a480c0000             mov        ecx, dword ptr [edx + 0xc48]
004c4e5f e88c7cf5ff               call       0x41caf0
004c4e64 8bc8                     mov        ecx, eax
004c4e66 e86533ffff               call       0x4b81d0
004c4e6b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c4e6e 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c4e71 50                       push       eax
004c4e72 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c4e75 e896c50100               call       0x4e1410
004c4e7a 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c4e7d 837de800                 cmp        dword ptr [ebp - 0x18], 0
004c4e81 7463                     je         0x4c4ee6
004c4e83 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4e86 8b89480c0000             mov        ecx, dword ptr [ecx + 0xc48]
004c4e8c e8ffb0f4ff               call       0x40ff90
004c4e91 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c4e94 6aff                     push       -1
004c4e96 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c4e99 e8025e0300               call       0x4faca0
004c4e9e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4ea1 51                       push       ecx
004c4ea2 f30f1082e40b0000         movss      xmm0, dword ptr [edx + 0xbe4]
004c4eaa f30f110424               movss      dword ptr [esp], xmm0
004c4eaf 6a0d                     push       0xd
004c4eb1 b930a85b00               mov        ecx, 0x5ba830
004c4eb6 e8f51ff6ff               call       0x426eb0
004c4ebb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4ebe 8b88480c0000             mov        ecx, dword ptr [eax + 0xc48]
004c4ec4 e847c9f5ff               call       0x421810
004c4ec9 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c4ecc 6840ffffff               push       0xffffff40
004c4ed1 6aff                     push       -1
004c4ed3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4ed6 81c1e40b0000             add        ecx, 0xbe4
004c4edc 51                       push       ecx
004c4edd 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c4ee0 e82bb80400               call       0x510710
004c4ee5 90                       nop        
004c4ee6 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c4ee9 52                       push       edx
004c4eea b968a55b00               mov        ecx, 0x5ba568
004c4eef e85c36fcff               call       0x488550
004c4ef4 90                       nop        
004c4ef5 5e                       pop        esi
004c4ef6 8be5                     mov        esp, ebp
004c4ef8 5d                       pop        ebp
004c4ef9 c3                       ret        
004c4efa cc                       int3       
004c4efb cc                       int3       
004c4efc cc                       int3       
004c4efd cc                       int3       
004c4efe cc                       int3       
004c4eff cc                       int3       
