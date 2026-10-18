004e14b0 55                       push       ebp
004e14b1 8bec                     mov        ebp, esp
004e14b3 83ec08                   sub        esp, 8
004e14b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004e14b9 6a00                     push       0
004e14bb 6a0a                     push       0xa
004e14bd 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004e14c0 50                       push       eax
004e14c1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004e14c4 51                       push       ecx
004e14c5 e8361d0600               call       0x543200
004e14ca 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e14cd 0301                     add        eax, dword ptr [ecx]
004e14cf 135104                   adc        edx, dword ptr [ecx + 4]
004e14d2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e14d5 8901                     mov        dword ptr [ecx], eax
004e14d7 895104                   mov        dword ptr [ecx + 4], edx
004e14da 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e14dd 8955f8                   mov        dword ptr [ebp - 8], edx
004e14e0 8b45f8                   mov        eax, dword ptr [ebp - 8]
004e14e3 83780400                 cmp        dword ptr [eax + 4], 0
004e14e7 770b                     ja         0x4e14f4
004e14e9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e14ec 813900ca9a3b             cmp        dword ptr [ecx], 0x3b9aca00
004e14f2 720e                     jb         0x4e1502
004e14f4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e14f7 33c0                     xor        eax, eax
004e14f9 c702ffc99a3b             mov        dword ptr [edx], 0x3b9ac9ff
004e14ff 894204                   mov        dword ptr [edx + 4], eax
004e1502 8be5                     mov        esp, ebp
004e1504 5d                       pop        ebp
004e1505 c20800                   ret        8
004e1508 cc                       int3       
004e1509 cc                       int3       
004e150a cc                       int3       
004e150b cc                       int3       
004e150c cc                       int3       
004e150d cc                       int3       
004e150e cc                       int3       
004e150f cc                       int3       
