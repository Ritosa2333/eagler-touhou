005077d0 55                       push       ebp
005077d1 8bec                     mov        ebp, esp
005077d3 83ec08                   sub        esp, 8
005077d6 894dfc                   mov        dword ptr [ebp - 4], ecx
005077d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005077dc c70000000000             mov        dword ptr [eax], 0
005077e2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005077e5 c7410400000000           mov        dword ptr [ecx + 4], 0
005077ec 8b55fc                   mov        edx, dword ptr [ebp - 4]
005077ef c7420800000000           mov        dword ptr [edx + 8], 0
005077f6 8b45fc                   mov        eax, dword ptr [ebp - 4]
005077f9 c7400c00000000           mov        dword ptr [eax + 0xc], 0
00507800 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507803 83c110                   add        ecx, 0x10
00507806 894df8                   mov        dword ptr [ebp - 8], ecx
00507809 6a00                     push       0
0050780b 6a00                     push       0
0050780d 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00507810 e8eb65f0ff               call       0x40de00
00507815 8b55fc                   mov        edx, dword ptr [ebp - 4]
00507818 c7421800000000           mov        dword ptr [edx + 0x18], 0
0050781f 6a50                     push       0x50
00507821 6a00                     push       0
00507823 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507826 83c01c                   add        eax, 0x1c
00507829 50                       push       eax
0050782a e831cf0300               call       0x544760
0050782f 83c40c                   add        esp, 0xc
00507832 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507835 83c170                   add        ecx, 0x70
00507838 e873faffff               call       0x5072b0
0050783d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00507840 81c198020000             add        ecx, 0x298
00507846 e815faffff               call       0x507260
0050784b 90                       nop        
0050784c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050784f 8be5                     mov        esp, ebp
00507851 5d                       pop        ebp
00507852 c3                       ret        
00507853 cc                       int3       
00507854 cc                       int3       
00507855 cc                       int3       
00507856 cc                       int3       
00507857 cc                       int3       
00507858 cc                       int3       
00507859 cc                       int3       
0050785a cc                       int3       
0050785b cc                       int3       
0050785c cc                       int3       
0050785d cc                       int3       
0050785e cc                       int3       
0050785f cc                       int3       
