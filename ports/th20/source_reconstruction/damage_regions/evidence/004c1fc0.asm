004c1fc0 55                       push       ebp
004c1fc1 8bec                     mov        ebp, esp
004c1fc3 51                       push       ecx
004c1fc4 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1fc7 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1fca 83e001                   and        eax, 1
004c1fcd c1e004                   shl        eax, 4
004c1fd0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1fd3 8b9158030000             mov        edx, dword ptr [ecx + 0x358]
004c1fd9 83e2ef                   and        edx, 0xffffffef
004c1fdc 0bd0                     or         edx, eax
004c1fde 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1fe1 899058030000             mov        dword ptr [eax + 0x358], edx
004c1fe7 8be5                     mov        esp, ebp
004c1fe9 5d                       pop        ebp
004c1fea c20400                   ret        4
004c1fed cc                       int3       
004c1fee cc                       int3       
004c1fef cc                       int3       
