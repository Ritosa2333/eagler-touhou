004c5870 55                       push       ebp
004c5871 8bec                     mov        ebp, esp
004c5873 83ec08                   sub        esp, 8
004c5876 894dfc                   mov        dword ptr [ebp - 4], ecx
004c5879 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c587c 8b88fc000000             mov        ecx, dword ptr [eax + 0xfc]
004c5882 894df8                   mov        dword ptr [ebp - 8], ecx
004c5885 837df801                 cmp        dword ptr [ebp - 8], 1
004c5889 740a                     je         0x4c5895
004c588b 837df802                 cmp        dword ptr [ebp - 8], 2
004c588f 740f                     je         0x4c58a0
004c5891 eb16                     jmp        0x4c58a9
004c5893 eb14                     jmp        0x4c58a9
004c5895 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c5898 e8d31e0000               call       0x4c7770
004c589d 90                       nop        
004c589e eb09                     jmp        0x4c58a9
004c58a0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c58a3 e888120000               call       0x4c6b30
004c58a8 90                       nop        
004c58a9 6a00                     push       0
004c58ab 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c58b1 e84a32fcff               call       0x488b00
004c58b6 6aff                     push       -1
004c58b8 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004c58be e80db2faff               call       0x470ad0
004c58c3 b801000000               mov        eax, 1
004c58c8 8be5                     mov        esp, ebp
004c58ca 5d                       pop        ebp
004c58cb c3                       ret        
004c58cc cc                       int3       
004c58cd cc                       int3       
004c58ce cc                       int3       
004c58cf cc                       int3       
