004ab650 55                       push       ebp
004ab651 8bec                     mov        ebp, esp
004ab653 83ec20                   sub        esp, 0x20
004ab656 894dfc                   mov        dword ptr [ebp - 4], ecx
004ab659 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ab65c 81c140030000             add        ecx, 0x340
004ab662 e899030000               call       0x4aba00
004ab667 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ab66a 83c101                   add        ecx, 1
004ab66d 3bc1                     cmp        eax, ecx
004ab66f 731c                     jae        0x4ab68d
004ab671 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ab674 81c240030000             add        edx, 0x340
004ab67a 8955f8                   mov        dword ptr [ebp - 8], edx
004ab67d 8b4508                   mov        eax, dword ptr [ebp + 8]
004ab680 83c001                   add        eax, 1
004ab683 50                       push       eax
004ab684 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004ab687 e8e4feffff               call       0x4ab570
004ab68c 90                       nop        
004ab68d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004ab690 81c140030000             add        ecx, 0x340
004ab696 894df4                   mov        dword ptr [ebp - 0xc], ecx
004ab699 8b5508                   mov        edx, dword ptr [ebp + 8]
004ab69c 52                       push       edx
004ab69d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004ab6a0 e89b06feff               call       0x48bd40
004ab6a5 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004ab6a8 8908                     mov        dword ptr [eax], ecx
004ab6aa 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004ab6ae 0f8cc3000000             jl         0x4ab777
004ab6b4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ab6b7 81c240030000             add        edx, 0x340
004ab6bd 8955f0                   mov        dword ptr [ebp - 0x10], edx
004ab6c0 8b4508                   mov        eax, dword ptr [ebp + 8]
004ab6c3 50                       push       eax
004ab6c4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004ab6c7 e87406feff               call       0x48bd40
004ab6cc 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
004ab6cf 894804                   mov        dword ptr [eax + 4], ecx
004ab6d2 837d1400                 cmp        dword ptr [ebp + 0x14], 0
004ab6d6 7456                     je         0x4ab72e
004ab6d8 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ab6db 81c240030000             add        edx, 0x340
004ab6e1 8955ec                   mov        dword ptr [ebp - 0x14], edx
004ab6e4 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004ab6e7 50                       push       eax
004ab6e8 6a40                     push       0x40
004ab6ea 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ab6ed 51                       push       ecx
004ab6ee 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004ab6f1 e84a06feff               call       0x48bd40
004ab6f6 83c008                   add        eax, 8
004ab6f9 50                       push       eax
004ab6fa e811cf0900               call       0x548610
004ab6ff 83c40c                   add        esp, 0xc
004ab702 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ab705 81c240030000             add        edx, 0x340
004ab70b 8955e8                   mov        dword ptr [ebp - 0x18], edx
004ab70e 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004ab711 50                       push       eax
004ab712 6a40                     push       0x40
004ab714 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ab717 51                       push       ecx
004ab718 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004ab71b e82006feff               call       0x48bd40
004ab720 83c048                   add        eax, 0x48
004ab723 50                       push       eax
004ab724 e8e7ce0900               call       0x548610
004ab729 83c40c                   add        esp, 0xc
004ab72c eb49                     jmp        0x4ab777
004ab72e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004ab731 81c240030000             add        edx, 0x340
004ab737 8955e4                   mov        dword ptr [ebp - 0x1c], edx
004ab73a 8b4508                   mov        eax, dword ptr [ebp + 8]
004ab73d 50                       push       eax
004ab73e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004ab741 e8fa05feff               call       0x48bd40
004ab746 b901000000               mov        ecx, 1
004ab74b 6bd100                   imul       edx, ecx, 0
004ab74e c644100800               mov        byte ptr [eax + edx + 8], 0
004ab753 8b45fc                   mov        eax, dword ptr [ebp - 4]
004ab756 0540030000               add        eax, 0x340
004ab75b 8945e0                   mov        dword ptr [ebp - 0x20], eax
004ab75e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004ab761 51                       push       ecx
004ab762 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004ab765 e8d605feff               call       0x48bd40
004ab76a ba01000000               mov        edx, 1
004ab76f 6bca00                   imul       ecx, edx, 0
004ab772 c644084800               mov        byte ptr [eax + ecx + 0x48], 0
004ab777 8be5                     mov        esp, ebp
004ab779 5d                       pop        ebp
004ab77a c21000                   ret        0x10
004ab77d cc                       int3       
004ab77e cc                       int3       
004ab77f cc                       int3       
