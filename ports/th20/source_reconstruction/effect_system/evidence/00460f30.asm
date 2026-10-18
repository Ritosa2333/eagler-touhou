00460f30 55                       push       ebp
00460f31 8bec                     mov        ebp, esp
00460f33 51                       push       ecx
00460f34 8b4508                   mov        eax, dword ptr [ebp + 8]
00460f37 50                       push       eax
00460f38 6848ef5600               push       0x56ef48
00460f3d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00460f43 e828f9ffff               call       0x460870
00460f48 8945fc                   mov        dword ptr [ebp - 4], eax
00460f4b 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00460f4e 51                       push       ecx
00460f4f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00460f52 52                       push       edx
00460f53 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00460f56 e805fcffff               call       0x460b60
00460f5b 8be5                     mov        esp, ebp
00460f5d 5d                       pop        ebp
00460f5e c3                       ret        
00460f5f cc                       int3       
