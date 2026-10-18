004dbd70  55                                 push       ebp
004dbd71  8b ec                              mov        ebp, esp
004dbd73  83 ec 64                           sub        esp, 0x64
004dbd76  56                                 push       esi
004dbd77  57                                 push       edi
004dbd78  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004dbd7b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbd7e  83 b8 9c 01 00 00 00               cmp        dword ptr [eax + 0x19c], 0
004dbd85  0f 85 2f 03 00 00                  jne        0x4dc0ba
004dbd8b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbd8e  83 b9 a4 01 00 00 00               cmp        dword ptr [ecx + 0x1a4], 0
004dbd95  75 36                              jne        0x4dbdcd
004dbd97  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbd9a  e8 91 69 f3 ff                     call       0x412730
004dbd9f  89 45 f8                           mov        dword ptr [ebp - 8], eax
004dbda2  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004dbda5  8b 02                              mov        eax, dword ptr [edx]
004dbda7  8b 48 48                           mov        ecx, dword ptr [eax + 0x48]
004dbdaa  89 4d ec                           mov        dword ptr [ebp - 0x14], ecx
004dbdad  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbdb0  81 c2 a4 01 00 00                  add        edx, 0x1a4
004dbdb6  52                                 push       edx
004dbdb7  6a 00                              push       0
004dbdb9  6a 00                              push       0
004dbdbb  6a 00                              push       0
004dbdbd  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
004dbdc0  50                                 push       eax
004dbdc1  ff 55 ec                           call       dword ptr [ebp - 0x14]
004dbdc4  85 c0                              test       eax, eax
004dbdc6  74 05                              je         0x4dbdcd
004dbdc8  e9 18 03 00 00                     jmp        0x4dc0e5
004dbdcd  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbdd0  8b 91 40 0b 00 00                  mov        edx, dword ptr [ecx + 0xb40]
004dbdd6  89 55 e8                           mov        dword ptr [ebp - 0x18], edx
004dbdd9  6a 00                              push       0
004dbddb  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004dbdde  e8 1d 3d fe ff                     call       0x4bfb00
004dbde3  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004dbde6  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
004dbde9  8b 08                              mov        ecx, dword ptr [eax]
004dbdeb  8b 51 48                           mov        edx, dword ptr [ecx + 0x48]
004dbdee  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
004dbdf1  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbdf4  05 9c 01 00 00                     add        eax, 0x19c
004dbdf9  50                                 push       eax
004dbdfa  6a 00                              push       0
004dbdfc  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004dbdff  51                                 push       ecx
004dbe00  ff 55 e4                           call       dword ptr [ebp - 0x1c]
004dbe03  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbe06  8b 82 40 0b 00 00                  mov        eax, dword ptr [edx + 0xb40]
004dbe0c  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004dbe0f  6a 01                              push       1
004dbe11  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004dbe14  e8 e7 3c fe ff                     call       0x4bfb00
004dbe19  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004dbe1c  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004dbe1f  8b 11                              mov        edx, dword ptr [ecx]
004dbe21  8b 42 48                           mov        eax, dword ptr [edx + 0x48]
004dbe24  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004dbe27  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbe2a  81 c1 a0 01 00 00                  add        ecx, 0x1a0
004dbe30  51                                 push       ecx
004dbe31  6a 00                              push       0
004dbe33  8b 55 f0                           mov        edx, dword ptr [ebp - 0x10]
004dbe36  52                                 push       edx
004dbe37  ff 55 dc                           call       dword ptr [ebp - 0x24]
004dbe3a  90                                 nop        
004dbe3b  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbe3e  8b 88 ac 01 00 00                  mov        ecx, dword ptr [eax + 0x1ac]
004dbe44  e8 67 9c f6 ff                     call       0x445ab0
004dbe49  85 c0                              test       eax, eax
004dbe4b  0f 85 3b 02 00 00                  jne        0x4dc08c
004dbe51  81 3d f8 87 5b 00 80 02 00 00      cmp        dword ptr [0x5b87f8], 0x280
004dbe5b  0f 85 b0 00 00 00                  jne        0x4dbf11
004dbe61  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbe64  8b 91 40 0b 00 00                  mov        edx, dword ptr [ecx + 0xb40]
004dbe6a  89 55 d8                           mov        dword ptr [ebp - 0x28], edx
004dbe6d  6a 00                              push       0
004dbe6f  6a 00                              push       0
004dbe71  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbe74  8b 88 ac 01 00 00                  mov        ecx, dword ptr [eax + 0x1ac]
004dbe7a  51                                 push       ecx
004dbe7b  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
004dbe7e  e8 2d c4 f5 ff                     call       0x4382b0
004dbe83  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbe86  8b 82 40 0b 00 00                  mov        eax, dword ptr [edx + 0xb40]
004dbe8c  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
004dbe8f  6a 00                              push       0
004dbe91  6a 07                              push       7
004dbe93  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbe96  8b 91 b0 01 00 00                  mov        edx, dword ptr [ecx + 0x1b0]
004dbe9c  52                                 push       edx
004dbe9d  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
004dbea0  e8 0b c4 f5 ff                     call       0x4382b0
004dbea5  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbea8  8b 88 40 0b 00 00                  mov        ecx, dword ptr [eax + 0xb40]
004dbeae  89 4d d0                           mov        dword ptr [ebp - 0x30], ecx
004dbeb1  6a 00                              push       0
004dbeb3  6a 04                              push       4
004dbeb5  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbeb8  8b 82 b4 01 00 00                  mov        eax, dword ptr [edx + 0x1b4]
004dbebe  50                                 push       eax
004dbebf  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
004dbec2  e8 e9 c3 f5 ff                     call       0x4382b0
004dbec7  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbeca  8b 91 40 0b 00 00                  mov        edx, dword ptr [ecx + 0xb40]
004dbed0  89 55 cc                           mov        dword ptr [ebp - 0x34], edx
004dbed3  6a 00                              push       0
004dbed5  6a 0a                              push       0xa
004dbed7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbeda  8b 88 b8 01 00 00                  mov        ecx, dword ptr [eax + 0x1b8]
004dbee0  51                                 push       ecx
004dbee1  8b 4d cc                           mov        ecx, dword ptr [ebp - 0x34]
004dbee4  e8 c7 c3 f5 ff                     call       0x4382b0
004dbee9  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbeec  8b 82 40 0b 00 00                  mov        eax, dword ptr [edx + 0xb40]
004dbef2  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
004dbef5  6a 00                              push       0
004dbef7  6a 0d                              push       0xd
004dbef9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbefc  8b 91 bc 01 00 00                  mov        edx, dword ptr [ecx + 0x1bc]
004dbf02  52                                 push       edx
004dbf03  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
004dbf06  e8 a5 c3 f5 ff                     call       0x4382b0
004dbf0b  90                                 nop        
004dbf0c  e9 7b 01 00 00                     jmp        0x4dc08c
004dbf11  81 3d f8 87 5b 00 c0 03 00 00      cmp        dword ptr [0x5b87f8], 0x3c0
004dbf1b  0f 85 b0 00 00 00                  jne        0x4dbfd1
004dbf21  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbf24  8b 88 40 0b 00 00                  mov        ecx, dword ptr [eax + 0xb40]
004dbf2a  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
004dbf2d  6a 00                              push       0
004dbf2f  6a 01                              push       1
004dbf31  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbf34  8b 82 ac 01 00 00                  mov        eax, dword ptr [edx + 0x1ac]
004dbf3a  50                                 push       eax
004dbf3b  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
004dbf3e  e8 6d c3 f5 ff                     call       0x4382b0
004dbf43  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbf46  8b 91 40 0b 00 00                  mov        edx, dword ptr [ecx + 0xb40]
004dbf4c  89 55 c0                           mov        dword ptr [ebp - 0x40], edx
004dbf4f  6a 00                              push       0
004dbf51  6a 08                              push       8
004dbf53  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbf56  8b 88 b0 01 00 00                  mov        ecx, dword ptr [eax + 0x1b0]
004dbf5c  51                                 push       ecx
004dbf5d  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
004dbf60  e8 4b c3 f5 ff                     call       0x4382b0
004dbf65  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbf68  8b 82 40 0b 00 00                  mov        eax, dword ptr [edx + 0xb40]
004dbf6e  89 45 bc                           mov        dword ptr [ebp - 0x44], eax
004dbf71  6a 00                              push       0
004dbf73  6a 05                              push       5
004dbf75  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbf78  8b 91 b4 01 00 00                  mov        edx, dword ptr [ecx + 0x1b4]
004dbf7e  52                                 push       edx
004dbf7f  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
004dbf82  e8 29 c3 f5 ff                     call       0x4382b0
004dbf87  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbf8a  8b 88 40 0b 00 00                  mov        ecx, dword ptr [eax + 0xb40]
004dbf90  89 4d b8                           mov        dword ptr [ebp - 0x48], ecx
004dbf93  6a 00                              push       0
004dbf95  6a 0b                              push       0xb
004dbf97  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbf9a  8b 82 b8 01 00 00                  mov        eax, dword ptr [edx + 0x1b8]
004dbfa0  50                                 push       eax
004dbfa1  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
004dbfa4  e8 07 c3 f5 ff                     call       0x4382b0
004dbfa9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbfac  8b 91 40 0b 00 00                  mov        edx, dword ptr [ecx + 0xb40]
004dbfb2  89 55 b4                           mov        dword ptr [ebp - 0x4c], edx
004dbfb5  6a 00                              push       0
004dbfb7  6a 0e                              push       0xe
004dbfb9  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dbfbc  8b 88 bc 01 00 00                  mov        ecx, dword ptr [eax + 0x1bc]
004dbfc2  51                                 push       ecx
004dbfc3  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
004dbfc6  e8 e5 c2 f5 ff                     call       0x4382b0
004dbfcb  90                                 nop        
004dbfcc  e9 bb 00 00 00                     jmp        0x4dc08c
004dbfd1  81 3d f8 87 5b 00 00 05 00 00      cmp        dword ptr [0x5b87f8], 0x500
004dbfdb  0f 85 ab 00 00 00                  jne        0x4dc08c
004dbfe1  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dbfe4  8b 82 40 0b 00 00                  mov        eax, dword ptr [edx + 0xb40]
004dbfea  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
004dbfed  6a 00                              push       0
004dbfef  6a 02                              push       2
004dbff1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dbff4  8b 91 ac 01 00 00                  mov        edx, dword ptr [ecx + 0x1ac]
004dbffa  52                                 push       edx
004dbffb  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
004dbffe  e8 ad c2 f5 ff                     call       0x4382b0
004dc003  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc006  8b 88 40 0b 00 00                  mov        ecx, dword ptr [eax + 0xb40]
004dc00c  89 4d ac                           mov        dword ptr [ebp - 0x54], ecx
004dc00f  6a 00                              push       0
004dc011  6a 09                              push       9
004dc013  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc016  8b 82 b0 01 00 00                  mov        eax, dword ptr [edx + 0x1b0]
004dc01c  50                                 push       eax
004dc01d  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
004dc020  e8 8b c2 f5 ff                     call       0x4382b0
004dc025  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc028  8b 91 40 0b 00 00                  mov        edx, dword ptr [ecx + 0xb40]
004dc02e  89 55 a8                           mov        dword ptr [ebp - 0x58], edx
004dc031  6a 00                              push       0
004dc033  6a 06                              push       6
004dc035  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc038  8b 88 b4 01 00 00                  mov        ecx, dword ptr [eax + 0x1b4]
004dc03e  51                                 push       ecx
004dc03f  8b 4d a8                           mov        ecx, dword ptr [ebp - 0x58]
004dc042  e8 69 c2 f5 ff                     call       0x4382b0
004dc047  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc04a  8b 82 40 0b 00 00                  mov        eax, dword ptr [edx + 0xb40]
004dc050  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
004dc053  6a 00                              push       0
004dc055  6a 0c                              push       0xc
004dc057  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc05a  8b 91 b8 01 00 00                  mov        edx, dword ptr [ecx + 0x1b8]
004dc060  52                                 push       edx
004dc061  8b 4d a4                           mov        ecx, dword ptr [ebp - 0x5c]
004dc064  e8 47 c2 f5 ff                     call       0x4382b0
004dc069  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dc06c  8b 88 40 0b 00 00                  mov        ecx, dword ptr [eax + 0xb40]
004dc072  89 4d a0                           mov        dword ptr [ebp - 0x60], ecx
004dc075  6a 00                              push       0
004dc077  6a 0f                              push       0xf
004dc079  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc07c  8b 82 bc 01 00 00                  mov        eax, dword ptr [edx + 0x1bc]
004dc082  50                                 push       eax
004dc083  8b 4d a0                           mov        ecx, dword ptr [ebp - 0x60]
004dc086  e8 25 c2 f5 ff                     call       0x4382b0
004dc08b  90                                 nop        
004dc08c  f3 0f 10 05 18 88 5b 00            movss      xmm0, dword ptr [0x5b8818]
004dc094  0f 2e 05 68 cd 56 00               ucomiss    xmm0, dword ptr [0x56cd68]
004dc09b  9f                                 lahf       
004dc09c  f6 c4 44                           test       ah, 0x44
004dc09f  7a 17                              jp         0x4dc0b8
004dc0a1  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dc0a4  8b 91 b4 01 00 00                  mov        edx, dword ptr [ecx + 0x1b4]
004dc0aa  89 55 9c                           mov        dword ptr [ebp - 0x64], edx
004dc0ad  6a 00                              push       0
004dc0af  8b 4d 9c                           mov        ecx, dword ptr [ebp - 0x64]
004dc0b2  e8 79 19 00 00                     call       0x4dda30
004dc0b7  90                                 nop        
004dc0b8  eb 2b                              jmp        0x4dc0e5
004dc0ba  b8 6c 01 00 00                     mov        eax, 0x16c
004dc0bf  6b c8 00                           imul       ecx, eax, 0
004dc0c2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc0c5  8d b4 0a 78 02 00 00               lea        esi, [edx + ecx + 0x278]
004dc0cc  b8 6c 01 00 00                     mov        eax, 0x16c
004dc0d1  6b c8 03                           imul       ecx, eax, 3
004dc0d4  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004dc0d7  8d bc 0a 78 02 00 00               lea        edi, [edx + ecx + 0x278]
004dc0de  b9 5b 00 00 00                     mov        ecx, 0x5b
004dc0e3  f3 a5                              rep movsd  dword ptr es:[edi], dword ptr [esi]
004dc0e5  5f                                 pop        edi
004dc0e6  5e                                 pop        esi
004dc0e7  8b e5                              mov        esp, ebp
004dc0e9  5d                                 pop        ebp
004dc0ea  c3                                 ret        
