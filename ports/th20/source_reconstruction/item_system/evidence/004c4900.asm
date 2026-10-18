004c4900 55                       push       ebp
004c4901 8bec                     mov        ebp, esp
004c4903 51                       push       ecx
004c4904 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4907 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c490a d98088200000             fld        dword ptr [eax + 0x2088]
004c4910 8be5                     mov        esp, ebp
004c4912 5d                       pop        ebp
004c4913 c3                       ret        
004c4914 cc                       int3       
004c4915 cc                       int3       
004c4916 cc                       int3       
004c4917 cc                       int3       
004c4918 cc                       int3       
004c4919 cc                       int3       
004c491a cc                       int3       
004c491b cc                       int3       
004c491c cc                       int3       
004c491d cc                       int3       
004c491e cc                       int3       
004c491f cc                       int3       
