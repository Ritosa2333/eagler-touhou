0050e6e0  55                                 push       ebp
0050e6e1  8b ec                              mov        ebp, esp
0050e6e3  51                                 push       ecx
0050e6e4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050e6e7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e6ea  e8 11 fe ff ff                     call       0x50e500
0050e6ef  33 c0                              xor        eax, eax
0050e6f1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e6f4  83 c1 16                           add        ecx, 0x16
0050e6f7  89 01                              mov        dword ptr [ecx], eax
0050e6f9  89 41 04                           mov        dword ptr [ecx + 4], eax
0050e6fc  89 41 08                           mov        dword ptr [ecx + 8], eax
0050e6ff  89 41 0c                           mov        dword ptr [ecx + 0xc], eax
0050e702  89 41 10                           mov        dword ptr [ecx + 0x10], eax
0050e705  89 41 14                           mov        dword ptr [ecx + 0x14], eax
0050e708  89 41 18                           mov        dword ptr [ecx + 0x18], eax
0050e70b  89 41 1c                           mov        dword ptr [ecx + 0x1c], eax
0050e70e  33 d2                              xor        edx, edx
0050e710  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e713  83 c0 36                           add        eax, 0x36
0050e716  89 10                              mov        dword ptr [eax], edx
0050e718  89 50 04                           mov        dword ptr [eax + 4], edx
0050e71b  89 50 08                           mov        dword ptr [eax + 8], edx
0050e71e  89 50 0c                           mov        dword ptr [eax + 0xc], edx
0050e721  89 50 10                           mov        dword ptr [eax + 0x10], edx
0050e724  89 50 14                           mov        dword ptr [eax + 0x14], edx
0050e727  89 50 18                           mov        dword ptr [eax + 0x18], edx
0050e72a  89 50 1c                           mov        dword ptr [eax + 0x1c], edx
0050e72d  33 c9                              xor        ecx, ecx
0050e72f  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e732  83 c2 56                           add        edx, 0x56
0050e735  66 89 0a                           mov        word ptr [edx], cx
0050e738  33 c0                              xor        eax, eax
0050e73a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e73d  83 c1 58                           add        ecx, 0x58
0050e740  89 01                              mov        dword ptr [ecx], eax
0050e742  89 41 04                           mov        dword ptr [ecx + 4], eax
0050e745  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e748  33 c0                              xor        eax, eax
0050e74a  33 c9                              xor        ecx, ecx
0050e74c  89 42 60                           mov        dword ptr [edx + 0x60], eax
0050e74f  89 4a 64                           mov        dword ptr [edx + 0x64], ecx
0050e752  68 80 00 00 00                     push       0x80
0050e757  6a 00                              push       0
0050e759  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e75c  83 c2 68                           add        edx, 0x68
0050e75f  52                                 push       edx
0050e760  e8 fb 5f 03 00                     call       0x544760
0050e765  83 c4 0c                           add        esp, 0xc
0050e768  6a 40                              push       0x40
0050e76a  6a 00                              push       0
0050e76c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e76f  05 e8 00 00 00                     add        eax, 0xe8
0050e774  50                                 push       eax
0050e775  e8 e6 5f 03 00                     call       0x544760
0050e77a  83 c4 0c                           add        esp, 0xc
0050e77d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e780  c7 81 28 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x128], 0
0050e78a  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e78d  c7 82 2c 01 00 00 08 00 00 00      mov        dword ptr [edx + 0x12c], 8
0050e797  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e79a  c7 80 30 01 00 00 08 00 00 00      mov        dword ptr [eax + 0x130], 8
0050e7a4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e7a7  c7 81 34 01 00 00 08 00 00 00      mov        dword ptr [ecx + 0x134], 8
0050e7b1  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e7b4  c7 82 38 01 00 00 01 00 00 00      mov        dword ptr [edx + 0x138], 1
0050e7be  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e7c1  c7 80 3c 01 00 00 08 00 00 00      mov        dword ptr [eax + 0x13c], 8
0050e7cb  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e7ce  c7 81 40 01 00 00 08 00 00 00      mov        dword ptr [ecx + 0x140], 8
0050e7d8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e7db  c7 82 44 01 00 00 08 00 00 00      mov        dword ptr [edx + 0x144], 8
0050e7e5  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e7e8  c7 80 48 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x148], 0
0050e7f2  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e7f5  c7 81 4c 01 00 00 08 00 00 00      mov        dword ptr [ecx + 0x14c], 8
0050e7ff  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e802  c7 82 50 01 00 00 08 00 00 00      mov        dword ptr [edx + 0x150], 8
0050e80c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e80f  c7 80 54 01 00 00 08 00 00 00      mov        dword ptr [eax + 0x154], 8
0050e819  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e81c  c7 81 58 01 00 00 01 00 00 00      mov        dword ptr [ecx + 0x158], 1
0050e826  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e829  c7 82 5c 01 00 00 08 00 00 00      mov        dword ptr [edx + 0x15c], 8
0050e833  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e836  c7 80 60 01 00 00 08 00 00 00      mov        dword ptr [eax + 0x160], 8
0050e840  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e843  c7 81 64 01 00 00 08 00 00 00      mov        dword ptr [ecx + 0x164], 8
0050e84d  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e850  c7 82 68 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x168], 0
0050e85a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e85d  c7 80 6c 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x16c], 0
0050e867  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e86a  c7 81 70 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x170], 0
0050e874  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e877  c7 82 74 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x174], 0
0050e881  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e884  c7 80 78 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x178], 0
0050e88e  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e891  c7 81 7c 01 00 00 00 00 00 00      mov        dword ptr [ecx + 0x17c], 0
0050e89b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e89e  c7 82 80 01 00 00 00 00 00 00      mov        dword ptr [edx + 0x180], 0
0050e8a8  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e8ab  c7 80 84 01 00 00 00 00 00 00      mov        dword ptr [eax + 0x184], 0
0050e8b5  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e8b8  c7 81 88 01 00 00 09 00 00 00      mov        dword ptr [ecx + 0x188], 9
0050e8c2  33 d2                              xor        edx, edx
0050e8c4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e8c7  05 8c 01 00 00                     add        eax, 0x18c
0050e8cc  89 10                              mov        dword ptr [eax], edx
0050e8ce  89 50 04                           mov        dword ptr [eax + 4], edx
0050e8d1  89 50 08                           mov        dword ptr [eax + 8], edx
0050e8d4  89 50 0c                           mov        dword ptr [eax + 0xc], edx
0050e8d7  89 50 10                           mov        dword ptr [eax + 0x10], edx
0050e8da  89 50 14                           mov        dword ptr [eax + 0x14], edx
0050e8dd  89 50 18                           mov        dword ptr [eax + 0x18], edx
0050e8e0  89 50 1c                           mov        dword ptr [eax + 0x1c], edx
0050e8e3  89 50 20                           mov        dword ptr [eax + 0x20], edx
0050e8e6  33 c9                              xor        ecx, ecx
0050e8e8  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0050e8eb  81 c2 b0 01 00 00                  add        edx, 0x1b0
0050e8f1  89 0a                              mov        dword ptr [edx], ecx
0050e8f3  89 4a 04                           mov        dword ptr [edx + 4], ecx
0050e8f6  89 4a 08                           mov        dword ptr [edx + 8], ecx
0050e8f9  89 4a 0c                           mov        dword ptr [edx + 0xc], ecx
0050e8fc  89 4a 10                           mov        dword ptr [edx + 0x10], ecx
0050e8ff  89 4a 14                           mov        dword ptr [edx + 0x14], ecx
0050e902  89 4a 18                           mov        dword ptr [edx + 0x18], ecx
0050e905  89 4a 1c                           mov        dword ptr [edx + 0x1c], ecx
0050e908  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e90b  c6 80 d0 01 00 00 00               mov        byte ptr [eax + 0x1d0], 0
0050e912  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e915  c6 81 d1 01 00 00 00               mov        byte ptr [ecx + 0x1d1], 0
0050e91c  33 d2                              xor        edx, edx
0050e91e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e921  05 d2 01 00 00                     add        eax, 0x1d2
0050e926  89 10                              mov        dword ptr [eax], edx
0050e928  89 50 04                           mov        dword ptr [eax + 4], edx
0050e92b  89 50 08                           mov        dword ptr [eax + 8], edx
0050e92e  89 50 0c                           mov        dword ptr [eax + 0xc], edx
0050e931  89 50 10                           mov        dword ptr [eax + 0x10], edx
0050e934  89 50 14                           mov        dword ptr [eax + 0x14], edx
0050e937  89 50 18                           mov        dword ptr [eax + 0x18], edx
0050e93a  89 50 1c                           mov        dword ptr [eax + 0x1c], edx
0050e93d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e940  8b e5                              mov        esp, ebp
0050e942  5d                                 pop        ebp
0050e943  c3                                 ret        
