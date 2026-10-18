00421890 55                       push       ebp
00421891 8bec                     mov        ebp, esp
00421893 83ec18                   sub        esp, 0x18
00421896 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0042189b 33c5                     xor        eax, ebp
0042189d 8945fc                   mov        dword ptr [ebp - 4], eax
004218a0 6a0f                     push       0xf
004218a2 b940025c00               mov        ecx, 0x5c0240
004218a7 e864aafeff               call       0x40c310
004218ac 50                       push       eax
004218ad 8d4df8                   lea        ecx, [ebp - 8]
004218b0 e87b9efeff               call       0x40b730
004218b5 90                       nop        
004218b6 8b4508                   mov        eax, dword ptr [ebp + 8]
004218b9 8945f4                   mov        dword ptr [ebp - 0xc], eax
004218bc 837df400                 cmp        dword ptr [ebp - 0xc], 0
004218c0 7428                     je         0x4218ea
004218c2 837df402                 cmp        dword ptr [ebp - 0xc], 2
004218c6 7402                     je         0x4218ca
004218c8 eb43                     jmp        0x42190d
004218ca 837d0c0c                 cmp        dword ptr [ebp + 0xc], 0xc
004218ce 7d1a                     jge        0x4218ea
004218d0 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004218d3 8b148db8e25a00           mov        edx, dword ptr [ecx*4 + 0x5ae2b8]
004218da 8955f0                   mov        dword ptr [ebp - 0x10], edx
004218dd 8d4df8                   lea        ecx, [ebp - 8]
004218e0 e81ba0feff               call       0x40b900
004218e5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004218e8 eb4d                     jmp        0x421937
004218ea 817d0c00010000           cmp        dword ptr [ebp + 0xc], 0x100
004218f1 7d1a                     jge        0x42190d
004218f3 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004218f6 8b0c85e8e25a00           mov        ecx, dword ptr [eax*4 + 0x5ae2e8]
004218fd 894dec                   mov        dword ptr [ebp - 0x14], ecx
00421900 8d4df8                   lea        ecx, [ebp - 8]
00421903 e8f89ffeff               call       0x40b900
00421908 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0042190b eb2a                     jmp        0x421937
0042190d 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00421910 52                       push       edx
00421911 6834d75600               push       0x56d734
00421916 6a0a                     push       0xa
00421918 68e09e5b00               push       0x5b9ee0
0042191d e8fed8ffff               call       0x41f220
00421922 83c410                   add        esp, 0x10
00421925 c745e8e09e5b00           mov        dword ptr [ebp - 0x18], 0x5b9ee0
0042192c 8d4df8                   lea        ecx, [ebp - 8]
0042192f e8cc9ffeff               call       0x40b900
00421934 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00421937 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042193a 33cd                     xor        ecx, ebp
0042193c e8ad101200               call       0x5429ee
00421941 8be5                     mov        esp, ebp
00421943 5d                       pop        ebp
00421944 c3                       ret        
00421945 cc                       int3       
00421946 cc                       int3       
00421947 cc                       int3       
00421948 cc                       int3       
00421949 cc                       int3       
0042194a cc                       int3       
0042194b cc                       int3       
0042194c cc                       int3       
0042194d cc                       int3       
0042194e cc                       int3       
0042194f cc                       int3       
