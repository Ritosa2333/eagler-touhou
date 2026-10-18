004778e0 55                       push       ebp
004778e1 8bec                     mov        ebp, esp
004778e3 51                       push       ecx
004778e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004778e7 6a3c                     push       0x3c
004778e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004778ec 81c148330000             add        ecx, 0x3348
004778f2 e829bcfaff               call       0x423520
004778f7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004778fa 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00477900 83c904                   or         ecx, 4
00477903 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477906 898a44330000             mov        dword ptr [edx + 0x3344], ecx
0047790c 8be5                     mov        esp, ebp
0047790e 5d                       pop        ebp
0047790f c3                       ret        
