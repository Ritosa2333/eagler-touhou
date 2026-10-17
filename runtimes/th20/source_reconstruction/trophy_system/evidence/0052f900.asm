0052f900 55                       push       ebp
0052f901 8bec                     mov        ebp, esp
0052f903 51                       push       ecx
0052f904 8b4508                   mov        eax, dword ptr [ebp + 8]
0052f907 50                       push       eax
0052f908 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052f90e e85dd1ffff               call       0x52ca70
0052f913 85c0                     test       eax, eax
0052f915 7407                     je         0x52f91e
0052f917 33c0                     xor        eax, eax
0052f919 e983000000               jmp        0x52f9a1
0052f91e 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052f921 51                       push       ecx
0052f922 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
0052f928 e813ffffff               call       0x52f840
0052f92d 90                       nop        
0052f92e 833d58685c0000           cmp        dword ptr [0x5c6858], 0
0052f935 7416                     je         0x52f94d
0052f937 8b5508                   mov        edx, dword ptr [ebp + 8]
0052f93a 52                       push       edx
0052f93b 8b0d58685c00             mov        ecx, dword ptr [0x5c6858]
0052f941 e8bae5ffff               call       0x52df00
0052f946 a158685c00               mov        eax, dword ptr [0x5c6858]
0052f94b eb54                     jmp        0x52f9a1
0052f94d 6870575700               push       0x575770
0052f952 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052f958 e833ddffff               call       0x52d690
0052f95d 8945fc                   mov        dword ptr [ebp - 4], eax
0052f960 837dfc00                 cmp        dword ptr [ebp - 4], 0
0052f964 7504                     jne        0x52f96a
0052f966 33c0                     xor        eax, eax
0052f968 eb37                     jmp        0x52f9a1
0052f96a 8b4508                   mov        eax, dword ptr [ebp + 8]
0052f96d 50                       push       eax
0052f96e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052f971 e8fae7ffff               call       0x52e170
0052f976 85c0                     test       eax, eax
0052f978 7404                     je         0x52f97e
0052f97a eb07                     jmp        0x52f983
0052f97c eb05                     jmp        0x52f983
0052f97e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052f981 eb1e                     jmp        0x52f9a1
0052f983 837dfc00                 cmp        dword ptr [ebp - 4], 0
0052f987 7416                     je         0x52f99f
0052f989 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052f98c 51                       push       ecx
0052f98d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052f993 e828feeeff               call       0x41f7c0
0052f998 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052f99f 33c0                     xor        eax, eax
0052f9a1 8be5                     mov        esp, ebp
0052f9a3 5d                       pop        ebp
0052f9a4 c3                       ret        
0052f9a5 cc                       int3       
0052f9a6 cc                       int3       
0052f9a7 cc                       int3       
0052f9a8 cc                       int3       
0052f9a9 cc                       int3       
0052f9aa cc                       int3       
0052f9ab cc                       int3       
0052f9ac cc                       int3       
0052f9ad cc                       int3       
0052f9ae cc                       int3       
0052f9af cc                       int3       
