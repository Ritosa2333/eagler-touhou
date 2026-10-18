004d9940 55                       push       ebp
004d9941 8bec                     mov        ebp, esp
004d9943 83ec10                   sub        esp, 0x10
004d9946 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004d994b 33c5                     xor        eax, ebp
004d994d 8945fc                   mov        dword ptr [ebp - 4], eax
004d9950 894df4                   mov        dword ptr [ebp - 0xc], ecx
004d9953 6a0a                     push       0xa
004d9955 b940025c00               mov        ecx, 0x5c0240
004d995a e8b129f3ff               call       0x40c310
004d995f 50                       push       eax
004d9960 8d4df8                   lea        ecx, [ebp - 8]
004d9963 e8c81df3ff               call       0x40b730
004d9968 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004d996b c7400800000000           mov        dword ptr [eax + 8], 0
004d9972 e86978f3ff               call       0x4111e0
004d9977 d1e8                     shr        eax, 1
004d9979 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d997c 89410c                   mov        dword ptr [ecx + 0xc], eax
004d997f 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d9982 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004d9985 8b4a0c                   mov        ecx, dword ptr [edx + 0xc]
004d9988 2b4808                   sub        ecx, dword ptr [eax + 8]
004d998b 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d998e 894a10                   mov        dword ptr [edx + 0x10], ecx
004d9991 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004d9994 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d9997 894814                   mov        dword ptr [eax + 0x14], ecx
004d999a 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d999d 83c204                   add        edx, 4
004d99a0 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d99a3 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004d99a6 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004d99a9 51                       push       ecx
004d99aa 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d99ad e85e400000               call       0x4dda10
004d99b2 8d4df8                   lea        ecx, [ebp - 8]
004d99b5 e8461ff3ff               call       0x40b900
004d99ba 90                       nop        
004d99bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d99be 33cd                     xor        ecx, ebp
004d99c0 e829900600               call       0x5429ee
004d99c5 8be5                     mov        esp, ebp
004d99c7 5d                       pop        ebp
004d99c8 c20400                   ret        4
004d99cb cc                       int3       
004d99cc cc                       int3       
004d99cd cc                       int3       
004d99ce cc                       int3       
004d99cf cc                       int3       
