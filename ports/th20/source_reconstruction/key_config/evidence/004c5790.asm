004c5790 55                       push       ebp
004c5791 8bec                     mov        ebp, esp
004c5793 83ec18                   sub        esp, 0x18
004c5796 894dfc                   mov        dword ptr [ebp - 4], ecx
004c5799 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c579c 8b88fc000000             mov        ecx, dword ptr [eax + 0xfc]
004c57a2 894df8                   mov        dword ptr [ebp - 8], ecx
004c57a5 837df803                 cmp        dword ptr [ebp - 8], 3
004c57a9 775c                     ja         0x4c5807
004c57ab 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c57ae ff24955c584c00           jmp        dword ptr [edx*4 + 0x4c585c]
004c57b5 6a01                     push       1
004c57b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c57ba e8110a0000               call       0x4c61d0
004c57bf 90                       nop        
004c57c0 eb45                     jmp        0x4c5807
004c57c2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c57c5 e896190000               call       0x4c7160
004c57ca 90                       nop        
004c57cb eb3a                     jmp        0x4c5807
004c57cd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c57d0 e85b0a0000               call       0x4c6230
004c57d5 90                       nop        
004c57d6 eb2f                     jmp        0x4c5807
004c57d8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c57db 05c0000000               add        eax, 0xc0
004c57e0 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c57e3 6a0a                     push       0xa
004c57e5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c57e8 e8d3ddf5ff               call       0x4235c0
004c57ed 0fb6c8                   movzx      ecx, al
004c57f0 85c9                     test       ecx, ecx
004c57f2 7413                     je         0x4c5807
004c57f4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c57f7 52                       push       edx
004c57f8 e8c3bff5ff               call       0x4217c0
004c57fd 83c404                   add        esp, 4
004c5800 b801000000               mov        eax, 1
004c5805 eb4e                     jmp        0x4c5855
004c5807 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c580a 05c0000000               add        eax, 0xc0
004c580f 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c5812 6a00                     push       0
004c5814 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c5817 e824ddf5ff               call       0x423540
004c581c 90                       nop        
004c581d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5820 81c1ec000000             add        ecx, 0xec
004c5826 894dec                   mov        dword ptr [ebp - 0x14], ecx
004c5829 6a00                     push       0
004c582b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c582e e83db8f9ff               call       0x461070
004c5833 0fb6d0                   movzx      edx, al
004c5836 85d2                     test       edx, edx
004c5838 7416                     je         0x4c5850
004c583a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c583d 05ec000000               add        eax, 0xec
004c5842 8945e8                   mov        dword ptr [ebp - 0x18], eax
004c5845 6a00                     push       0
004c5847 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c584a e8d13bf6ff               call       0x429420
004c584f 90                       nop        
004c5850 b801000000               mov        eax, 1
004c5855 8be5                     mov        esp, ebp
004c5857 5d                       pop        ebp
004c5858 c3                       ret        
004c5859 0f1f00                   nop        dword ptr [eax]
004c585c b557                     mov        ch, 0x57
004c585e 4c                       dec        esp
004c585f 00c2                     add        dl, al
004c5861 57                       push       edi
004c5862 4c                       dec        esp
004c5863 00cd                     add        ch, cl
004c5865 57                       push       edi
004c5866 4c                       dec        esp
004c5867 00d8                     add        al, bl
004c5869 57                       push       edi
004c586a 4c                       dec        esp
004c586b 00cc                     add        ah, cl
004c586d cc                       int3       
004c586e cc                       int3       
004c586f cc                       int3       
