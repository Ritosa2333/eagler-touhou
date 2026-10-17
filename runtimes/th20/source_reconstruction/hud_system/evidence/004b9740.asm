004b9740 55                       push       ebp
004b9741 8bec                     mov        ebp, esp
004b9743 81ec1c010000             sub        esp, 0x11c
004b9749 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004b974e 33c5                     xor        eax, ebp
004b9750 8945fc                   mov        dword ptr [ebp - 4], eax
004b9753 898df8feffff             mov        dword ptr [ebp - 0x108], ecx
004b9759 9b                       wait       
004b975a dbe3                     fninit     
004b975c 837d08ff                 cmp        dword ptr [ebp + 8], -1
004b9760 740a                     je         0x4b976c
004b9762 837d08fd                 cmp        dword ptr [ebp + 8], -3
004b9766 0f8568010000             jne        0x4b98d4
004b976c 837d08ff                 cmp        dword ptr [ebp + 8], -1
004b9770 750c                     jne        0x4b977e
004b9772 c785ecfeffff01000000     mov        dword ptr [ebp - 0x114], 1
004b977c eb0a                     jmp        0x4b9788
004b977e c785ecfeffff00000000     mov        dword ptr [ebp - 0x114], 0
004b9788 8b85ecfeffff             mov        eax, dword ptr [ebp - 0x114]
004b978e 8985f0feffff             mov        dword ptr [ebp - 0x110], eax
004b9794 b968a55b00               mov        ecx, 0x5ba568
004b9799 e8b2f0fcff               call       0x488850
004b979e 0fb6c8                   movzx      ecx, al
004b97a1 85c9                     test       ecx, ecx
004b97a3 0f84cf000000             je         0x4b9878
004b97a9 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004b97af e87cf0fcff               call       0x488830
004b97b4 85c0                     test       eax, eax
004b97b6 0f85bc000000             jne        0x4b9878
004b97bc 8b95f0feffff             mov        edx, dword ptr [ebp - 0x110]
004b97c2 a110615c00               mov        eax, dword ptr [0x5c6110]
004b97c7 8b4c900c                 mov        ecx, dword ptr [eax + edx*4 + 0xc]
004b97cb 51                       push       ecx
004b97cc 6800010000               push       0x100
004b97d1 8d95fcfeffff             lea        edx, [ebp - 0x104]
004b97d7 52                       push       edx
004b97d8 e833ee0800               call       0x548610
004b97dd 83c40c                   add        esp, 0xc
004b97e0 8d85fcfeffff             lea        eax, [ebp - 0x104]
004b97e6 50                       push       eax
004b97e7 e834660900               call       0x54fe20
004b97ec 83c404                   add        esp, 4
004b97ef 8d8c05fcfeffff           lea        ecx, [ebp + eax - 0x104]
004b97f6 898df4feffff             mov        dword ptr [ebp - 0x10c], ecx
004b97fc ba01000000               mov        edx, 1
004b9801 6bc200                   imul       eax, edx, 0
004b9804 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
004b980a c604012e                 mov        byte ptr [ecx + eax], 0x2e
004b980e ba01000000               mov        edx, 1
004b9813 c1e200                   shl        edx, 0
004b9816 8b85f4feffff             mov        eax, dword ptr [ebp - 0x10c]
004b981c c6041077                 mov        byte ptr [eax + edx], 0x77
004b9820 b901000000               mov        ecx, 1
004b9825 d1e1                     shl        ecx, 1
004b9827 8b95f4feffff             mov        edx, dword ptr [ebp - 0x10c]
004b982d c6040a61                 mov        byte ptr [edx + ecx], 0x61
004b9831 b801000000               mov        eax, 1
004b9836 6bc803                   imul       ecx, eax, 3
004b9839 8b95f4feffff             mov        edx, dword ptr [ebp - 0x10c]
004b983f c6040a76                 mov        byte ptr [edx + ecx], 0x76
004b9843 b801000000               mov        eax, 1
004b9848 c1e002                   shl        eax, 2
004b984b 8b8df4feffff             mov        ecx, dword ptr [ebp - 0x10c]
004b9851 c6040100                 mov        byte ptr [ecx + eax], 0
004b9855 8d95fcfeffff             lea        edx, [ebp - 0x104]
004b985b 52                       push       edx
004b985c b930a85b00               mov        ecx, 0x5ba830
004b9861 e82ae8ffff               call       0x4b8090
004b9866 50                       push       eax
004b9867 e8a4640900               call       0x54fd10
004b986c 83c408                   add        esp, 8
004b986f 85c0                     test       eax, eax
004b9871 7505                     jne        0x4b9878
004b9873 e92a010000               jmp        0x4b99a2
004b9878 8b85f0feffff             mov        eax, dword ptr [ebp - 0x110]
004b987e 8b0d10615c00             mov        ecx, dword ptr [0x5c6110]
004b9884 8b548158                 mov        edx, dword ptr [ecx + eax*4 + 0x58]
004b9888 52                       push       edx
004b9889 8b85f0feffff             mov        eax, dword ptr [ebp - 0x110]
004b988f 50                       push       eax
004b9890 b9404d5c00               mov        ecx, 0x5c4d40
004b9895 e8b6020200               call       0x4d9b50
004b989a 90                       nop        
004b989b 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004b98a1 e8cae9ffff               call       0x4b8270
004b98a6 8985e8feffff             mov        dword ptr [ebp - 0x118], eax
004b98ac 6a00                     push       0
004b98ae 6aff                     push       -1
004b98b0 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
004b98b6 83c101                   add        ecx, 1
004b98b9 51                       push       ecx
004b98ba 6a00                     push       0
004b98bc 8d95e4feffff             lea        edx, [ebp - 0x11c]
004b98c2 52                       push       edx
004b98c3 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
004b98c9 e8a273f9ff               call       0x450c70
004b98ce 90                       nop        
004b98cf e9ce000000               jmp        0x4b99a2
004b98d4 837d08fe                 cmp        dword ptr [ebp + 8], -2
004b98d8 7534                     jne        0x4b990e
004b98da 6a00                     push       0
004b98dc e8bff5fbff               call       0x478ea0
004b98e1 83c404                   add        esp, 4
004b98e4 8bc8                     mov        ecx, eax
004b98e6 e895effcff               call       0x488880
004b98eb 85c0                     test       eax, eax
004b98ed 740e                     je         0x4b98fd
004b98ef 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
004b98f5 e8d6c20200               call       0x4e5bd0
004b98fa 90                       nop        
004b98fb eb0c                     jmp        0x4b9909
004b98fd 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004b9903 e8682c0000               call       0x4bc570
004b9908 90                       nop        
004b9909 e994000000               jmp        0x4b99a2
004b990e 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
004b9914 e847e8fbff               call       0x478160
004b9919 85c0                     test       eax, eax
004b991b 7438                     je         0x4b9955
004b991d 8b85f8feffff             mov        eax, dword ptr [ebp - 0x108]
004b9923 8b88bc010000             mov        ecx, dword ptr [eax + 0x1bc]
004b9929 51                       push       ecx
004b992a 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004b9930 e8fb4effff               call       0x4ae830
004b9935 8b95f8feffff             mov        edx, dword ptr [ebp - 0x108]
004b993b c782bc01000000000000     mov        dword ptr [edx + 0x1bc], 0
004b9945 8b85f8feffff             mov        eax, dword ptr [ebp - 0x108]
004b994b c780bc01000000000000     mov        dword ptr [eax + 0x1bc], 0
004b9955 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
004b995b 8b91c0010000             mov        edx, dword ptr [ecx + 0x1c0]
004b9961 8b4508                   mov        eax, dword ptr [ebp + 8]
004b9964 8b4cc204                 mov        ecx, dword ptr [edx + eax*8 + 4]
004b9968 8b95f8feffff             mov        edx, dword ptr [ebp - 0x108]
004b996e 038ac0010000             add        ecx, dword ptr [edx + 0x1c0]
004b9974 51                       push       ecx
004b9975 68f8065700               push       0x5706f8
004b997a 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004b9980 e8ab4fffff               call       0x4ae930
004b9985 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
004b998b 8981bc010000             mov        dword ptr [ecx + 0x1bc], eax
004b9991 8b95f8feffff             mov        edx, dword ptr [ebp - 0x108]
004b9997 8b82bc010000             mov        eax, dword ptr [edx + 0x1bc]
004b999d 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004b99a0 8908                     mov        dword ptr [eax], ecx
004b99a2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b99a5 33cd                     xor        ecx, ebp
004b99a7 e842900800               call       0x5429ee
004b99ac 8be5                     mov        esp, ebp
004b99ae 5d                       pop        ebp
004b99af c20400                   ret        4
004b99b2 cc                       int3       
004b99b3 cc                       int3       
004b99b4 cc                       int3       
004b99b5 cc                       int3       
004b99b6 cc                       int3       
004b99b7 cc                       int3       
004b99b8 cc                       int3       
004b99b9 cc                       int3       
004b99ba cc                       int3       
004b99bb cc                       int3       
004b99bc cc                       int3       
004b99bd cc                       int3       
004b99be cc                       int3       
004b99bf cc                       int3       
