00477860 55                       push       ebp
00477861 8bec                     mov        ebp, esp
00477863 51                       push       ecx
00477864 894dfc                   mov        dword ptr [ebp - 4], ecx
00477867 6a00                     push       0
00477869 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047786c 81c18c000000             add        ecx, 0x8c
00477872 e8a9bcfaff               call       0x423520
00477877 90                       nop        
00477878 8be5                     mov        esp, ebp
0047787a 5d                       pop        ebp
0047787b c3                       ret        
0047787c cc                       int3       
0047787d cc                       int3       
0047787e cc                       int3       
0047787f cc                       int3       
