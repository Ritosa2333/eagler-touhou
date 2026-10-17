004c4920 55                       push       ebp
004c4921 8bec                     mov        ebp, esp
004c4923 51                       push       ecx
004c4924 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4927 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c492a d98084200000             fld        dword ptr [eax + 0x2084]
004c4930 8be5                     mov        esp, ebp
004c4932 5d                       pop        ebp
004c4933 c3                       ret        
004c4934 cc                       int3       
004c4935 cc                       int3       
004c4936 cc                       int3       
004c4937 cc                       int3       
004c4938 cc                       int3       
004c4939 cc                       int3       
004c493a cc                       int3       
004c493b cc                       int3       
004c493c cc                       int3       
004c493d cc                       int3       
004c493e cc                       int3       
004c493f cc                       int3       
