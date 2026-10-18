0050a930 55                       push       ebp
0050a931 8bec                     mov        ebp, esp
0050a933 51                       push       ecx
0050a934 6828345700               push       0x573428
0050a939 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0050a93f e8bcc7ffff               call       0x507100
0050a944 8945fc                   mov        dword ptr [ebp - 4], eax
0050a947 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0050a94a 50                       push       eax
0050a94b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0050a94e 51                       push       ecx
0050a94f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a952 e829dbffff               call       0x508480
0050a957 85c0                     test       eax, eax
0050a959 7404                     je         0x50a95f
0050a95b eb10                     jmp        0x50a96d
0050a95d eb0e                     jmp        0x50a96d
0050a95f 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a962 8915fc605c00             mov        dword ptr [0x5c60fc], edx
0050a968 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a96b eb1e                     jmp        0x50a98b
0050a96d 837dfc00                 cmp        dword ptr [ebp - 4], 0
0050a971 7416                     je         0x50a989
0050a973 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a976 50                       push       eax
0050a977 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0050a97d e83e4ef1ff               call       0x41f7c0
0050a982 c745fc00000000           mov        dword ptr [ebp - 4], 0
0050a989 33c0                     xor        eax, eax
0050a98b 8be5                     mov        esp, ebp
0050a98d 5d                       pop        ebp
0050a98e c3                       ret        
0050a98f cc                       int3       
