00424010 55                       push       ebp
00424011 8bec                     mov        ebp, esp
00424013 51                       push       ecx
00424014 894dfc                   mov        dword ptr [ebp - 4], ecx
00424017 8b45fc                   mov        eax, dword ptr [ebp - 4]
0042401a 8b4018                   mov        eax, dword ptr [eax + 0x18]
0042401d 8be5                     mov        esp, ebp
0042401f 5d                       pop        ebp
00424020 c3                       ret        
00424021 cc                       int3       
00424022 cc                       int3       
00424023 cc                       int3       
00424024 cc                       int3       
00424025 cc                       int3       
00424026 cc                       int3       
00424027 cc                       int3       
00424028 cc                       int3       
00424029 cc                       int3       
0042402a cc                       int3       
0042402b cc                       int3       
0042402c cc                       int3       
0042402d cc                       int3       
0042402e cc                       int3       
0042402f cc                       int3       
