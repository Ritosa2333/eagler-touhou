00416420  55                                 push       ebp
00416421  8b ec                              mov        ebp, esp
00416423  6a ff                              push       -1
00416425  68 75 7e 56 00                     push       0x567e75
0041642a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00416430  50                                 push       eax
00416431  81 ec d0 00 00 00                  sub        esp, 0xd0
00416437  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041643c  33 c5                              xor        eax, ebp
0041643e  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
00416441  56                                 push       esi
00416442  50                                 push       eax
00416443  8d 45 f4                           lea        eax, [ebp - 0xc]
00416446  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
0041644c  89 4d c0                           mov        dword ptr [ebp - 0x40], ecx
0041644f  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416452  e8 69 f2 ff ff                     call       0x4156c0
00416457  0f af 45 08                        imul       eax, dword ptr [ebp + 8]
0041645b  89 85 54 ff ff ff                  mov        dword ptr [ebp - 0xac], eax
00416461  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416464  e8 37 f2 ff ff                     call       0x4156a0
00416469  89 85 60 ff ff ff                  mov        dword ptr [ebp - 0xa0], eax
0041646f  8b 85 54 ff ff ff                  mov        eax, dword ptr [ebp - 0xac]
00416475  50                                 push       eax
00416476  68 e0 c7 56 00                     push       0x56c7e0 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\grpfont.cpp:615 u_char'
0041647b  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
00416481  e8 3a 74 ff ff                     call       0x40d8c0
00416486  89 85 78 ff ff ff                  mov        dword ptr [ebp - 0x88], eax
0041648c  8b 8d 54 ff ff ff                  mov        ecx, dword ptr [ebp - 0xac]
00416492  51                                 push       ecx
00416493  8b 95 60 ff ff ff                  mov        edx, dword ptr [ebp - 0xa0]
00416499  52                                 push       edx
0041649a  8b 85 78 ff ff ff                  mov        eax, dword ptr [ebp - 0x88]
004164a0  50                                 push       eax
004164a1  e8 7a d9 12 00                     call       0x543e20
004164a6  83 c4 0c                           add        esp, 0xc
004164a9  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
004164ac  8b 91 00 01 00 00                  mov        edx, dword ptr [ecx + 0x100]
004164b2  89 95 50 ff ff ff                  mov        dword ptr [ebp - 0xb0], edx
004164b8  83 bd 50 ff ff ff 15               cmp        dword ptr [ebp - 0xb0], 0x15
004164bf  74 12                              je         0x4164d3
004164c1  83 bd 50 ff ff ff 1a               cmp        dword ptr [ebp - 0xb0], 0x1a
004164c8  0f 84 7d 03 00 00                  je         0x41684b
004164ce  e9 d8 07 00 00                     jmp        0x416cab
004164d3  f3 0f 10 45 18                     movss      xmm0, dword ptr [ebp + 0x18]
004164d8  f3 0f 59 45 18                     mulss      xmm0, dword ptr [ebp + 0x18]
004164dd  f3 0f 11 85 5c ff ff ff            movss      dword ptr [ebp - 0xa4], xmm0
004164e5  6a 10                              push       0x10
004164e7  8d 4d c4                           lea        ecx, [ebp - 0x3c]
004164ea  e8 91 5b ff ff                     call       0x40c080
004164ef  8d 4d c4                           lea        ecx, [ebp - 0x3c]
004164f2  e8 c9 dc ff ff                     call       0x4141c0
004164f7  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004164fe  c7 45 94 00 00 00 00               mov        dword ptr [ebp - 0x6c], 0
00416505  eb 09                              jmp        0x416510
00416507  8b 45 94                           mov        eax, dword ptr [ebp - 0x6c]
0041650a  83 c0 01                           add        eax, 1
0041650d  89 45 94                           mov        dword ptr [ebp - 0x6c], eax
00416510  f3 0f 2c 8d 5c ff ff ff            cvttss2si  ecx, dword ptr [ebp - 0xa4]
00416518  39 4d 94                           cmp        dword ptr [ebp - 0x6c], ecx
0041651b  7d 75                              jge        0x416592
0041651d  c7 45 90 00 00 00 00               mov        dword ptr [ebp - 0x70], 0
00416524  eb 09                              jmp        0x41652f
00416526  8b 55 90                           mov        edx, dword ptr [ebp - 0x70]
00416529  83 c2 01                           add        edx, 1
0041652c  89 55 90                           mov        dword ptr [ebp - 0x70], edx
0041652f  f3 0f 2c 85 5c ff ff ff            cvttss2si  eax, dword ptr [ebp - 0xa4]
00416537  39 45 90                           cmp        dword ptr [ebp - 0x70], eax
0041653a  7d 51                              jge        0x41658d
0041653c  8b 4d 94                           mov        ecx, dword ptr [ebp - 0x6c]
0041653f  0f af 4d 94                        imul       ecx, dword ptr [ebp - 0x6c]
00416543  8b 55 90                           mov        edx, dword ptr [ebp - 0x70]
00416546  0f af 55 90                        imul       edx, dword ptr [ebp - 0x70]
0041654a  03 ca                              add        ecx, edx
0041654c  f3 0f 2a c1                        cvtsi2ss   xmm0, ecx
00416550  f3 0f 10 8d 5c ff ff ff            movss      xmm1, dword ptr [ebp - 0xa4]
00416558  0f 2f c8                           comiss     xmm1, xmm0
0041655b  76 2e                              jbe        0x41658b
0041655d  83 7d 94 00                        cmp        dword ptr [ebp - 0x6c], 0
00416561  75 06                              jne        0x416569
00416563  83 7d 90 00                        cmp        dword ptr [ebp - 0x70], 0
00416567  74 22                              je         0x41658b
00416569  8b 45 94                           mov        eax, dword ptr [ebp - 0x6c]
0041656c  89 85 30 ff ff ff                  mov        dword ptr [ebp - 0xd0], eax
00416572  8b 4d 90                           mov        ecx, dword ptr [ebp - 0x70]
00416575  89 8d 34 ff ff ff                  mov        dword ptr [ebp - 0xcc], ecx
0041657b  8d 95 30 ff ff ff                  lea        edx, [ebp - 0xd0]
00416581  52                                 push       edx
00416582  8d 4d c4                           lea        ecx, [ebp - 0x3c]
00416585  e8 a6 fd ff ff                     call       0x416330
0041658a  90                                 nop        
0041658b  eb 99                              jmp        0x416526
0041658d  e9 75 ff ff ff                     jmp        0x416507
00416592  8b 85 78 ff ff ff                  mov        eax, dword ptr [ebp - 0x88]
00416598  89 85 68 ff ff ff                  mov        dword ptr [ebp - 0x98], eax
0041659e  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
004165a1  51                                 push       ecx
004165a2  8d 4d e8                           lea        ecx, [ebp - 0x18]
004165a5  e8 f6 dc ff ff                     call       0x4142a0
004165aa  c7 85 24 ff ff ff 04 00 00 00      mov        dword ptr [ebp - 0xdc], 4
004165b4  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
004165b7  e8 04 f1 ff ff                     call       0x4156c0
004165bc  99                                 cdq        
004165bd  83 e2 03                           and        edx, 3
004165c0  03 c2                              add        eax, edx
004165c2  c1 f8 02                           sar        eax, 2
004165c5  89 85 6c ff ff ff                  mov        dword ptr [ebp - 0x94], eax
004165cb  0f b6 55 eb                        movzx      edx, byte ptr [ebp - 0x15]
004165cf  81 f2 ff 00 00 00                  xor        edx, 0xff
004165d5  88 55 eb                           mov        byte ptr [ebp - 0x15], dl
004165d8  c7 45 b4 00 00 00 00               mov        dword ptr [ebp - 0x4c], 0
004165df  c7 85 58 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0xa8], 0
004165e9  eb 1d                              jmp        0x416608
004165eb  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
004165ee  83 c0 01                           add        eax, 1
004165f1  89 45 b4                           mov        dword ptr [ebp - 0x4c], eax
004165f4  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
004165f7  e8 c4 f0 ff ff                     call       0x4156c0
004165fc  03 85 58 ff ff ff                  add        eax, dword ptr [ebp - 0xa8]
00416602  89 85 58 ff ff ff                  mov        dword ptr [ebp - 0xa8], eax
00416608  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
0041660b  3b 4d 08                           cmp        ecx, dword ptr [ebp + 8]
0041660e  0f 8d 22 02 00 00                  jge        0x416836
00416614  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
00416617  8b 85 58 ff ff ff                  mov        eax, dword ptr [ebp - 0xa8]
0041661d  8d 0c 90                           lea        ecx, [eax + edx*4]
00416620  03 8d 60 ff ff ff                  add        ecx, dword ptr [ebp - 0xa0]
00416626  89 4d 88                           mov        dword ptr [ebp - 0x78], ecx
00416629  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0041662c  89 55 b0                           mov        dword ptr [ebp - 0x50], edx
0041662f  eb 12                              jmp        0x416643
00416631  8b 45 b0                           mov        eax, dword ptr [ebp - 0x50]
00416634  83 c0 01                           add        eax, 1
00416637  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
0041663a  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
0041663d  83 c1 04                           add        ecx, 4
00416640  89 4d 88                           mov        dword ptr [ebp - 0x78], ecx
00416643  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
00416646  3b 55 10                           cmp        edx, dword ptr [ebp + 0x10]
00416649  0f 8d e2 01 00 00                  jge        0x416831
0041664f  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
00416652  0f b6 48 03                        movzx      ecx, byte ptr [eax + 3]
00416656  85 c9                              test       ecx, ecx
00416658  75 02                              jne        0x41665c
0041665a  eb d5                              jmp        0x416631
0041665c  8d 55 c4                           lea        edx, [ebp - 0x3c]
0041665f  89 95 40 ff ff ff                  mov        dword ptr [ebp - 0xc0], edx
00416665  8b 8d 40 ff ff ff                  mov        ecx, dword ptr [ebp - 0xc0]
0041666b  e8 c0 e8 ff ff                     call       0x414f30
00416670  89 85 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], eax
00416676  8b 8d 40 ff ff ff                  mov        ecx, dword ptr [ebp - 0xc0]
0041667c  e8 ef e8 ff ff                     call       0x414f70
00416681  89 85 3c ff ff ff                  mov        dword ptr [ebp - 0xc4], eax
00416687  eb 0f                              jmp        0x416698
00416689  8b 85 74 ff ff ff                  mov        eax, dword ptr [ebp - 0x8c]
0041668f  83 c0 08                           add        eax, 8
00416692  89 85 74 ff ff ff                  mov        dword ptr [ebp - 0x8c], eax
00416698  8b 8d 74 ff ff ff                  mov        ecx, dword ptr [ebp - 0x8c]
0041669e  3b 8d 3c ff ff ff                  cmp        ecx, dword ptr [ebp - 0xc4]
004166a4  0f 84 5b 01 00 00                  je         0x416805
004166aa  8b 95 74 ff ff ff                  mov        edx, dword ptr [ebp - 0x8c]
004166b0  89 95 4c ff ff ff                  mov        dword ptr [ebp - 0xb4], edx
004166b6  6a 00                              push       0
004166b8  8b 8d 4c ff ff ff                  mov        ecx, dword ptr [ebp - 0xb4]
004166be  e8 bd de ff ff                     call       0x414580
004166c3  8b 00                              mov        eax, dword ptr [eax]
004166c5  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
004166c8  6a 01                              push       1
004166ca  8b 8d 4c ff ff ff                  mov        ecx, dword ptr [ebp - 0xb4]
004166d0  e8 ab de ff ff                     call       0x414580
004166d5  8b 08                              mov        ecx, dword ptr [eax]
004166d7  89 4d a8                           mov        dword ptr [ebp - 0x58], ecx
004166da  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
004166dd  2b 55 a4                           sub        edx, dword ptr [ebp - 0x5c]
004166e0  78 3c                              js         0x41671e
004166e2  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
004166e5  2b 45 a8                           sub        eax, dword ptr [ebp - 0x58]
004166e8  78 34                              js         0x41671e
004166ea  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
004166ed  2b 4d a8                           sub        ecx, dword ptr [ebp - 0x58]
004166f0  0f af 8d 6c ff ff ff               imul       ecx, dword ptr [ebp - 0x94]
004166f7  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
004166fa  2b 55 a4                           sub        edx, dword ptr [ebp - 0x5c]
004166fd  03 ca                              add        ecx, edx
004166ff  8b 85 68 ff ff ff                  mov        eax, dword ptr [ebp - 0x98]
00416705  8d 0c 88                           lea        ecx, [eax + ecx*4]
00416708  89 4d 8c                           mov        dword ptr [ebp - 0x74], ecx
0041670b  8b 55 8c                           mov        edx, dword ptr [ebp - 0x74]
0041670e  0f b6 42 03                        movzx      eax, byte ptr [edx + 3]
00416712  3d ff 00 00 00                     cmp        eax, 0xff
00416717  74 05                              je         0x41671e
00416719  e9 06 01 00 00                     jmp        0x416824
0041671e  8b 75 b0                           mov        esi, dword ptr [ebp - 0x50]
00416721  03 75 a4                           add        esi, dword ptr [ebp - 0x5c]
00416724  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416727  e8 d4 f0 ff ff                     call       0x415800
0041672c  3b f0                              cmp        esi, eax
0041672e  7d 3d                              jge        0x41676d
00416730  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
00416733  2b 4d a8                           sub        ecx, dword ptr [ebp - 0x58]
00416736  78 35                              js         0x41676d
00416738  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
0041673b  2b 55 a8                           sub        edx, dword ptr [ebp - 0x58]
0041673e  0f af 95 6c ff ff ff               imul       edx, dword ptr [ebp - 0x94]
00416745  8b 45 b0                           mov        eax, dword ptr [ebp - 0x50]
00416748  03 45 a4                           add        eax, dword ptr [ebp - 0x5c]
0041674b  03 d0                              add        edx, eax
0041674d  8b 8d 68 ff ff ff                  mov        ecx, dword ptr [ebp - 0x98]
00416753  8d 14 91                           lea        edx, [ecx + edx*4]
00416756  89 55 8c                           mov        dword ptr [ebp - 0x74], edx
00416759  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
0041675c  0f b6 48 03                        movzx      ecx, byte ptr [eax + 3]
00416760  81 f9 ff 00 00 00                  cmp        ecx, 0xff
00416766  74 05                              je         0x41676d
00416768  e9 b7 00 00 00                     jmp        0x416824
0041676d  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
00416770  2b 55 a4                           sub        edx, dword ptr [ebp - 0x5c]
00416773  78 3c                              js         0x4167b1
00416775  8b 45 b4                           mov        eax, dword ptr [ebp - 0x4c]
00416778  03 45 a8                           add        eax, dword ptr [ebp - 0x58]
0041677b  3b 45 08                           cmp        eax, dword ptr [ebp + 8]
0041677e  7d 31                              jge        0x4167b1
00416780  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
00416783  03 4d a8                           add        ecx, dword ptr [ebp - 0x58]
00416786  0f af 8d 6c ff ff ff               imul       ecx, dword ptr [ebp - 0x94]
0041678d  8b 55 b0                           mov        edx, dword ptr [ebp - 0x50]
00416790  2b 55 a4                           sub        edx, dword ptr [ebp - 0x5c]
00416793  03 ca                              add        ecx, edx
00416795  8b 85 68 ff ff ff                  mov        eax, dword ptr [ebp - 0x98]
0041679b  8d 0c 88                           lea        ecx, [eax + ecx*4]
0041679e  89 4d 8c                           mov        dword ptr [ebp - 0x74], ecx
004167a1  8b 55 8c                           mov        edx, dword ptr [ebp - 0x74]
004167a4  0f b6 42 03                        movzx      eax, byte ptr [edx + 3]
004167a8  3d ff 00 00 00                     cmp        eax, 0xff
004167ad  74 02                              je         0x4167b1
004167af  eb 73                              jmp        0x416824
004167b1  8b 75 b0                           mov        esi, dword ptr [ebp - 0x50]
004167b4  03 75 a4                           add        esi, dword ptr [ebp - 0x5c]
004167b7  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
004167ba  e8 41 f0 ff ff                     call       0x415800
004167bf  3b f0                              cmp        esi, eax
004167c1  7d 3d                              jge        0x416800
004167c3  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
004167c6  03 4d a8                           add        ecx, dword ptr [ebp - 0x58]
004167c9  3b 4d 08                           cmp        ecx, dword ptr [ebp + 8]
004167cc  7d 32                              jge        0x416800
004167ce  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
004167d1  03 55 a8                           add        edx, dword ptr [ebp - 0x58]
004167d4  0f af 95 6c ff ff ff               imul       edx, dword ptr [ebp - 0x94]
004167db  8b 45 b0                           mov        eax, dword ptr [ebp - 0x50]
004167de  03 45 a4                           add        eax, dword ptr [ebp - 0x5c]
004167e1  03 d0                              add        edx, eax
004167e3  8b 8d 68 ff ff ff                  mov        ecx, dword ptr [ebp - 0x98]
004167e9  8d 14 91                           lea        edx, [ecx + edx*4]
004167ec  89 55 8c                           mov        dword ptr [ebp - 0x74], edx
004167ef  8b 45 8c                           mov        eax, dword ptr [ebp - 0x74]
004167f2  0f b6 48 03                        movzx      ecx, byte ptr [eax + 3]
004167f6  81 f9 ff 00 00 00                  cmp        ecx, 0xff
004167fc  74 02                              je         0x416800
004167fe  eb 24                              jmp        0x416824
00416800  e9 84 fe ff ff                     jmp        0x416689
00416805  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
00416808  8a 45 ea                           mov        al, byte ptr [ebp - 0x16]
0041680b  88 42 02                           mov        byte ptr [edx + 2], al
0041680e  8b 4d 88                           mov        ecx, dword ptr [ebp - 0x78]
00416811  8a 55 e9                           mov        dl, byte ptr [ebp - 0x17]
00416814  88 51 01                           mov        byte ptr [ecx + 1], dl
00416817  8b 45 88                           mov        eax, dword ptr [ebp - 0x78]
0041681a  8a 4d e8                           mov        cl, byte ptr [ebp - 0x18]
0041681d  88 08                              mov        byte ptr [eax], cl
0041681f  e9 0d fe ff ff                     jmp        0x416631
00416824  8b 55 88                           mov        edx, dword ptr [ebp - 0x78]
00416827  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
0041682a  89 02                              mov        dword ptr [edx], eax
0041682c  e9 00 fe ff ff                     jmp        0x416631
00416831  e9 b5 fd ff ff                     jmp        0x4165eb
00416836  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
0041683d  8d 4d c4                           lea        ecx, [ebp - 0x3c]
00416840  e8 fb db ff ff                     call       0x414440
00416845  90                                 nop        
00416846  e9 60 04 00 00                     jmp        0x416cab
0041684b  f3 0f 10 45 18                     movss      xmm0, dword ptr [ebp + 0x18]
00416850  f3 0f 59 45 18                     mulss      xmm0, dword ptr [ebp + 0x18]
00416855  f3 0f 11 85 64 ff ff ff            movss      dword ptr [ebp - 0x9c], xmm0
0041685d  6a 10                              push       0x10
0041685f  8d 4d d4                           lea        ecx, [ebp - 0x2c]
00416862  e8 19 58 ff ff                     call       0x40c080
00416867  8d 4d d4                           lea        ecx, [ebp - 0x2c]
0041686a  e8 51 d9 ff ff                     call       0x4141c0
0041686f  c7 45 fc 01 00 00 00               mov        dword ptr [ebp - 4], 1
00416876  c7 45 84 00 00 00 00               mov        dword ptr [ebp - 0x7c], 0
0041687d  eb 09                              jmp        0x416888
0041687f  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
00416882  83 c1 01                           add        ecx, 1
00416885  89 4d 84                           mov        dword ptr [ebp - 0x7c], ecx
00416888  f3 0f 2c 95 64 ff ff ff            cvttss2si  edx, dword ptr [ebp - 0x9c]
00416890  39 55 84                           cmp        dword ptr [ebp - 0x7c], edx
00416893  7d 75                              jge        0x41690a
00416895  c7 45 80 00 00 00 00               mov        dword ptr [ebp - 0x80], 0
0041689c  eb 09                              jmp        0x4168a7
0041689e  8b 45 80                           mov        eax, dword ptr [ebp - 0x80]
004168a1  83 c0 01                           add        eax, 1
004168a4  89 45 80                           mov        dword ptr [ebp - 0x80], eax
004168a7  f3 0f 2c 8d 64 ff ff ff            cvttss2si  ecx, dword ptr [ebp - 0x9c]
004168af  39 4d 80                           cmp        dword ptr [ebp - 0x80], ecx
004168b2  7d 51                              jge        0x416905
004168b4  8b 55 84                           mov        edx, dword ptr [ebp - 0x7c]
004168b7  0f af 55 84                        imul       edx, dword ptr [ebp - 0x7c]
004168bb  8b 45 80                           mov        eax, dword ptr [ebp - 0x80]
004168be  0f af 45 80                        imul       eax, dword ptr [ebp - 0x80]
004168c2  03 d0                              add        edx, eax
004168c4  f3 0f 2a c2                        cvtsi2ss   xmm0, edx
004168c8  f3 0f 10 8d 64 ff ff ff            movss      xmm1, dword ptr [ebp - 0x9c]
004168d0  0f 2f c8                           comiss     xmm1, xmm0
004168d3  76 2e                              jbe        0x416903
004168d5  83 7d 84 00                        cmp        dword ptr [ebp - 0x7c], 0
004168d9  75 06                              jne        0x4168e1
004168db  83 7d 80 00                        cmp        dword ptr [ebp - 0x80], 0
004168df  74 22                              je         0x416903
004168e1  8b 4d 84                           mov        ecx, dword ptr [ebp - 0x7c]
004168e4  89 8d 28 ff ff ff                  mov        dword ptr [ebp - 0xd8], ecx
004168ea  8b 55 80                           mov        edx, dword ptr [ebp - 0x80]
004168ed  89 95 2c ff ff ff                  mov        dword ptr [ebp - 0xd4], edx
004168f3  8d 85 28 ff ff ff                  lea        eax, [ebp - 0xd8]
004168f9  50                                 push       eax
004168fa  8d 4d d4                           lea        ecx, [ebp - 0x2c]
004168fd  e8 2e fa ff ff                     call       0x416330
00416902  90                                 nop        
00416903  eb 99                              jmp        0x41689e
00416905  e9 75 ff ff ff                     jmp        0x41687f
0041690a  8b 8d 78 ff ff ff                  mov        ecx, dword ptr [ebp - 0x88]
00416910  89 8d 7c ff ff ff                  mov        dword ptr [ebp - 0x84], ecx
00416916  8b 55 14                           mov        edx, dword ptr [ebp + 0x14]
00416919  52                                 push       edx
0041691a  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0041691d  e8 7e d9 ff ff                     call       0x4142a0
00416922  8d 4d ec                           lea        ecx, [ebp - 0x14]
00416925  e8 56 d9 ff ff                     call       0x414280
0041692a  0f b6 45 e6                        movzx      eax, byte ptr [ebp - 0x1a]
0041692e  c1 f8 04                           sar        eax, 4
00416931  66 83 e0 0f                        and        ax, 0xf
00416935  66 c1 e0 08                        shl        ax, 8
00416939  b9 ff f0 00 00                     mov        ecx, 0xf0ff
0041693e  66 23 4d ec                        and        cx, word ptr [ebp - 0x14]
00416942  66 0b c8                           or         cx, ax
00416945  66 89 4d ec                        mov        word ptr [ebp - 0x14], cx
00416949  0f b6 55 e5                        movzx      edx, byte ptr [ebp - 0x1b]
0041694d  c1 fa 04                           sar        edx, 4
00416950  66 83 e2 0f                        and        dx, 0xf
00416954  66 c1 e2 04                        shl        dx, 4
00416958  b8 0f ff 00 00                     mov        eax, 0xff0f
0041695d  66 23 45 ec                        and        ax, word ptr [ebp - 0x14]
00416961  66 0b c2                           or         ax, dx
00416964  66 89 45 ec                        mov        word ptr [ebp - 0x14], ax
00416968  0f b6 4d e4                        movzx      ecx, byte ptr [ebp - 0x1c]
0041696c  c1 f9 04                           sar        ecx, 4
0041696f  66 83 e1 0f                        and        cx, 0xf
00416973  ba f0 ff 00 00                     mov        edx, 0xfff0
00416978  66 23 55 ec                        and        dx, word ptr [ebp - 0x14]
0041697c  66 0b d1                           or         dx, cx
0041697f  66 89 55 ec                        mov        word ptr [ebp - 0x14], dx
00416983  0f b6 45 e7                        movzx      eax, byte ptr [ebp - 0x19]
00416987  c1 f8 04                           sar        eax, 4
0041698a  83 f0 0f                           xor        eax, 0xf
0041698d  66 83 e0 0f                        and        ax, 0xf
00416991  66 c1 e0 0c                        shl        ax, 0xc
00416995  b9 ff 0f 00 00                     mov        ecx, 0xfff
0041699a  66 23 4d ec                        and        cx, word ptr [ebp - 0x14]
0041699e  66 0b c8                           or         cx, ax
004169a1  66 89 4d ec                        mov        word ptr [ebp - 0x14], cx
004169a5  c7 45 bc 00 00 00 00               mov        dword ptr [ebp - 0x44], 0
004169ac  eb 09                              jmp        0x4169b7
004169ae  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
004169b1  83 c2 01                           add        edx, 1
004169b4  89 55 bc                           mov        dword ptr [ebp - 0x44], edx
004169b7  8b 45 bc                           mov        eax, dword ptr [ebp - 0x44]
004169ba  3b 45 08                           cmp        eax, dword ptr [ebp + 8]
004169bd  0f 8d d8 02 00 00                  jge        0x416c9b
004169c3  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
004169c6  e8 f5 ec ff ff                     call       0x4156c0
004169cb  0f af 45 bc                        imul       eax, dword ptr [ebp - 0x44]
004169cf  03 85 60 ff ff ff                  add        eax, dword ptr [ebp - 0xa0]
004169d5  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004169d8  8d 14 48                           lea        edx, [eax + ecx*2]
004169db  89 55 ac                           mov        dword ptr [ebp - 0x54], edx
004169de  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004169e1  89 45 b8                           mov        dword ptr [ebp - 0x48], eax
004169e4  eb 12                              jmp        0x4169f8
004169e6  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
004169e9  83 c1 01                           add        ecx, 1
004169ec  89 4d b8                           mov        dword ptr [ebp - 0x48], ecx
004169ef  8b 55 ac                           mov        edx, dword ptr [ebp - 0x54]
004169f2  83 c2 02                           add        edx, 2
004169f5  89 55 ac                           mov        dword ptr [ebp - 0x54], edx
004169f8  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
004169fb  3b 45 10                           cmp        eax, dword ptr [ebp + 0x10]
004169fe  0f 8d 92 02 00 00                  jge        0x416c96
00416a04  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416a07  e8 b4 ec ff ff                     call       0x4156c0
00416a0c  0f af 45 bc                        imul       eax, dword ptr [ebp - 0x44]
00416a10  99                                 cdq        
00416a11  2b c2                              sub        eax, edx
00416a13  d1 f8                              sar        eax, 1
00416a15  03 45 b8                           add        eax, dword ptr [ebp - 0x48]
00416a18  8b 8d 7c ff ff ff                  mov        ecx, dword ptr [ebp - 0x84]
00416a1e  8d 14 41                           lea        edx, [ecx + eax*2]
00416a21  89 55 98                           mov        dword ptr [ebp - 0x68], edx
00416a24  8d 45 d4                           lea        eax, [ebp - 0x2c]
00416a27  89 85 48 ff ff ff                  mov        dword ptr [ebp - 0xb8], eax
00416a2d  8b 8d 48 ff ff ff                  mov        ecx, dword ptr [ebp - 0xb8]
00416a33  e8 f8 e4 ff ff                     call       0x414f30
00416a38  89 85 70 ff ff ff                  mov        dword ptr [ebp - 0x90], eax
00416a3e  8b 8d 48 ff ff ff                  mov        ecx, dword ptr [ebp - 0xb8]
00416a44  e8 27 e5 ff ff                     call       0x414f70
00416a49  89 85 38 ff ff ff                  mov        dword ptr [ebp - 0xc8], eax
00416a4f  eb 0f                              jmp        0x416a60
00416a51  8b 8d 70 ff ff ff                  mov        ecx, dword ptr [ebp - 0x90]
00416a57  83 c1 08                           add        ecx, 8
00416a5a  89 8d 70 ff ff ff                  mov        dword ptr [ebp - 0x90], ecx
00416a60  8b 95 70 ff ff ff                  mov        edx, dword ptr [ebp - 0x90]
00416a66  3b 95 38 ff ff ff                  cmp        edx, dword ptr [ebp - 0xc8]
00416a6c  0f 84 a0 01 00 00                  je         0x416c12
00416a72  8b 85 70 ff ff ff                  mov        eax, dword ptr [ebp - 0x90]
00416a78  89 85 44 ff ff ff                  mov        dword ptr [ebp - 0xbc], eax
00416a7e  6a 00                              push       0
00416a80  8b 8d 44 ff ff ff                  mov        ecx, dword ptr [ebp - 0xbc]
00416a86  e8 f5 da ff ff                     call       0x414580
00416a8b  8b 08                              mov        ecx, dword ptr [eax]
00416a8d  89 4d 9c                           mov        dword ptr [ebp - 0x64], ecx
00416a90  6a 01                              push       1
00416a92  8b 8d 44 ff ff ff                  mov        ecx, dword ptr [ebp - 0xbc]
00416a98  e8 e3 da ff ff                     call       0x414580
00416a9d  8b 10                              mov        edx, dword ptr [eax]
00416a9f  89 55 a0                           mov        dword ptr [ebp - 0x60], edx
00416aa2  8b 45 b8                           mov        eax, dword ptr [ebp - 0x48]
00416aa5  2b 45 9c                           sub        eax, dword ptr [ebp - 0x64]
00416aa8  78 4d                              js         0x416af7
00416aaa  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
00416aad  2b 4d a0                           sub        ecx, dword ptr [ebp - 0x60]
00416ab0  78 45                              js         0x416af7
00416ab2  8b 75 bc                           mov        esi, dword ptr [ebp - 0x44]
00416ab5  2b 75 a0                           sub        esi, dword ptr [ebp - 0x60]
00416ab8  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416abb  e8 00 ec ff ff                     call       0x4156c0
00416ac0  0f af c6                           imul       eax, esi
00416ac3  99                                 cdq        
00416ac4  2b c2                              sub        eax, edx
00416ac6  d1 f8                              sar        eax, 1
00416ac8  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
00416acb  2b 55 9c                           sub        edx, dword ptr [ebp - 0x64]
00416ace  03 c2                              add        eax, edx
00416ad0  8b 8d 7c ff ff ff                  mov        ecx, dword ptr [ebp - 0x84]
00416ad6  8d 14 41                           lea        edx, [ecx + eax*2]
00416ad9  89 55 98                           mov        dword ptr [ebp - 0x68], edx
00416adc  8b 45 98                           mov        eax, dword ptr [ebp - 0x68]
00416adf  66 8b 08                           mov        cx, word ptr [eax]
00416ae2  66 c1 e9 0c                        shr        cx, 0xc
00416ae6  66 83 e1 0f                        and        cx, 0xf
00416aea  0f b7 d1                           movzx      edx, cx
00416aed  83 fa 0f                           cmp        edx, 0xf
00416af0  74 05                              je         0x416af7
00416af2  e9 90 01 00 00                     jmp        0x416c87
00416af7  8b 75 b8                           mov        esi, dword ptr [ebp - 0x48]
00416afa  03 75 9c                           add        esi, dword ptr [ebp - 0x64]
00416afd  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416b00  e8 fb ec ff ff                     call       0x415800
00416b05  3b f0                              cmp        esi, eax
00416b07  7d 4d                              jge        0x416b56
00416b09  8b 45 bc                           mov        eax, dword ptr [ebp - 0x44]
00416b0c  2b 45 a0                           sub        eax, dword ptr [ebp - 0x60]
00416b0f  78 45                              js         0x416b56
00416b11  8b 75 bc                           mov        esi, dword ptr [ebp - 0x44]
00416b14  2b 75 a0                           sub        esi, dword ptr [ebp - 0x60]
00416b17  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416b1a  e8 a1 eb ff ff                     call       0x4156c0
00416b1f  0f af c6                           imul       eax, esi
00416b22  99                                 cdq        
00416b23  2b c2                              sub        eax, edx
00416b25  d1 f8                              sar        eax, 1
00416b27  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
00416b2a  03 4d 9c                           add        ecx, dword ptr [ebp - 0x64]
00416b2d  03 c1                              add        eax, ecx
00416b2f  8b 95 7c ff ff ff                  mov        edx, dword ptr [ebp - 0x84]
00416b35  8d 04 42                           lea        eax, [edx + eax*2]
00416b38  89 45 98                           mov        dword ptr [ebp - 0x68], eax
00416b3b  8b 4d 98                           mov        ecx, dword ptr [ebp - 0x68]
00416b3e  66 8b 11                           mov        dx, word ptr [ecx]
00416b41  66 c1 ea 0c                        shr        dx, 0xc
00416b45  66 83 e2 0f                        and        dx, 0xf
00416b49  0f b7 c2                           movzx      eax, dx
00416b4c  83 f8 0f                           cmp        eax, 0xf
00416b4f  74 05                              je         0x416b56
00416b51  e9 31 01 00 00                     jmp        0x416c87
00416b56  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
00416b59  2b 4d 9c                           sub        ecx, dword ptr [ebp - 0x64]
00416b5c  78 50                              js         0x416bae
00416b5e  8b 55 bc                           mov        edx, dword ptr [ebp - 0x44]
00416b61  03 55 a0                           add        edx, dword ptr [ebp - 0x60]
00416b64  3b 55 08                           cmp        edx, dword ptr [ebp + 8]
00416b67  7d 45                              jge        0x416bae
00416b69  8b 75 bc                           mov        esi, dword ptr [ebp - 0x44]
00416b6c  03 75 a0                           add        esi, dword ptr [ebp - 0x60]
00416b6f  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416b72  e8 49 eb ff ff                     call       0x4156c0
00416b77  0f af c6                           imul       eax, esi
00416b7a  99                                 cdq        
00416b7b  2b c2                              sub        eax, edx
00416b7d  d1 f8                              sar        eax, 1
00416b7f  8b 4d b8                           mov        ecx, dword ptr [ebp - 0x48]
00416b82  2b 4d 9c                           sub        ecx, dword ptr [ebp - 0x64]
00416b85  03 c1                              add        eax, ecx
00416b87  8b 95 7c ff ff ff                  mov        edx, dword ptr [ebp - 0x84]
00416b8d  8d 04 42                           lea        eax, [edx + eax*2]
00416b90  89 45 98                           mov        dword ptr [ebp - 0x68], eax
00416b93  8b 4d 98                           mov        ecx, dword ptr [ebp - 0x68]
00416b96  66 8b 11                           mov        dx, word ptr [ecx]
00416b99  66 c1 ea 0c                        shr        dx, 0xc
00416b9d  66 83 e2 0f                        and        dx, 0xf
00416ba1  0f b7 c2                           movzx      eax, dx
00416ba4  83 f8 0f                           cmp        eax, 0xf
00416ba7  74 05                              je         0x416bae
00416ba9  e9 d9 00 00 00                     jmp        0x416c87
00416bae  8b 75 b8                           mov        esi, dword ptr [ebp - 0x48]
00416bb1  03 75 9c                           add        esi, dword ptr [ebp - 0x64]
00416bb4  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416bb7  e8 44 ec ff ff                     call       0x415800
00416bbc  3b f0                              cmp        esi, eax
00416bbe  7d 4d                              jge        0x416c0d
00416bc0  8b 4d bc                           mov        ecx, dword ptr [ebp - 0x44]
00416bc3  03 4d a0                           add        ecx, dword ptr [ebp - 0x60]
00416bc6  3b 4d 08                           cmp        ecx, dword ptr [ebp + 8]
00416bc9  7d 42                              jge        0x416c0d
00416bcb  8b 75 bc                           mov        esi, dword ptr [ebp - 0x44]
00416bce  03 75 a0                           add        esi, dword ptr [ebp - 0x60]
00416bd1  8b 4d c0                           mov        ecx, dword ptr [ebp - 0x40]
00416bd4  e8 e7 ea ff ff                     call       0x4156c0
00416bd9  0f af c6                           imul       eax, esi
00416bdc  99                                 cdq        
00416bdd  2b c2                              sub        eax, edx
00416bdf  d1 f8                              sar        eax, 1
00416be1  8b 55 b8                           mov        edx, dword ptr [ebp - 0x48]
00416be4  03 55 9c                           add        edx, dword ptr [ebp - 0x64]
00416be7  03 c2                              add        eax, edx
00416be9  8b 8d 7c ff ff ff                  mov        ecx, dword ptr [ebp - 0x84]
00416bef  8d 14 41                           lea        edx, [ecx + eax*2]
00416bf2  89 55 98                           mov        dword ptr [ebp - 0x68], edx
00416bf5  8b 45 98                           mov        eax, dword ptr [ebp - 0x68]
00416bf8  66 8b 08                           mov        cx, word ptr [eax]
00416bfb  66 c1 e9 0c                        shr        cx, 0xc
00416bff  66 83 e1 0f                        and        cx, 0xf
00416c03  0f b7 d1                           movzx      edx, cx
00416c06  83 fa 0f                           cmp        edx, 0xf
00416c09  74 02                              je         0x416c0d
00416c0b  eb 7a                              jmp        0x416c87
00416c0d  e9 3f fe ff ff                     jmp        0x416a51
00416c12  66 8b 45 ec                        mov        ax, word ptr [ebp - 0x14]
00416c16  66 c1 e8 08                        shr        ax, 8
00416c1a  66 83 e0 0f                        and        ax, 0xf
00416c1e  66 83 e0 0f                        and        ax, 0xf
00416c22  66 c1 e0 08                        shl        ax, 8
00416c26  b9 ff f0 00 00                     mov        ecx, 0xf0ff
00416c2b  8b 55 ac                           mov        edx, dword ptr [ebp - 0x54]
00416c2e  66 23 0a                           and        cx, word ptr [edx]
00416c31  66 0b c8                           or         cx, ax
00416c34  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
00416c37  66 89 08                           mov        word ptr [eax], cx
00416c3a  66 8b 4d ec                        mov        cx, word ptr [ebp - 0x14]
00416c3e  66 c1 e9 04                        shr        cx, 4
00416c42  66 83 e1 0f                        and        cx, 0xf
00416c46  66 83 e1 0f                        and        cx, 0xf
00416c4a  66 c1 e1 04                        shl        cx, 4
00416c4e  ba 0f ff 00 00                     mov        edx, 0xff0f
00416c53  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
00416c56  66 23 10                           and        dx, word ptr [eax]
00416c59  66 0b d1                           or         dx, cx
00416c5c  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
00416c5f  66 89 11                           mov        word ptr [ecx], dx
00416c62  66 8b 55 ec                        mov        dx, word ptr [ebp - 0x14]
00416c66  66 83 e2 0f                        and        dx, 0xf
00416c6a  66 83 e2 0f                        and        dx, 0xf
00416c6e  b8 f0 ff 00 00                     mov        eax, 0xfff0
00416c73  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
00416c76  66 23 01                           and        ax, word ptr [ecx]
00416c79  66 0b c2                           or         ax, dx
00416c7c  8b 55 ac                           mov        edx, dword ptr [ebp - 0x54]
00416c7f  66 89 02                           mov        word ptr [edx], ax
00416c82  e9 5f fd ff ff                     jmp        0x4169e6
00416c87  8b 45 ac                           mov        eax, dword ptr [ebp - 0x54]
00416c8a  66 8b 4d ec                        mov        cx, word ptr [ebp - 0x14]
00416c8e  66 89 08                           mov        word ptr [eax], cx
00416c91  e9 50 fd ff ff                     jmp        0x4169e6
00416c96  e9 13 fd ff ff                     jmp        0x4169ae
00416c9b  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
00416ca2  8d 4d d4                           lea        ecx, [ebp - 0x2c]
00416ca5  e8 96 d7 ff ff                     call       0x414440
00416caa  90                                 nop        
00416cab  8b 95 78 ff ff ff                  mov        edx, dword ptr [ebp - 0x88]
00416cb1  52                                 push       edx
00416cb2  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
00416cb8  e8 b3 ce ff ff                     call       0x413b70
00416cbd  c7 85 78 ff ff ff 00 00 00 00      mov        dword ptr [ebp - 0x88], 0
00416cc7  b0 01                              mov        al, 1
00416cc9  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
00416ccc  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
00416cd3  59                                 pop        ecx
00416cd4  5e                                 pop        esi
00416cd5  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
00416cd8  33 cd                              xor        ecx, ebp
00416cda  e8 0f bd 12 00                     call       0x5429ee
00416cdf  8b e5                              mov        esp, ebp
00416ce1  5d                                 pop        ebp
00416ce2  c2 14 00                           ret        0x14
