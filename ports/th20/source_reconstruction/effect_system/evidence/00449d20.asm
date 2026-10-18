00449d20 55                       push       ebp
00449d21 8bec                     mov        ebp, esp
00449d23 83ec10                   sub        esp, 0x10
00449d26 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00449d2b 33c5                     xor        eax, ebp
00449d2d 8945fc                   mov        dword ptr [ebp - 4], eax
00449d30 894df0                   mov        dword ptr [ebp - 0x10], ecx
00449d33 6a0a                     push       0xa
00449d35 b940025c00               mov        ecx, 0x5c0240
00449d3a e8d125fcff               call       0x40c310
00449d3f 50                       push       eax
00449d40 8d4df8                   lea        ecx, [ebp - 8]
00449d43 e8e819fcff               call       0x40b730
00449d48 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00449d4b 83c104                   add        ecx, 4
00449d4e e89d98fdff               call       0x4235f0
00449d53 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00449d56 894114                   mov        dword ptr [ecx + 0x14], eax
00449d59 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00449d5c 8a4214                   mov        al, byte ptr [edx + 0x14]
00449d5f 8845f7                   mov        byte ptr [ebp - 9], al
00449d62 8d4df8                   lea        ecx, [ebp - 8]
00449d65 e8961bfcff               call       0x40b900
00449d6a 0fb645f7                 movzx      eax, byte ptr [ebp - 9]
00449d6e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00449d71 33cd                     xor        ecx, ebp
00449d73 e8768c0f00               call       0x5429ee
00449d78 8be5                     mov        esp, ebp
00449d7a 5d                       pop        ebp
00449d7b c3                       ret        
00449d7c cc                       int3       
00449d7d cc                       int3       
00449d7e cc                       int3       
00449d7f cc                       int3       
