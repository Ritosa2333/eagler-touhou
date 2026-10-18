0052ffd0 55                       push       ebp
0052ffd1 8bec                     mov        ebp, esp
0052ffd3 51                       push       ecx
0052ffd4 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ffd7 e884420000               call       0x534260
0052ffdc 50                       push       eax
0052ffdd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052ffe0 83c114                   add        ecx, 0x14
0052ffe3 e83835efff               call       0x423520
0052ffe8 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ffeb c6403401                 mov        byte ptr [eax + 0x34], 1
0052ffef 6a00                     push       0
0052fff1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fff4 83c124                   add        ecx, 0x24
0052fff7 e82435efff               call       0x423520
0052fffc 90                       nop        
0052fffd 8be5                     mov        esp, ebp
0052ffff 5d                       pop        ebp
00530000 c3                       ret        
00530001 cc                       int3       
00530002 cc                       int3       
00530003 cc                       int3       
00530004 cc                       int3       
00530005 cc                       int3       
00530006 cc                       int3       
00530007 cc                       int3       
00530008 cc                       int3       
00530009 cc                       int3       
0053000a cc                       int3       
0053000b cc                       int3       
0053000c cc                       int3       
0053000d cc                       int3       
0053000e cc                       int3       
0053000f cc                       int3       
