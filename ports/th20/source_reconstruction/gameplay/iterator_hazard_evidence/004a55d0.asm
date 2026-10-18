004a55d0 55                       push       ebp
004a55d1 8bec                     mov        ebp, esp
004a55d3 51                       push       ecx
004a55d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004a55d7 8b4508                   mov        eax, dword ptr [ebp + 8]
004a55da 83c074                   add        eax, 0x74
004a55dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a55e0 398120010000             cmp        dword ptr [ecx + 0x120], eax
004a55e6 7514                     jne        0x4a55fc
004a55e8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004a55eb 83c174                   add        ecx, 0x74
004a55ee e89da9f6ff               call       0x40ff90
004a55f3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a55f6 898220010000             mov        dword ptr [edx + 0x120], eax
004a55fc 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004a55ff 83c174                   add        ecx, 0x74
004a5602 e8d9c6f6ff               call       0x411ce0
004a5607 90                       nop        
004a5608 8b4508                   mov        eax, dword ptr [ebp + 8]
004a560b 8b8854030000             mov        ecx, dword ptr [eax + 0x354]
004a5611 c1e912                   shr        ecx, 0x12
004a5614 83e101                   and        ecx, 1
004a5617 7515                     jne        0x4a562e
004a5619 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a561c 8b8224010000             mov        eax, dword ptr [edx + 0x124]
004a5622 83e801                   sub        eax, 1
004a5625 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a5628 898124010000             mov        dword ptr [ecx + 0x124], eax
004a562e 8be5                     mov        esp, ebp
004a5630 5d                       pop        ebp
004a5631 c20400                   ret        4
004a5634 cc                       int3       
004a5635 cc                       int3       
004a5636 cc                       int3       
004a5637 cc                       int3       
004a5638 cc                       int3       
004a5639 cc                       int3       
004a563a cc                       int3       
004a563b cc                       int3       
004a563c cc                       int3       
004a563d cc                       int3       
004a563e cc                       int3       
004a563f cc                       int3       
