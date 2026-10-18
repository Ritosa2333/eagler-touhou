004df6e0 55                       push       ebp
004df6e1 8bec                     mov        ebp, esp
004df6e3 51                       push       ecx
004df6e4 68501f5700               push       0x571f50
004df6e9 6a0e                     push       0xe
004df6eb 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004df6f1 e83af4f6ff               call       0x44eb30
004df6f6 8945fc                   mov        dword ptr [ebp - 4], eax
004df6f9 a1385b5c00               mov        eax, dword ptr [0x5c5b38]
004df6fe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004df701 89889c000000             mov        dword ptr [eax + 0x9c], ecx
004df707 837dfc00                 cmp        dword ptr [ebp - 4], 0
004df70b 7514                     jne        0x4df721
004df70d 685c1f5700               push       0x571f5c
004df712 6878065c00               push       0x5c0678
004df717 e8344af7ff               call       0x454150
004df71c 83c408                   add        esp, 8
004df71f eb26                     jmp        0x4df747
004df721 b9d05a5c00               mov        ecx, 0x5c5ad0
004df726 e885b1fdff               call       0x4ba8b0
004df72b 8b15385b5c00             mov        edx, dword ptr [0x5c5b38]
004df731 8b4a08                   mov        ecx, dword ptr [edx + 8]
004df734 e84736f3ff               call       0x412d80
004df739 a1385b5c00               mov        eax, dword ptr [0x5c5b38]
004df73e 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004df741 e83a36f3ff               call       0x412d80
004df746 90                       nop        
004df747 8be5                     mov        esp, ebp
004df749 5d                       pop        ebp
004df74a c3                       ret        
004df74b cc                       int3       
004df74c cc                       int3       
004df74d cc                       int3       
004df74e cc                       int3       
004df74f cc                       int3       
