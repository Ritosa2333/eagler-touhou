004e1710 55                       push       ebp
004e1711 8bec                     mov        ebp, esp
004e1713 83ec10                   sub        esp, 0x10
004e1716 894dfc                   mov        dword ptr [ebp - 4], ecx
004e1719 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e171c 8b88cc000000             mov        ecx, dword ptr [eax + 0xcc]
004e1722 83e901                   sub        ecx, 1
004e1725 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e1728 898acc000000             mov        dword ptr [edx + 0xcc], ecx
004e172e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e1731 83b8cc00000000           cmp        dword ptr [eax + 0xcc], 0
004e1738 7d0d                     jge        0x4e1747
004e173a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e173d c781cc00000000000000     mov        dword ptr [ecx + 0xcc], 0
004e1747 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e174a e8d166fdff               call       0x4b7e20
004e174f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e1752 3982cc000000             cmp        dword ptr [edx + 0xcc], eax
004e1758 7e11                     jle        0x4e176b
004e175a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e175d e8be66fdff               call       0x4b7e20
004e1762 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1765 8981cc000000             mov        dword ptr [ecx + 0xcc], eax
004e176b 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004e1772 7439                     je         0x4e17ad
004e1774 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1777 e8a466fdff               call       0x4b7e20
004e177c 8945f8                   mov        dword ptr [ebp - 8], eax
004e177f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e1782 e8e966fdff               call       0x4b7e70
004e1787 8945f4                   mov        dword ptr [ebp - 0xc], eax
004e178a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e178d e85e68f9ff               call       0x477ff0
004e1792 8945f0                   mov        dword ptr [ebp - 0x10], eax
004e1795 8b55f8                   mov        edx, dword ptr [ebp - 8]
004e1798 52                       push       edx
004e1799 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004e179c 50                       push       eax
004e179d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e17a0 51                       push       ecx
004e17a1 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e17a7 e8a46efdff               call       0x4b8650
004e17ac 90                       nop        
004e17ad 8be5                     mov        esp, ebp
004e17af 5d                       pop        ebp
004e17b0 c3                       ret        
004e17b1 cc                       int3       
004e17b2 cc                       int3       
004e17b3 cc                       int3       
004e17b4 cc                       int3       
004e17b5 cc                       int3       
004e17b6 cc                       int3       
004e17b7 cc                       int3       
004e17b8 cc                       int3       
004e17b9 cc                       int3       
004e17ba cc                       int3       
004e17bb cc                       int3       
004e17bc cc                       int3       
004e17bd cc                       int3       
004e17be cc                       int3       
004e17bf cc                       int3       
