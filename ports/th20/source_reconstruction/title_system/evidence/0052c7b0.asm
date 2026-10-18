0052c7b0 55                       push       ebp
0052c7b1 8bec                     mov        ebp, esp
0052c7b3 6aff                     push       -1
0052c7b5 688d805600               push       0x56808d
0052c7ba 64a100000000             mov        eax, dword ptr fs:[0]
0052c7c0 50                       push       eax
0052c7c1 83ec14                   sub        esp, 0x14
0052c7c4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c7c9 33c5                     xor        eax, ebp
0052c7cb 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c7ce 50                       push       eax
0052c7cf 8d45f4                   lea        eax, [ebp - 0xc]
0052c7d2 64a300000000             mov        dword ptr fs:[0], eax
0052c7d8 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0052c7db 6a14                     push       0x14
0052c7dd b940025c00               mov        ecx, 0x5c0240
0052c7e2 e829fbedff               call       0x40c310
0052c7e7 50                       push       eax
0052c7e8 8d4dec                   lea        ecx, [ebp - 0x14]
0052c7eb e840efedff               call       0x40b730
0052c7f0 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c7f7 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c7fa e8b177f3ff               call       0x463fb0
0052c7ff 90                       nop        
0052c800 6a00                     push       0
0052c802 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c805 e866000000               call       0x52c870
0052c80a 0fb6c0                   movzx      eax, al
0052c80d 85c0                     test       eax, eax
0052c80f 751a                     jne        0x52c82b
0052c811 6a01                     push       1
0052c813 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c816 e855000000               call       0x52c870
0052c81b 0fb6c8                   movzx      ecx, al
0052c81e 85c9                     test       ecx, ecx
0052c820 7509                     jne        0x52c82b
0052c822 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0052c829 eb07                     jmp        0x52c832
0052c82b c745e001000000           mov        dword ptr [ebp - 0x20], 1
0052c832 8a55e0                   mov        dl, byte ptr [ebp - 0x20]
0052c835 8855eb                   mov        byte ptr [ebp - 0x15], dl
0052c838 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c83f 8d4dec                   lea        ecx, [ebp - 0x14]
0052c842 e8b9f0edff               call       0x40b900
0052c847 0fb645eb                 movzx      eax, byte ptr [ebp - 0x15]
0052c84b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c84e 64890d00000000           mov        dword ptr fs:[0], ecx
0052c855 59                       pop        ecx
0052c856 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c859 33cd                     xor        ecx, ebp
0052c85b e88e610100               call       0x5429ee
0052c860 8be5                     mov        esp, ebp
0052c862 5d                       pop        ebp
0052c863 c3                       ret        
0052c864 cc                       int3       
0052c865 cc                       int3       
0052c866 cc                       int3       
0052c867 cc                       int3       
0052c868 cc                       int3       
0052c869 cc                       int3       
0052c86a cc                       int3       
0052c86b cc                       int3       
0052c86c cc                       int3       
0052c86d cc                       int3       
0052c86e cc                       int3       
0052c86f cc                       int3       
