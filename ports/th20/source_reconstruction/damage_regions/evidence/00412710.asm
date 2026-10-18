00412710 55                       push       ebp
00412711 8bec                     mov        ebp, esp
00412713 51                       push       ecx
00412714 894dfc                   mov        dword ptr [ebp - 4], ecx
00412717 8b45fc                   mov        eax, dword ptr [ebp - 4]
0041271a 8b4028                   mov        eax, dword ptr [eax + 0x28]
0041271d 8be5                     mov        esp, ebp
0041271f 5d                       pop        ebp
00412720 c3                       ret        
00412721 cc                       int3       
00412722 cc                       int3       
00412723 cc                       int3       
00412724 cc                       int3       
00412725 cc                       int3       
00412726 cc                       int3       
00412727 cc                       int3       
00412728 cc                       int3       
00412729 cc                       int3       
0041272a cc                       int3       
0041272b cc                       int3       
0041272c cc                       int3       
0041272d cc                       int3       
0041272e cc                       int3       
0041272f cc                       int3       
