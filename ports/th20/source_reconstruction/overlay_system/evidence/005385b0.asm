005385b0 55                       push       ebp
005385b1 8bec                     mov        ebp, esp
005385b3 83ec0c                   sub        esp, 0xc
005385b6 894dfc                   mov        dword ptr [ebp - 4], ecx
005385b9 e8627fffff               call       0x530520
005385be 0fb6c0                   movzx      eax, al
005385c1 85c0                     test       eax, eax
005385c3 753a                     jne        0x5385ff
005385c5 6a00                     push       0
005385c7 e86482f2ff               call       0x460830
005385cc 83c404                   add        esp, 4
005385cf 8bc8                     mov        ecx, eax
005385d1 e8da70fcff               call       0x4ff6b0
005385d6 8945f4                   mov        dword ptr [ebp - 0xc], eax
005385d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005385dc 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
005385e0 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005385e3 8d4c0205                 lea        ecx, [edx + eax + 5]
005385e7 894df8                   mov        dword ptr [ebp - 8], ecx
005385ea 8b55f8                   mov        edx, dword ptr [ebp - 8]
005385ed 52                       push       edx
005385ee 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005385f1 50                       push       eax
005385f2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005385f5 51                       push       ecx
005385f6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005385f9 e842c7fcff               call       0x504d40
005385fe 90                       nop        
005385ff 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00538602 52                       push       edx
00538603 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538606 e815000000               call       0x538620
0053860b 90                       nop        
0053860c 8be5                     mov        esp, ebp
0053860e 5d                       pop        ebp
0053860f c20c00                   ret        0xc
00538612 cc                       int3       
00538613 cc                       int3       
00538614 cc                       int3       
00538615 cc                       int3       
00538616 cc                       int3       
00538617 cc                       int3       
00538618 cc                       int3       
00538619 cc                       int3       
0053861a cc                       int3       
0053861b cc                       int3       
0053861c cc                       int3       
0053861d cc                       int3       
0053861e cc                       int3       
0053861f cc                       int3       
