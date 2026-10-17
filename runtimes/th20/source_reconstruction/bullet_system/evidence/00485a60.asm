00485a60 55                       push       ebp
00485a61 8bec                     mov        ebp, esp
00485a63 83ec24                   sub        esp, 0x24
00485a66 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00485a6b 33c5                     xor        eax, ebp
00485a6d 8945fc                   mov        dword ptr [ebp - 4], eax
00485a70 8d4df0                   lea        ecx, [ebp - 0x10]
00485a73 e898d3f9ff               call       0x422e10
00485a78 90                       nop        
00485a79 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00485a7c 50                       push       eax
00485a7d 8d4ddc                   lea        ecx, [ebp - 0x24]
00485a80 51                       push       ecx
00485a81 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00485a84 e8b739faff               call       0x429440
00485a89 8b10                     mov        edx, dword ptr [eax]
00485a8b 8955f0                   mov        dword ptr [ebp - 0x10], edx
00485a8e 8b4804                   mov        ecx, dword ptr [eax + 4]
00485a91 894df4                   mov        dword ptr [ebp - 0xc], ecx
00485a94 8b5008                   mov        edx, dword ptr [eax + 8]
00485a97 8955f8                   mov        dword ptr [ebp - 8], edx
00485a9a 8d45f0                   lea        eax, [ebp - 0x10]
00485a9d 50                       push       eax
00485a9e e88d06fdff               call       0x456130
00485aa3 83c404                   add        esp, 4
00485aa6 d95de8                   fstp       dword ptr [ebp - 0x18]
00485aa9 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00485aae f30f594510               mulss      xmm0, dword ptr [ebp + 0x10]
00485ab3 0f2f45e8                 comiss     xmm0, dword ptr [ebp - 0x18]
00485ab7 7209                     jb         0x485ac2
00485ab9 c745ec01000000           mov        dword ptr [ebp - 0x14], 1
00485ac0 eb07                     jmp        0x485ac9
00485ac2 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00485ac9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00485acc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00485acf 33cd                     xor        ecx, ebp
00485ad1 e818cf0b00               call       0x5429ee
00485ad6 8be5                     mov        esp, ebp
00485ad8 5d                       pop        ebp
00485ad9 c3                       ret        
00485ada cc                       int3       
00485adb cc                       int3       
00485adc cc                       int3       
00485add cc                       int3       
00485ade cc                       int3       
00485adf cc                       int3       
