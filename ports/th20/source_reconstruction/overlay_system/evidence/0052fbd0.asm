0052fbd0 55                       push       ebp
0052fbd1 8bec                     mov        ebp, esp
0052fbd3 83ec14                   sub        esp, 0x14
0052fbd6 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fbd9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052fbdc 0fb64834                 movzx      ecx, byte ptr [eax + 0x34]
0052fbe0 85c9                     test       ecx, ecx
0052fbe2 7549                     jne        0x52fc2d
0052fbe4 6a00                     push       0
0052fbe6 e84546f3ff               call       0x464230
0052fbeb 83c404                   add        esp, 4
0052fbee 8bc8                     mov        ecx, eax
0052fbf0 e88b090000               call       0x530580
0052fbf5 6a00                     push       0
0052fbf7 e8340cf3ff               call       0x460830
0052fbfc 83c404                   add        esp, 4
0052fbff 8bc8                     mov        ecx, eax
0052fc01 e8aafafcff               call       0x4ff6b0
0052fc06 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052fc09 8b55fc                   mov        edx, dword ptr [ebp - 4]
0052fc0c 6b420414                 imul       eax, dword ptr [edx + 4], 0x14
0052fc10 034510                   add        eax, dword ptr [ebp + 0x10]
0052fc13 8945f8                   mov        dword ptr [ebp - 8], eax
0052fc16 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052fc19 51                       push       ecx
0052fc1a 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0052fc1d 52                       push       edx
0052fc1e 8b4508                   mov        eax, dword ptr [ebp + 8]
0052fc21 50                       push       eax
0052fc22 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052fc25 e81651fdff               call       0x504d40
0052fc2a 90                       nop        
0052fc2b eb4b                     jmp        0x52fc78
0052fc2d 6a00                     push       0
0052fc2f e8fc45f3ff               call       0x464230
0052fc34 83c404                   add        esp, 4
0052fc37 8bc8                     mov        ecx, eax
0052fc39 e862090000               call       0x5305a0
0052fc3e 6a00                     push       0
0052fc40 e8eb0bf3ff               call       0x460830
0052fc45 83c404                   add        esp, 4
0052fc48 8bc8                     mov        ecx, eax
0052fc4a e861fafcff               call       0x4ff6b0
0052fc4f 8945ec                   mov        dword ptr [ebp - 0x14], eax
0052fc52 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fc55 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
0052fc59 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0052fc5c 8d4c020f                 lea        ecx, [edx + eax + 0xf]
0052fc60 894df0                   mov        dword ptr [ebp - 0x10], ecx
0052fc63 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0052fc66 52                       push       edx
0052fc67 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052fc6a 50                       push       eax
0052fc6b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052fc6e 51                       push       ecx
0052fc6f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0052fc72 e8c950fdff               call       0x504d40
0052fc77 90                       nop        
0052fc78 8be5                     mov        esp, ebp
0052fc7a 5d                       pop        ebp
0052fc7b c20c00                   ret        0xc
0052fc7e cc                       int3       
0052fc7f cc                       int3       
