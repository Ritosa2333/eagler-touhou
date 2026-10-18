004d47f0 55                       push       ebp
004d47f1 8bec                     mov        ebp, esp
004d47f3 83ec28                   sub        esp, 0x28
004d47f6 894dfc                   mov        dword ptr [ebp - 4], ecx
004d47f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d47fc 81782800020000           cmp        dword ptr [eax + 0x28], 0x200
004d4803 7c07                     jl         0x4d480c
004d4805 33c0                     xor        eax, eax
004d4807 e9b8010000               jmp        0x4d49c4
004d480c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d480f 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
004d4812 83c201                   add        edx, 1
004d4815 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d4818 89502c                   mov        dword ptr [eax + 0x2c], edx
004d481b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d481e 81792c00000100           cmp        dword ptr [ecx + 0x2c], 0x10000
004d4825 7d0a                     jge        0x4d4831
004d4827 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d482a c7422c00000100           mov        dword ptr [edx + 0x2c], 0x10000
004d4831 8b4508                   mov        eax, dword ptr [ebp + 8]
004d4834 8945e8                   mov        dword ptr [ebp - 0x18], eax
004d4837 837de803                 cmp        dword ptr [ebp - 0x18], 3
004d483b 0f877d010000             ja         0x4d49be
004d4841 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004d4844 ff248dcc494d00           jmp        dword ptr [ecx*4 + 0x4d49cc]
004d484b 6878145700               push       0x571478
004d4850 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d4856 e83539ffff               call       0x4c8190
004d485b 8945f8                   mov        dword ptr [ebp - 8], eax
004d485e 837df800                 cmp        dword ptr [ebp - 8], 0
004d4862 7507                     jne        0x4d486b
004d4864 33c0                     xor        eax, eax
004d4866 e959010000               jmp        0x4d49c4
004d486b 8b55f8                   mov        edx, dword ptr [ebp - 8]
004d486e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d4871 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
004d4874 898a8c000000             mov        dword ptr [edx + 0x8c], ecx
004d487a 8b55f8                   mov        edx, dword ptr [ebp - 8]
004d487d 8b02                     mov        eax, dword ptr [edx]
004d487f 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004d4882 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004d4885 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004d4888 52                       push       edx
004d4889 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d488c ff55e4                   call       dword ptr [ebp - 0x1c]
004d488f 85c0                     test       eax, eax
004d4891 7d07                     jge        0x4d489a
004d4893 33c0                     xor        eax, eax
004d4895 e92a010000               jmp        0x4d49c4
004d489a 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d489d 50                       push       eax
004d489e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d48a1 e85a4affff               call       0x4c9300
004d48a6 90                       nop        
004d48a7 e912010000               jmp        0x4d49be
004d48ac 68c0145700               push       0x5714c0
004d48b1 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d48b7 e87438ffff               call       0x4c8130
004d48bc 8945f4                   mov        dword ptr [ebp - 0xc], eax
004d48bf 837df400                 cmp        dword ptr [ebp - 0xc], 0
004d48c3 7507                     jne        0x4d48cc
004d48c5 33c0                     xor        eax, eax
004d48c7 e9f8000000               jmp        0x4d49c4
004d48cc 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d48cf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d48d2 8b422c                   mov        eax, dword ptr [edx + 0x2c]
004d48d5 89818c000000             mov        dword ptr [ecx + 0x8c], eax
004d48db 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d48de 8b11                     mov        edx, dword ptr [ecx]
004d48e0 8b4214                   mov        eax, dword ptr [edx + 0x14]
004d48e3 8945e0                   mov        dword ptr [ebp - 0x20], eax
004d48e6 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004d48e9 51                       push       ecx
004d48ea 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d48ed ff55e0                   call       dword ptr [ebp - 0x20]
004d48f0 85c0                     test       eax, eax
004d48f2 7d07                     jge        0x4d48fb
004d48f4 33c0                     xor        eax, eax
004d48f6 e9c9000000               jmp        0x4d49c4
004d48fb 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d48fe 52                       push       edx
004d48ff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d4902 e8f949ffff               call       0x4c9300
004d4907 90                       nop        
004d4908 e9b1000000               jmp        0x4d49be
004d490d 6808155700               push       0x571508
004d4912 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d4918 e80337ffff               call       0x4c8020
004d491d 8945f0                   mov        dword ptr [ebp - 0x10], eax
004d4920 837df000                 cmp        dword ptr [ebp - 0x10], 0
004d4924 7507                     jne        0x4d492d
004d4926 33c0                     xor        eax, eax
004d4928 e997000000               jmp        0x4d49c4
004d492d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004d4930 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d4933 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
004d4936 89908c000000             mov        dword ptr [eax + 0x8c], edx
004d493c 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004d493f 8b08                     mov        ecx, dword ptr [eax]
004d4941 8b5114                   mov        edx, dword ptr [ecx + 0x14]
004d4944 8955dc                   mov        dword ptr [ebp - 0x24], edx
004d4947 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004d494a 50                       push       eax
004d494b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d494e ff55dc                   call       dword ptr [ebp - 0x24]
004d4951 85c0                     test       eax, eax
004d4953 7d04                     jge        0x4d4959
004d4955 33c0                     xor        eax, eax
004d4957 eb6b                     jmp        0x4d49c4
004d4959 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d495c 51                       push       ecx
004d495d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d4960 e89b49ffff               call       0x4c9300
004d4965 90                       nop        
004d4966 eb56                     jmp        0x4d49be
004d4968 6850155700               push       0x571550
004d496d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d4973 e80837ffff               call       0x4c8080
004d4978 8945ec                   mov        dword ptr [ebp - 0x14], eax
004d497b 837dec00                 cmp        dword ptr [ebp - 0x14], 0
004d497f 7504                     jne        0x4d4985
004d4981 33c0                     xor        eax, eax
004d4983 eb3f                     jmp        0x4d49c4
004d4985 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d4988 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d498b 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
004d498e 898a8c000000             mov        dword ptr [edx + 0x8c], ecx
004d4994 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004d4997 8b02                     mov        eax, dword ptr [edx]
004d4999 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004d499c 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004d499f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004d49a2 52                       push       edx
004d49a3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004d49a6 ff55d8                   call       dword ptr [ebp - 0x28]
004d49a9 85c0                     test       eax, eax
004d49ab 7d04                     jge        0x4d49b1
004d49ad 33c0                     xor        eax, eax
004d49af eb13                     jmp        0x4d49c4
004d49b1 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004d49b4 50                       push       eax
004d49b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d49b8 e84349ffff               call       0x4c9300
004d49bd 90                       nop        
004d49be 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d49c1 8b412c                   mov        eax, dword ptr [ecx + 0x2c]
004d49c4 8be5                     mov        esp, ebp
004d49c6 5d                       pop        ebp
004d49c7 c20800                   ret        8
004d49ca 6690                     nop        
004d49cc 4b                       dec        ebx
004d49cd 48                       dec        eax
004d49ce 4d                       dec        ebp
004d49cf 00ac484d006849           add        byte ptr [eax + ecx*2 + 0x4968004d], ch
004d49d6 4d                       dec        ebp
004d49d7 000d494d00cc             add        byte ptr [0xcc004d49], cl
004d49dd cc                       int3       
004d49de cc                       int3       
004d49df cc                       int3       
