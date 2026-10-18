00531800 55                       push       ebp
00531801 8bec                     mov        ebp, esp
00531803 83ec0c                   sub        esp, 0xc
00531806 894dfc                   mov        dword ptr [ebp - 4], ecx
00531809 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053180c 0fb64834                 movzx      ecx, byte ptr [eax + 0x34]
00531810 85c9                     test       ecx, ecx
00531812 7536                     jne        0x53184a
00531814 6a00                     push       0
00531816 e815f0f2ff               call       0x460830
0053181b 83c404                   add        esp, 4
0053181e 8bc8                     mov        ecx, eax
00531820 e88bdefcff               call       0x4ff6b0
00531825 8945f4                   mov        dword ptr [ebp - 0xc], eax
00531828 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053182b 6b420414                 imul       eax, dword ptr [edx + 4], 0x14
0053182f 034510                   add        eax, dword ptr [ebp + 0x10]
00531832 8945f8                   mov        dword ptr [ebp - 8], eax
00531835 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00531838 51                       push       ecx
00531839 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0053183c 52                       push       edx
0053183d 8b4508                   mov        eax, dword ptr [ebp + 8]
00531840 50                       push       eax
00531841 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00531844 e8f734fdff               call       0x504d40
00531849 90                       nop        
0053184a 8be5                     mov        esp, ebp
0053184c 5d                       pop        ebp
0053184d c20c00                   ret        0xc
