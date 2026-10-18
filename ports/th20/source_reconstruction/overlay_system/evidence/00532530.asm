00532530 55                       push       ebp
00532531 8bec                     mov        ebp, esp
00532533 83ec08                   sub        esp, 8
00532536 894df8                   mov        dword ptr [ebp - 8], ecx
00532539 6a00                     push       0
0053253b e8401bf3ff               call       0x464080
00532540 83c404                   add        esp, 4
00532543 8945fc                   mov        dword ptr [ebp - 4], eax
00532546 6a01                     push       1
00532548 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053254b e840c4f8ff               call       0x4be990
00532550 90                       nop        
00532551 8be5                     mov        esp, ebp
00532553 5d                       pop        ebp
00532554 c3                       ret        
00532555 cc                       int3       
00532556 cc                       int3       
00532557 cc                       int3       
00532558 cc                       int3       
00532559 cc                       int3       
0053255a cc                       int3       
0053255b cc                       int3       
0053255c cc                       int3       
0053255d cc                       int3       
0053255e cc                       int3       
0053255f cc                       int3       
