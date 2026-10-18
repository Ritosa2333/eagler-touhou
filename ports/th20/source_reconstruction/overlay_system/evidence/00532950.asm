00532950 55                       push       ebp
00532951 8bec                     mov        ebp, esp
00532953 51                       push       ecx
00532954 894dfc                   mov        dword ptr [ebp - 4], ecx
00532957 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053295a c70010585700             mov        dword ptr [eax], 0x575810
00532960 8be5                     mov        esp, ebp
00532962 5d                       pop        ebp
00532963 c3                       ret        
00532964 cc                       int3       
00532965 cc                       int3       
00532966 cc                       int3       
00532967 cc                       int3       
00532968 cc                       int3       
00532969 cc                       int3       
0053296a cc                       int3       
0053296b cc                       int3       
0053296c cc                       int3       
0053296d cc                       int3       
0053296e cc                       int3       
0053296f cc                       int3       
