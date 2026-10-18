00479040 55                       push       ebp
00479041 8bec                     mov        ebp, esp
00479043 51                       push       ecx
00479044 894dfc                   mov        dword ptr [ebp - 4], ecx
00479047 6a01                     push       1
00479049 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047904c e83f5ffdff               call       0x44ef90
00479051 90                       nop        
00479052 8be5                     mov        esp, ebp
00479054 5d                       pop        ebp
00479055 c3                       ret        
00479056 cc                       int3       
00479057 cc                       int3       
00479058 cc                       int3       
00479059 cc                       int3       
0047905a cc                       int3       
0047905b cc                       int3       
0047905c cc                       int3       
0047905d cc                       int3       
0047905e cc                       int3       
0047905f cc                       int3       
