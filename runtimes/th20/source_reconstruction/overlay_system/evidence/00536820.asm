00536820 55                       push       ebp
00536821 8bec                     mov        ebp, esp
00536823 51                       push       ecx
00536824 894dfc                   mov        dword ptr [ebp - 4], ecx
00536827 e834daffff               call       0x534260
0053682c 50                       push       eax
0053682d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00536830 83c114                   add        ecx, 0x14
00536833 e8e8cceeff               call       0x423520
00536838 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053683b c6403401                 mov        byte ptr [eax + 0x34], 1
0053683f 8be5                     mov        esp, ebp
00536841 5d                       pop        ebp
00536842 c3                       ret        
00536843 cc                       int3       
00536844 cc                       int3       
00536845 cc                       int3       
00536846 cc                       int3       
00536847 cc                       int3       
00536848 cc                       int3       
00536849 cc                       int3       
0053684a cc                       int3       
0053684b cc                       int3       
0053684c cc                       int3       
0053684d cc                       int3       
0053684e cc                       int3       
0053684f cc                       int3       
