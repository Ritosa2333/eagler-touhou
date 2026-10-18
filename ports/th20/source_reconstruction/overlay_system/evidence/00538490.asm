00538490 55                       push       ebp
00538491 8bec                     mov        ebp, esp
00538493 83ec0c                   sub        esp, 0xc
00538496 894dfc                   mov        dword ptr [ebp - 4], ecx
00538499 e88280ffff               call       0x530520
0053849e 0fb6c0                   movzx      eax, al
005384a1 85c0                     test       eax, eax
005384a3 753a                     jne        0x5384df
005384a5 6a00                     push       0
005384a7 e88483f2ff               call       0x460830
005384ac 83c404                   add        esp, 4
005384af 8bc8                     mov        ecx, eax
005384b1 e8fa71fcff               call       0x4ff6b0
005384b6 8945f4                   mov        dword ptr [ebp - 0xc], eax
005384b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005384bc 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
005384c0 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005384c3 8d4c020a                 lea        ecx, [edx + eax + 0xa]
005384c7 894df8                   mov        dword ptr [ebp - 8], ecx
005384ca 8b55f8                   mov        edx, dword ptr [ebp - 8]
005384cd 52                       push       edx
005384ce 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005384d1 50                       push       eax
005384d2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005384d5 51                       push       ecx
005384d6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005384d9 e862c8fcff               call       0x504d40
005384de 90                       nop        
005384df 8b5510                   mov        edx, dword ptr [ebp + 0x10]
005384e2 52                       push       edx
005384e3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005384e6 e835010000               call       0x538620
005384eb 90                       nop        
005384ec 8be5                     mov        esp, ebp
005384ee 5d                       pop        ebp
005384ef c20c00                   ret        0xc
005384f2 cc                       int3       
005384f3 cc                       int3       
005384f4 cc                       int3       
005384f5 cc                       int3       
005384f6 cc                       int3       
005384f7 cc                       int3       
005384f8 cc                       int3       
005384f9 cc                       int3       
005384fa cc                       int3       
005384fb cc                       int3       
005384fc cc                       int3       
005384fd cc                       int3       
005384fe cc                       int3       
005384ff cc                       int3       
