00512f30 55                       push       ebp
00512f31 8bec                     mov        ebp, esp
00512f33 51                       push       ecx
00512f34 894dfc                   mov        dword ptr [ebp - 4], ecx
00512f37 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00512f3a e8a1edefff               call       0x411ce0
00512f3f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00512f42 83c118                   add        ecx, 0x18
00512f45 e886cdf3ff               call       0x44fcd0
00512f4a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00512f4d 50                       push       eax
00512f4e 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00512f54 e8170cf0ff               call       0x413b70
00512f59 90                       nop        
00512f5a 8be5                     mov        esp, ebp
00512f5c 5d                       pop        ebp
00512f5d c3                       ret        
00512f5e cc                       int3       
00512f5f cc                       int3       
