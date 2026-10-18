0051c8b0 55                       push       ebp
0051c8b1 8bec                     mov        ebp, esp
0051c8b3 83ec08                   sub        esp, 8
0051c8b6 894dfc                   mov        dword ptr [ebp - 4], ecx
0051c8b9 8b4508                   mov        eax, dword ptr [ebp + 8]
0051c8bc 8945f8                   mov        dword ptr [ebp - 8], eax
0051c8bf 837df803                 cmp        dword ptr [ebp - 8], 3
0051c8c3 770a                     ja         0x51c8cf
0051c8c5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051c8c8 ff248d10c95100           jmp        dword ptr [ecx*4 + 0x51c910]
0051c8cf 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0051c8d2 52                       push       edx
0051c8d3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051c8d6 e8e564efff               call       0x412dc0
0051c8db 90                       nop        
0051c8dc eb2b                     jmp        0x51c909
0051c8de 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0051c8e1 50                       push       eax
0051c8e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051c8e5 e86615f0ff               call       0x41de50
0051c8ea 90                       nop        
0051c8eb eb1c                     jmp        0x51c909
0051c8ed 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0051c8f0 51                       push       ecx
0051c8f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051c8f4 e8a764efff               call       0x412da0
0051c8f9 90                       nop        
0051c8fa eb0d                     jmp        0x51c909
0051c8fc 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0051c8ff 52                       push       edx
0051c900 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051c903 e88815f0ff               call       0x41de90
0051c908 90                       nop        
0051c909 8be5                     mov        esp, ebp
0051c90b 5d                       pop        ebp
0051c90c c20800                   ret        8
0051c90f 90                       nop        
0051c910 cf                       iretd      
0051c911 c85100de                 enter      0x51, -0x22
0051c915 c85100ed                 enter      0x51, -0x13
0051c919 c85100fc                 enter      0x51, -4
