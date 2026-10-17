0046cc80  55                                 push       ebp
0046cc81  8b ec                              mov        ebp, esp
0046cc83  6a ff                              push       -1
0046cc85  68 ed 76 56 00                     push       0x5676ed
0046cc8a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
0046cc90  50                                 push       eax
0046cc91  83 ec 18                           sub        esp, 0x18
0046cc94  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0046cc99  33 c5                              xor        eax, ebp
0046cc9b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0046cc9e  50                                 push       eax
0046cc9f  8d 45 f4                           lea        eax, [ebp - 0xc]
0046cca2  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0046cca8  89 4d e4                           mov        dword ptr [ebp - 0x1c], ecx
0046ccab  6a 12                              push       0x12
0046ccad  b9 40 02 5c 00                     mov        ecx, 0x5c0240
0046ccb2  e8 59 f6 f9 ff                     call       0x40c310
0046ccb7  50                                 push       eax
0046ccb8  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046ccbb  e8 70 ea f9 ff                     call       0x40b730
0046ccc0  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0046ccc7  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046ccca  e8 51 39 00 00                     call       0x470620
0046cccf  89 45 e8                           mov        dword ptr [ebp - 0x18], eax
0046ccd2  6a 0a                              push       0xa
0046ccd4  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0046ccd7  50                                 push       eax
0046ccd8  b9 01 00 00 00                     mov        ecx, 1
0046ccdd  69 d1 ff 00 00 00                  imul       edx, ecx, 0xff
0046cce3  03 55 e8                           add        edx, dword ptr [ebp - 0x18]
0046cce6  52                                 push       edx
0046cce7  b8 01 00 00 00                     mov        eax, 1
0046ccec  6b c8 00                           imul       ecx, eax, 0
0046ccef  03 4d e8                           add        ecx, dword ptr [ebp - 0x18]
0046ccf2  51                                 push       ecx
0046ccf3  e8 18 c5 fe ff                     call       0x459210
0046ccf8  83 c4 10                           add        esp, 0x10
0046ccfb  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
0046ccfe  89 55 e0                           mov        dword ptr [ebp - 0x20], edx
0046cd01  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
0046cd04  c6 02 00                           mov        byte ptr [edx], 0
0046cd07  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0046cd0a  50                                 push       eax
0046cd0b  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
0046cd0e  e8 2d fd ff ff                     call       0x46ca40
0046cd13  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0046cd1a  8d 4d ec                           lea        ecx, [ebp - 0x14]
0046cd1d  e8 de eb f9 ff                     call       0x40b900
0046cd22  90                                 nop        
0046cd23  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
0046cd26  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
0046cd2d  59                                 pop        ecx
0046cd2e  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
0046cd31  33 cd                              xor        ecx, ebp
0046cd33  e8 b6 5c 0d 00                     call       0x5429ee
0046cd38  8b e5                              mov        esp, ebp
0046cd3a  5d                                 pop        ebp
0046cd3b  c2 08 00                           ret        8
