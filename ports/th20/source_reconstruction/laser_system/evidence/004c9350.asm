004c9350 55                       push       ebp
004c9351 8bec                     mov        ebp, esp
004c9353 51                       push       ecx
004c9354 894dfc                   mov        dword ptr [ebp - 4], ecx
004c9357 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
004c935b 7512                     jne        0x4c936f
004c935d 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c9360 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004c9363 83e1f9                   and        ecx, 0xfffffff9
004c9366 83c902                   or         ecx, 2
004c9369 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c936c 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004c936f 33c0                     xor        eax, eax
004c9371 8be5                     mov        esp, ebp
004c9373 5d                       pop        ebp
004c9374 c20800                   ret        8
004c9377 cc                       int3       
004c9378 cc                       int3       
004c9379 cc                       int3       
004c937a cc                       int3       
004c937b cc                       int3       
004c937c cc                       int3       
004c937d cc                       int3       
004c937e cc                       int3       
004c937f cc                       int3       
