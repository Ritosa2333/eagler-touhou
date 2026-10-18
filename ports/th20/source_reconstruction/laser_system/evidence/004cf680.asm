004cf680 55                       push       ebp
004cf681 8bec                     mov        ebp, esp
004cf683 6aff                     push       -1
004cf685 68cd9b5600               push       0x569bcd
004cf68a 64a100000000             mov        eax, dword ptr fs:[0]
004cf690 50                       push       eax
004cf691 83ec4c                   sub        esp, 0x4c
004cf694 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004cf699 33c5                     xor        eax, ebp
004cf69b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004cf69e 50                       push       eax
004cf69f 8d45f4                   lea        eax, [ebp - 0xc]
004cf6a2 64a300000000             mov        dword ptr fs:[0], eax
004cf6a8 894de0                   mov        dword ptr [ebp - 0x20], ecx
004cf6ab 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004cf6ae 8b4850                   mov        ecx, dword ptr [eax + 0x50]
004cf6b1 51                       push       ecx
004cf6b2 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004cf6b8 e8e37ffaff               call       0x4776a0
004cf6bd 90                       nop        
004cf6be 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004cf6c1 83c210                   add        edx, 0x10
004cf6c4 8955d8                   mov        dword ptr [ebp - 0x28], edx
004cf6c7 6a08                     push       8
004cf6c9 8d4de8                   lea        ecx, [ebp - 0x18]
004cf6cc e8afc9f3ff               call       0x40c080
004cf6d1 8d45e8                   lea        eax, [ebp - 0x18]
004cf6d4 50                       push       eax
004cf6d5 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004cf6d8 e8a32bf4ff               call       0x412280
004cf6dd c745fc00000000           mov        dword ptr [ebp - 4], 0
004cf6e4 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004cf6e7 e8542ef4ff               call       0x412540
004cf6ec 8945bc                   mov        dword ptr [ebp - 0x44], eax
004cf6ef eb09                     jmp        0x4cf6fa
004cf6f1 8d4de8                   lea        ecx, [ebp - 0x18]
004cf6f4 e83725f4ff               call       0x411c30
004cf6f9 90                       nop        
004cf6fa 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004cf6fd 51                       push       ecx
004cf6fe 8d4de8                   lea        ecx, [ebp - 0x18]
004cf701 e8aa24f4ff               call       0x411bb0
004cf706 0fb6d0                   movzx      edx, al
004cf709 85d2                     test       edx, edx
004cf70b 0f84f7010000             je         0x4cf908
004cf711 8d4de8                   lea        ecx, [ebp - 0x18]
004cf714 e8e7cbf3ff               call       0x40c300
004cf719 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004cf71c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf71f e83cfaf3ff               call       0x40f160
004cf724 8b00                     mov        eax, dword ptr [eax]
004cf726 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cf729 d1e9                     shr        ecx, 1
004cf72b 83e103                   and        ecx, 3
004cf72e 0f8495000000             je         0x4cf7c9
004cf734 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf737 e824faf3ff               call       0x40f160
004cf73c 8b10                     mov        edx, dword ptr [eax]
004cf73e 83c21c                   add        edx, 0x1c
004cf741 8955dc                   mov        dword ptr [ebp - 0x24], edx
004cf744 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004cf747 8b08                     mov        ecx, dword ptr [eax]
004cf749 d1e9                     shr        ecx, 1
004cf74b 83e103                   and        ecx, 3
004cf74e 83c101                   add        ecx, 1
004cf751 83e103                   and        ecx, 3
004cf754 d1e1                     shl        ecx, 1
004cf756 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
004cf759 8b02                     mov        eax, dword ptr [edx]
004cf75b 83e0f9                   and        eax, 0xfffffff9
004cf75e 0bc1                     or         eax, ecx
004cf760 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004cf763 8901                     mov        dword ptr [ecx], eax
004cf765 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf768 e8f3f9f3ff               call       0x40f160
004cf76d 8b10                     mov        edx, dword ptr [eax]
004cf76f 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004cf772 d1e8                     shr        eax, 1
004cf774 83e003                   and        eax, 3
004cf777 83f802                   cmp        eax, 2
004cf77a 724d                     jb         0x4cf7c9
004cf77c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf77f e8dcf9f3ff               call       0x40f160
004cf784 8b08                     mov        ecx, dword ptr [eax]
004cf786 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004cf789 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
004cf78c 8b02                     mov        eax, dword ptr [edx]
004cf78e 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004cf791 8b5024                   mov        edx, dword ptr [eax + 0x24]
004cf794 ffd2                     call       edx
004cf796 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf799 e8c2f9f3ff               call       0x40f160
004cf79e 8b00                     mov        eax, dword ptr [eax]
004cf7a0 50                       push       eax
004cf7a1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004cf7a4 e8d7030000               call       0x4cfb80
004cf7a9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf7ac e8aff9f3ff               call       0x40f160
004cf7b1 8945b8                   mov        dword ptr [ebp - 0x48], eax
004cf7b4 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004cf7b7 51                       push       ecx
004cf7b8 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004cf7be e85d87ffff               call       0x4c7f20
004cf7c3 90                       nop        
004cf7c4 e928ffffff               jmp        0x4cf6f1
004cf7c9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf7cc e88ff9f3ff               call       0x40f160
004cf7d1 8b10                     mov        edx, dword ptr [eax]
004cf7d3 837a2001                 cmp        dword ptr [edx + 0x20], 1
004cf7d7 754d                     jne        0x4cf826
004cf7d9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf7dc e87ff9f3ff               call       0x40f160
004cf7e1 8b00                     mov        eax, dword ptr [eax]
004cf7e3 8945d0                   mov        dword ptr [ebp - 0x30], eax
004cf7e6 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004cf7e9 8b11                     mov        edx, dword ptr [ecx]
004cf7eb 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004cf7ee 8b4224                   mov        eax, dword ptr [edx + 0x24]
004cf7f1 ffd0                     call       eax
004cf7f3 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf7f6 e865f9f3ff               call       0x40f160
004cf7fb 8b08                     mov        ecx, dword ptr [eax]
004cf7fd 51                       push       ecx
004cf7fe 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004cf801 e87a030000               call       0x4cfb80
004cf806 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf809 e852f9f3ff               call       0x40f160
004cf80e 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004cf811 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
004cf814 52                       push       edx
004cf815 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004cf81b e80087ffff               call       0x4c7f20
004cf820 90                       nop        
004cf821 e9cbfeffff               jmp        0x4cf6f1
004cf826 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf829 e832f9f3ff               call       0x40f160
004cf82e 8b00                     mov        eax, dword ptr [eax]
004cf830 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cf833 c1e903                   shr        ecx, 3
004cf836 83e101                   and        ecx, 1
004cf839 7426                     je         0x4cf861
004cf83b 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf83e e81df9f3ff               call       0x40f160
004cf843 8b10                     mov        edx, dword ptr [eax]
004cf845 8955cc                   mov        dword ptr [ebp - 0x34], edx
004cf848 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004cf84b 8b08                     mov        ecx, dword ptr [eax]
004cf84d 8b514c                   mov        edx, dword ptr [ecx + 0x4c]
004cf850 8955b0                   mov        dword ptr [ebp - 0x50], edx
004cf853 6a01                     push       1
004cf855 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004cf858 ff55b0                   call       dword ptr [ebp - 0x50]
004cf85b 90                       nop        
004cf85c e990feffff               jmp        0x4cf6f1
004cf861 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf864 e8f7f8f3ff               call       0x40f160
004cf869 8b00                     mov        eax, dword ptr [eax]
004cf86b 8945c8                   mov        dword ptr [ebp - 0x38], eax
004cf86e 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cf871 8b11                     mov        edx, dword ptr [ecx]
004cf873 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004cf876 8b421c                   mov        eax, dword ptr [edx + 0x1c]
004cf879 ffd0                     call       eax
004cf87b 85c0                     test       eax, eax
004cf87d 744d                     je         0x4cf8cc
004cf87f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf882 e8d9f8f3ff               call       0x40f160
004cf887 8b08                     mov        ecx, dword ptr [eax]
004cf889 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004cf88c 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004cf88f 8b02                     mov        eax, dword ptr [edx]
004cf891 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004cf894 8b5024                   mov        edx, dword ptr [eax + 0x24]
004cf897 ffd2                     call       edx
004cf899 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf89c e8bff8f3ff               call       0x40f160
004cf8a1 8b00                     mov        eax, dword ptr [eax]
004cf8a3 50                       push       eax
004cf8a4 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004cf8a7 e8d4020000               call       0x4cfb80
004cf8ac 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf8af e8acf8f3ff               call       0x40f160
004cf8b4 8945ac                   mov        dword ptr [ebp - 0x54], eax
004cf8b7 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
004cf8ba 51                       push       ecx
004cf8bb 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004cf8c1 e85a86ffff               call       0x4c7f20
004cf8c6 90                       nop        
004cf8c7 e925feffff               jmp        0x4cf6f1
004cf8cc 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf8cf e88cf8f3ff               call       0x40f160
004cf8d4 8b10                     mov        edx, dword ptr [eax]
004cf8d6 83c228                   add        edx, 0x28
004cf8d9 8955a8                   mov        dword ptr [ebp - 0x58], edx
004cf8dc 6a00                     push       0
004cf8de 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004cf8e1 e85a3cf5ff               call       0x423540
004cf8e6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004cf8e9 e872f8f3ff               call       0x40f160
004cf8ee 8b00                     mov        eax, dword ptr [eax]
004cf8f0 83c01c                   add        eax, 0x1c
004cf8f3 8945c0                   mov        dword ptr [ebp - 0x40], eax
004cf8f6 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004cf8f9 8b11                     mov        edx, dword ptr [ecx]
004cf8fb 83ca01                   or         edx, 1
004cf8fe 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004cf901 8910                     mov        dword ptr [eax], edx
004cf903 e9e9fdffff               jmp        0x4cf6f1
004cf908 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004cf90f 8d4de8                   lea        ecx, [ebp - 0x18]
004cf912 e8e921f4ff               call       0x411b00
004cf917 90                       nop        
004cf918 b801000000               mov        eax, 1
004cf91d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004cf920 64890d00000000           mov        dword ptr fs:[0], ecx
004cf927 59                       pop        ecx
004cf928 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004cf92b 33cd                     xor        ecx, ebp
004cf92d e8bc300700               call       0x5429ee
004cf932 8be5                     mov        esp, ebp
004cf934 5d                       pop        ebp
004cf935 c3                       ret        
004cf936 cc                       int3       
004cf937 cc                       int3       
004cf938 cc                       int3       
004cf939 cc                       int3       
004cf93a cc                       int3       
004cf93b cc                       int3       
004cf93c cc                       int3       
004cf93d cc                       int3       
004cf93e cc                       int3       
004cf93f cc                       int3       
