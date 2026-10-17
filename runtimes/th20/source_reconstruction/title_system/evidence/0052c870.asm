0052c870 55                       push       ebp
0052c871 8bec                     mov        ebp, esp
0052c873 6aff                     push       -1
0052c875 688d805600               push       0x56808d
0052c87a 64a100000000             mov        eax, dword ptr fs:[0]
0052c880 50                       push       eax
0052c881 83ec14                   sub        esp, 0x14
0052c884 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c889 33c5                     xor        eax, ebp
0052c88b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c88e 50                       push       eax
0052c88f 8d45f4                   lea        eax, [ebp - 0xc]
0052c892 64a300000000             mov        dword ptr fs:[0], eax
0052c898 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052c89b 6a14                     push       0x14
0052c89d b940025c00               mov        ecx, 0x5c0240
0052c8a2 e869faedff               call       0x40c310
0052c8a7 50                       push       eax
0052c8a8 8d4dec                   lea        ecx, [ebp - 0x14]
0052c8ab e880eeedff               call       0x40b730
0052c8b0 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c8b7 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c8ba e8f176f3ff               call       0x463fb0
0052c8bf 90                       nop        
0052c8c0 6a00                     push       0
0052c8c2 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c8c5 50                       push       eax
0052c8c6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c8c9 e842f3feff               call       0x51bc10
0052c8ce 0fb6c8                   movzx      ecx, al
0052c8d1 85c9                     test       ecx, ecx
0052c8d3 0f85a0000000             jne        0x52c979
0052c8d9 6a01                     push       1
0052c8db 8b5508                   mov        edx, dword ptr [ebp + 8]
0052c8de 52                       push       edx
0052c8df 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c8e2 e829f3feff               call       0x51bc10
0052c8e7 0fb6c0                   movzx      eax, al
0052c8ea 85c0                     test       eax, eax
0052c8ec 0f8587000000             jne        0x52c979
0052c8f2 6a02                     push       2
0052c8f4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c8f7 51                       push       ecx
0052c8f8 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c8fb e810f3feff               call       0x51bc10
0052c900 0fb6d0                   movzx      edx, al
0052c903 85d2                     test       edx, edx
0052c905 7572                     jne        0x52c979
0052c907 6a03                     push       3
0052c909 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c90c 50                       push       eax
0052c90d 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c910 e8fbf2feff               call       0x51bc10
0052c915 0fb6c8                   movzx      ecx, al
0052c918 85c9                     test       ecx, ecx
0052c91a 755d                     jne        0x52c979
0052c91c 6a04                     push       4
0052c91e 8b5508                   mov        edx, dword ptr [ebp + 8]
0052c921 52                       push       edx
0052c922 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c925 e8e6f2feff               call       0x51bc10
0052c92a 0fb6c0                   movzx      eax, al
0052c92d 85c0                     test       eax, eax
0052c92f 7548                     jne        0x52c979
0052c931 6a05                     push       5
0052c933 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c936 51                       push       ecx
0052c937 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c93a e8d1f2feff               call       0x51bc10
0052c93f 0fb6d0                   movzx      edx, al
0052c942 85d2                     test       edx, edx
0052c944 7533                     jne        0x52c979
0052c946 6a06                     push       6
0052c948 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c94b 50                       push       eax
0052c94c 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c94f e8bcf2feff               call       0x51bc10
0052c954 0fb6c8                   movzx      ecx, al
0052c957 85c9                     test       ecx, ecx
0052c959 751e                     jne        0x52c979
0052c95b 6a07                     push       7
0052c95d 8b5508                   mov        edx, dword ptr [ebp + 8]
0052c960 52                       push       edx
0052c961 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c964 e8a7f2feff               call       0x51bc10
0052c969 0fb6c0                   movzx      eax, al
0052c96c 85c0                     test       eax, eax
0052c96e 7509                     jne        0x52c979
0052c970 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0052c977 eb07                     jmp        0x52c980
0052c979 c745e001000000           mov        dword ptr [ebp - 0x20], 1
0052c980 8a4de0                   mov        cl, byte ptr [ebp - 0x20]
0052c983 884de7                   mov        byte ptr [ebp - 0x19], cl
0052c986 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c98d 8d4dec                   lea        ecx, [ebp - 0x14]
0052c990 e86befedff               call       0x40b900
0052c995 0fb645e7                 movzx      eax, byte ptr [ebp - 0x19]
0052c999 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c99c 64890d00000000           mov        dword ptr fs:[0], ecx
0052c9a3 59                       pop        ecx
0052c9a4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c9a7 33cd                     xor        ecx, ebp
0052c9a9 e840600100               call       0x5429ee
0052c9ae 8be5                     mov        esp, ebp
0052c9b0 5d                       pop        ebp
0052c9b1 c20400                   ret        4
0052c9b4 cc                       int3       
0052c9b5 cc                       int3       
0052c9b6 cc                       int3       
0052c9b7 cc                       int3       
0052c9b8 cc                       int3       
0052c9b9 cc                       int3       
0052c9ba cc                       int3       
0052c9bb cc                       int3       
0052c9bc cc                       int3       
0052c9bd cc                       int3       
0052c9be cc                       int3       
0052c9bf cc                       int3       
