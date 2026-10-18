0046cd40  55                                 push       ebp
0046cd41  8b ec                              mov        ebp, esp
0046cd43  83 ec 08                           sub        esp, 8
0046cd46  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0046cd49  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046cd4c  e8 cf 38 00 00                     call       0x470620
0046cd51  89 45 f8                           mov        dword ptr [ebp - 8], eax
0046cd54  0f b6 45 14                        movzx      eax, byte ptr [ebp + 0x14]
0046cd58  50                                 push       eax
0046cd59  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
0046cd5c  51                                 push       ecx
0046cd5d  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0046cd60  52                                 push       edx
0046cd61  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0046cd64  50                                 push       eax
0046cd65  e8 56 36 00 00                     call       0x4703c0
0046cd6a  83 c4 10                           add        esp, 0x10
0046cd6d  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0046cd70  51                                 push       ecx
0046cd71  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0046cd74  e8 c7 fc ff ff                     call       0x46ca40
0046cd79  90                                 nop        
0046cd7a  8b e5                              mov        esp, ebp
0046cd7c  5d                                 pop        ebp
0046cd7d  c2 10 00                           ret        0x10
