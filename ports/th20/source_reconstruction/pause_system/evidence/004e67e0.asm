004e67e0 55                       push       ebp
004e67e1 8bec                     mov        ebp, esp
004e67e3 51                       push       ecx
004e67e4 894dfc                   mov        dword ptr [ebp - 4], ecx
004e67e7 6aff                     push       -1
004e67e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e67ec e8dfa2f8ff               call       0x470ad0
004e67f1 6aff                     push       -1
004e67f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e67f6 e865f9fdff               call       0x4c6160
004e67fb 68000000ff               push       0xff000000
004e6800 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6803 e8a8a2f8ff               call       0x470ab0
004e6808 6a00                     push       0
004e680a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e680d e86e010000               call       0x4e6980
004e6812 6a09                     push       9
004e6814 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6817 e844010000               call       0x4e6960
004e681c 51                       push       ecx
004e681d f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e6825 f30f110424               movss      dword ptr [esp], xmm0
004e682a 51                       push       ecx
004e682b f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e6833 f30f110424               movss      dword ptr [esp], xmm0
004e6838 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e683b e85025fdff               call       0x4b8d90
004e6840 6a00                     push       0
004e6842 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6845 e8d6010000               call       0x4e6a20
004e684a 6a00                     push       0
004e684c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e684f e8ac22faff               call       0x488b00
004e6854 6a02                     push       2
004e6856 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6859 e822f9fdff               call       0x4c6180
004e685e 6a00                     push       0
004e6860 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6863 e89821faff               call       0x488a00
004e6868 6a00                     push       0
004e686a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e686d e87e020000               call       0x4e6af0
004e6872 6a01                     push       1
004e6874 6a01                     push       1
004e6876 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6879 e8a21dfdff               call       0x4b8620
004e687e 51                       push       ecx
004e687f 0f57c0                   xorps      xmm0, xmm0
004e6882 f30f110424               movss      dword ptr [esp], xmm0
004e6887 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e688a e861010000               call       0x4e69f0
004e688f 6a00                     push       0
004e6891 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6894 e887000000               call       0x4e6920
004e6899 90                       nop        
004e689a 8be5                     mov        esp, ebp
004e689c 5d                       pop        ebp
004e689d c3                       ret        
004e689e cc                       int3       
004e689f cc                       int3       
