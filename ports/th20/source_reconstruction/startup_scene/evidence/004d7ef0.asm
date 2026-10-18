004d7ef0  55                                 push       ebp
004d7ef1  8b ec                              mov        ebp, esp
004d7ef3  6a ff                              push       -1
004d7ef5  68 00 76 56 00                     push       0x567600
004d7efa  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004d7f00  50                                 push       eax
004d7f01  51                                 push       ecx
004d7f02  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004d7f07  33 c5                              xor        eax, ebp
004d7f09  50                                 push       eax
004d7f0a  8d 45 f4                           lea        eax, [ebp - 0xc]
004d7f0d  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004d7f13  89 4d f0                           mov        dword ptr [ebp - 0x10], ecx
004d7f16  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7f19  e8 02 7e f4 ff                     call       0x41fd20
004d7f1e  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d7f21  c7 00 98 17 57 00                  mov        dword ptr [eax], 0x571798
004d7f27  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7f2a  83 c1 10                           add        ecx, 0x10
004d7f2d  e8 4e 38 f3 ff                     call       0x40b780
004d7f32  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7f35  c7 41 20 00 00 00 00               mov        dword ptr [ecx + 0x20], 0
004d7f3c  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7f3f  83 c1 24                           add        ecx, 0x24
004d7f42  e8 f9 0b f7 ff                     call       0x448b40
004d7f47  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7f4a  81 c1 08 06 00 00                  add        ecx, 0x608
004d7f50  e8 6b dd f4 ff                     call       0x425cc0
004d7f55  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004d7f58  c7 82 0c 06 00 00 00 00 00 00      mov        dword ptr [edx + 0x60c], 0
004d7f62  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d7f65  c7 80 10 06 00 00 00 00 00 00      mov        dword ptr [eax + 0x610], 0
004d7f6f  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7f72  c7 81 14 06 00 00 00 00 00 00      mov        dword ptr [ecx + 0x614], 0
004d7f7c  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004d7f7f  c7 82 18 06 00 00 00 00 00 00      mov        dword ptr [edx + 0x618], 0
004d7f89  68 a4 17 57 00                     push       0x5717a4 ; string='initialize LoadingInf'
004d7f8e  e8 1d 47 f3 ff                     call       0x40c6b0
004d7f93  83 c4 04                           add        esp, 4
004d7f96  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d7f99  a3 2c 4d 5c 00                     mov        dword ptr [0x5c4d2c], eax
004d7f9e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004d7fa1  8b 51 04                           mov        edx, dword ptr [ecx + 4]
004d7fa4  83 ca 02                           or         edx, 2
004d7fa7  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d7faa  89 50 04                           mov        dword ptr [eax + 4], edx
004d7fad  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
004d7fb0  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004d7fb3  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004d7fba  59                                 pop        ecx
004d7fbb  8b e5                              mov        esp, ebp
004d7fbd  5d                                 pop        ebp
004d7fbe  c3                                 ret        
