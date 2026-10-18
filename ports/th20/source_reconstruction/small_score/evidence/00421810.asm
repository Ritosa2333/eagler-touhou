00421810 55                       push       ebp
00421811 8bec                     mov        ebp, esp
00421813 51                       push       ecx
00421814 894dfc                   mov        dword ptr [ebp - 4], ecx
00421817 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042181a 8b401c                   mov        eax, dword ptr [eax + 0x1c]
0042181d 8be5                     mov        esp, ebp
0042181f 5d                       pop        ebp
00421820 c3                       ret        
00421821 cc                       int3       
00421822 cc                       int3       
00421823 cc                       int3       
00421824 cc                       int3       
00421825 cc                       int3       
00421826 cc                       int3       
00421827 cc                       int3       
00421828 cc                       int3       
00421829 cc                       int3       
0042182a cc                       int3       
0042182b cc                       int3       
0042182c cc                       int3       
0042182d cc                       int3       
0042182e cc                       int3       
0042182f cc                       int3       
