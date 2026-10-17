00531bb0 55                       push       ebp
00531bb1 8bec                     mov        ebp, esp
00531bb3 51                       push       ecx
00531bb4 894dfc                   mov        dword ptr [ebp - 4], ecx
00531bb7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531bba 83c05c                   add        eax, 0x5c
00531bbd 8be5                     mov        esp, ebp
00531bbf 5d                       pop        ebp
00531bc0 c3                       ret        
00531bc1 cc                       int3       
00531bc2 cc                       int3       
00531bc3 cc                       int3       
00531bc4 cc                       int3       
00531bc5 cc                       int3       
00531bc6 cc                       int3       
00531bc7 cc                       int3       
00531bc8 cc                       int3       
00531bc9 cc                       int3       
00531bca cc                       int3       
00531bcb cc                       int3       
00531bcc cc                       int3       
00531bcd cc                       int3       
00531bce cc                       int3       
00531bcf cc                       int3       
