004e68a0 55                       push       ebp
004e68a1 8bec                     mov        ebp, esp
004e68a3 51                       push       ecx
004e68a4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e68a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e68aa 83c110                   add        ecx, 0x10
004e68ad e88e16fbff               call       0x497f40
004e68b2 90                       nop        
004e68b3 8be5                     mov        esp, ebp
004e68b5 5d                       pop        ebp
004e68b6 c3                       ret        
004e68b7 cc                       int3       
004e68b8 cc                       int3       
004e68b9 cc                       int3       
004e68ba cc                       int3       
004e68bb cc                       int3       
004e68bc cc                       int3       
004e68bd cc                       int3       
004e68be cc                       int3       
004e68bf cc                       int3       
