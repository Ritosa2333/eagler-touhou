004da540  55                                 push       ebp
004da541  8b ec                              mov        ebp, esp
004da543  6a ff                              push       -1
004da545  68 7d 9d 56 00                     push       0x569d7d
004da54a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
004da550  50                                 push       eax
004da551  83 ec 38                           sub        esp, 0x38
004da554  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004da559  33 c5                              xor        eax, ebp
004da55b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
004da55e  56                                 push       esi
004da55f  50                                 push       eax
004da560  8d 45 f4                           lea        eax, [ebp - 0xc]
004da563  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004da569  89 4d e8                           mov        dword ptr [ebp - 0x18], ecx
004da56c  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da56f  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da572  8b 90 08 0b 00 00                  mov        edx, dword ptr [eax + 0xb08]
004da578  3b 91 0c 0b 00 00                  cmp        edx, dword ptr [ecx + 0xb0c]
004da57e  0f 84 17 05 00 00                  je         0x4daa9b
004da584  6a 05                              push       5
004da586  b9 40 02 5c 00                     mov        ecx, 0x5c0240
004da58b  e8 80 1d f3 ff                     call       0x40c310
004da590  50                                 push       eax
004da591  8d 4d ec                           lea        ecx, [ebp - 0x14]
004da594  e8 97 11 f3 ff                     call       0x40b730
004da599  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004da5a0  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da5a3  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da5a6  8b 91 08 0b 00 00                  mov        edx, dword ptr [ecx + 0xb08]
004da5ac  89 90 10 0b 00 00                  mov        dword ptr [eax + 0xb10], edx
004da5b2  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da5b5  8b 88 0c 0b 00 00                  mov        ecx, dword ptr [eax + 0xb0c]
004da5bb  51                                 push       ecx
004da5bc  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da5bf  8b 82 08 0b 00 00                  mov        eax, dword ptr [edx + 0xb08]
004da5c5  50                                 push       eax
004da5c6  68 70 1e 57 00                     push       0x571e70 ; string='scene %d -> %d'
004da5cb  e8 e0 20 f3 ff                     call       0x40c6b0
004da5d0  83 c4 0c                           add        esp, 0xc
004da5d3  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da5d6  c7 81 e0 0d 00 00 00 00 00 ff      mov        dword ptr [ecx + 0xde0], 0xff000000
004da5e0  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da5e3  8b 82 0c 0b 00 00                  mov        eax, dword ptr [edx + 0xb0c]
004da5e9  89 45 dc                           mov        dword ptr [ebp - 0x24], eax
004da5ec  83 7d dc 18                        cmp        dword ptr [ebp - 0x24], 0x18
004da5f0  0f 87 83 04 00 00                  ja         0x4daa79
004da5f6  8b 4d dc                           mov        ecx, dword ptr [ebp - 0x24]
004da5f9  0f b6 91 fc aa 4d 00               movzx      edx, byte ptr [ecx + 0x4daafc]
004da600  ff 24 95 bc aa 4d 00               jmp        dword ptr [edx*4 + 0x4daabc]
004da607  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da60a  c7 80 0c 0b 00 00 01 00 00 00      mov        dword ptr [eax + 0xb0c], 1
004da614  e8 a7 df ff ff                     call       0x4d85c0
004da619  89 45 d8                           mov        dword ptr [ebp - 0x28], eax
004da61c  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da61f  8b 55 d8                           mov        edx, dword ptr [ebp - 0x28]
004da622  89 91 c8 0d 00 00                  mov        dword ptr [ecx + 0xdc8], edx
004da628  83 7d d8 00                        cmp        dword ptr [ebp - 0x28], 0
004da62c  75 12                              jne        0x4da640
004da62e  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da631  c7 80 0c 0b 00 00 03 00 00 00      mov        dword ptr [eax + 0xb0c], 3
004da63b  e9 f1 03 00 00                     jmp        0x4daa31
004da640  e9 34 04 00 00                     jmp        0x4daa79
004da645  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da648  8b 91 08 0b 00 00                  mov        edx, dword ptr [ecx + 0xb08]
004da64e  89 55 e4                           mov        dword ptr [ebp - 0x1c], edx
004da651  8b 45 e4                           mov        eax, dword ptr [ebp - 0x1c]
004da654  83 e8 01                           sub        eax, 1
004da657  89 45 e4                           mov        dword ptr [ebp - 0x1c], eax
004da65a  83 7d e4 0e                        cmp        dword ptr [ebp - 0x1c], 0xe
004da65e  77 44                              ja         0x4da6a4
004da660  8b 4d e4                           mov        ecx, dword ptr [ebp - 0x1c]
004da663  0f b6 91 28 ab 4d 00               movzx      edx, byte ptr [ecx + 0x4dab28]
004da66a  ff 24 95 18 ab 4d 00               jmp        dword ptr [edx*4 + 0x4dab18]
004da671  e8 6a 29 05 00                     call       0x52cfe0
004da676  90                                 nop        
004da677  eb 2b                              jmp        0x4da6a4
004da679  a1 28 a8 5b 00                     mov        eax, dword ptr [0x5ba828]
004da67e  50                                 push       eax
004da67f  e8 4c 2d fe ff                     call       0x4bd3d0
004da684  83 c4 04                           add        esp, 4
004da687  e8 54 29 05 00                     call       0x52cfe0
004da68c  90                                 nop        
004da68d  eb 15                              jmp        0x4da6a4
004da68f  8b 0d e8 49 5c 00                  mov        ecx, dword ptr [0x5c49e8]
004da695  51                                 push       ecx
004da696  e8 25 71 f4 ff                     call       0x4217c0
004da69b  83 c4 04                           add        esp, 4
004da69e  e8 3d 29 05 00                     call       0x52cfe0
004da6a3  90                                 nop        
004da6a4  e9 d0 03 00 00                     jmp        0x4daa79
004da6a9  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da6ac  8b 82 08 0b 00 00                  mov        eax, dword ptr [edx + 0xb08]
004da6b2  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
004da6b5  83 7d e0 02                        cmp        dword ptr [ebp - 0x20], 2
004da6b9  74 4a                              je         0x4da705
004da6bb  83 7d e0 07                        cmp        dword ptr [ebp - 0x20], 7
004da6bf  74 08                              je         0x4da6c9
004da6c1  83 7d e0 0f                        cmp        dword ptr [ebp - 0x20], 0xf
004da6c5  74 30                              je         0x4da6f7
004da6c7  eb 59                              jmp        0x4da722
004da6c9  8b 0d 28 a8 5b 00                  mov        ecx, dword ptr [0x5ba828]
004da6cf  51                                 push       ecx
004da6d0  e8 fb 2c fe ff                     call       0x4bd3d0
004da6d5  83 c4 04                           add        esp, 4
004da6d8  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da6db  c7 82 0c 0b 00 00 04 00 00 00      mov        dword ptr [edx + 0xb0c], 4
004da6e5  c7 05 28 61 5c 00 03 00 00 00      mov        dword ptr [0x5c6128], 3
004da6ef  e8 ec 28 05 00                     call       0x52cfe0
004da6f4  90                                 nop        
004da6f5  eb 2b                              jmp        0x4da722
004da6f7  a1 e8 49 5c 00                     mov        eax, dword ptr [0x5c49e8]
004da6fc  50                                 push       eax
004da6fd  e8 be 70 f4 ff                     call       0x4217c0
004da702  83 c4 04                           add        esp, 4
004da705  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da708  c7 81 0c 0b 00 00 04 00 00 00      mov        dword ptr [ecx + 0xb0c], 4
004da712  c7 05 28 61 5c 00 03 00 00 00      mov        dword ptr [0x5c6128], 3
004da71c  e8 bf 28 05 00                     call       0x52cfe0
004da721  90                                 nop        
004da722  e9 52 03 00 00                     jmp        0x4daa79
004da727  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da72a  8b 82 08 0b 00 00                  mov        eax, dword ptr [edx + 0xb08]
004da730  89 45 d4                           mov        dword ptr [ebp - 0x2c], eax
004da733  83 7d d4 04                        cmp        dword ptr [ebp - 0x2c], 4
004da737  74 02                              je         0x4da73b
004da739  eb 0f                              jmp        0x4da74a
004da73b  8b 0d 24 61 5c 00                  mov        ecx, dword ptr [0x5c6124]
004da741  51                                 push       ecx
004da742  e8 79 70 f4 ff                     call       0x4217c0
004da747  83 c4 04                           add        esp, 4
004da74a  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da74d  c7 82 18 0b 00 00 01 00 00 00      mov        dword ptr [edx + 0xb18], 1
004da757  6a 00                              push       0
004da759  e8 12 45 fe ff                     call       0x4bec70
004da75e  83 c4 04                           add        esp, 4
004da761  e9 13 03 00 00                     jmp        0x4daa79
004da766  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da769  8b 88 08 0b 00 00                  mov        ecx, dword ptr [eax + 0xb08]
004da76f  89 4d d0                           mov        dword ptr [ebp - 0x30], ecx
004da772  83 7d d0 04                        cmp        dword ptr [ebp - 0x30], 4
004da776  74 02                              je         0x4da77a
004da778  eb 0f                              jmp        0x4da789
004da77a  8b 15 24 61 5c 00                  mov        edx, dword ptr [0x5c6124]
004da780  52                                 push       edx
004da781  e8 3a 70 f4 ff                     call       0x4217c0
004da786  83 c4 04                           add        esp, 4
004da789  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da78c  c7 80 0c 0b 00 00 07 00 00 00      mov        dword ptr [eax + 0xb0c], 7
004da796  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da799  c7 81 18 0b 00 00 01 00 00 00      mov        dword ptr [ecx + 0xb18], 1
004da7a3  6a 01                              push       1
004da7a5  e8 c6 44 fe ff                     call       0x4bec70
004da7aa  83 c4 04                           add        esp, 4
004da7ad  e9 c7 02 00 00                     jmp        0x4daa79
004da7b2  8b 0d 28 a8 5b 00                  mov        ecx, dword ptr [0x5ba828]
004da7b8  e8 73 e0 fa ff                     call       0x488830
004da7bd  89 45 c8                           mov        dword ptr [ebp - 0x38], eax
004da7c0  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da7c3  c7 82 18 0b 00 00 00 00 00 00      mov        dword ptr [edx + 0xb18], 0
004da7cd  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da7d0  8b 88 08 0b 00 00                  mov        ecx, dword ptr [eax + 0xb08]
004da7d6  89 4d cc                           mov        dword ptr [ebp - 0x34], ecx
004da7d9  83 7d cc 07                        cmp        dword ptr [ebp - 0x34], 7
004da7dd  74 02                              je         0x4da7e1
004da7df  eb 0f                              jmp        0x4da7f0
004da7e1  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004da7e7  52                                 push       edx
004da7e8  e8 e3 2b fe ff                     call       0x4bd3d0
004da7ed  83 c4 04                           add        esp, 4
004da7f0  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da7f3  c7 80 0c 0b 00 00 07 00 00 00      mov        dword ptr [eax + 0xb0c], 7
004da7fd  8b 4d c8                           mov        ecx, dword ptr [ebp - 0x38]
004da800  51                                 push       ecx
004da801  e8 6a 44 fe ff                     call       0x4bec70
004da806  83 c4 04                           add        esp, 4
004da809  e9 6b 02 00 00                     jmp        0x4daa79
004da80e  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004da814  52                                 push       edx
004da815  e8 b6 2b fe ff                     call       0x4bd3d0
004da81a  83 c4 04                           add        esp, 4
004da81d  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da820  c7 80 0c 0b 00 00 07 00 00 00      mov        dword ptr [eax + 0xb0c], 7
004da82a  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da82d  c7 81 18 0b 00 00 01 00 00 00      mov        dword ptr [ecx + 0xb18], 1
004da837  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da83a  c7 82 1c 0b 00 00 00 00 00 00      mov        dword ptr [edx + 0xb1c], 0
004da844  e8 f7 e6 fb ff                     call       0x498f40
004da849  8b c8                              mov        ecx, eax
004da84b  e8 80 30 00 00                     call       0x4dd8d0
004da850  6a 00                              push       0
004da852  e8 19 44 fe ff                     call       0x4bec70
004da857  83 c4 04                           add        esp, 4
004da85a  e9 1a 02 00 00                     jmp        0x4daa79
004da85f  e8 dc e6 fb ff                     call       0x498f40
004da864  8b c8                              mov        ecx, eax
004da866  e8 15 a5 f9 ff                     call       0x474d80
004da86b  8b f0                              mov        esi, eax
004da86d  e8 ce e6 fb ff                     call       0x498f40
004da872  8b c8                              mov        ecx, eax
004da874  e8 47 2d fe ff                     call       0x4bd5c0
004da879  3b f0                              cmp        esi, eax
004da87b  74 0d                              je         0x4da88a
004da87d  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da880  c7 80 0c 0b 00 00 07 00 00 00      mov        dword ptr [eax + 0xb0c], 7
004da88a  8b 0d 28 a8 5b 00                  mov        ecx, dword ptr [0x5ba828]
004da890  51                                 push       ecx
004da891  e8 3a 2b fe ff                     call       0x4bd3d0
004da896  83 c4 04                           add        esp, 4
004da899  e8 a2 e6 fb ff                     call       0x498f40
004da89e  8b c8                              mov        ecx, eax
004da8a0  e8 db a4 f9 ff                     call       0x474d80
004da8a5  8b f0                              mov        esi, eax
004da8a7  e8 94 e6 fb ff                     call       0x498f40
004da8ac  8b c8                              mov        ecx, eax
004da8ae  e8 0d 2d fe ff                     call       0x4bd5c0
004da8b3  3b f0                              cmp        esi, eax
004da8b5  75 0d                              jne        0x4da8c4
004da8b7  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da8ba  c7 82 0c 0b 00 00 07 00 00 00      mov        dword ptr [edx + 0xb0c], 7
004da8c4  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da8c7  c7 80 18 0b 00 00 01 00 00 00      mov        dword ptr [eax + 0xb18], 1
004da8d1  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da8d4  c7 81 1c 0b 00 00 00 00 00 00      mov        dword ptr [ecx + 0xb1c], 0
004da8de  e8 5d e6 fb ff                     call       0x498f40
004da8e3  8b c8                              mov        ecx, eax
004da8e5  e8 e6 2f 00 00                     call       0x4dd8d0
004da8ea  6a 00                              push       0
004da8ec  e8 7f 43 fe ff                     call       0x4bec70
004da8f1  83 c4 04                           add        esp, 4
004da8f4  e9 80 01 00 00                     jmp        0x4daa79
004da8f9  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004da8ff  52                                 push       edx
004da900  e8 cb 2a fe ff                     call       0x4bd3d0
004da905  83 c4 04                           add        esp, 4
004da908  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da90b  c7 80 0c 0b 00 00 07 00 00 00      mov        dword ptr [eax + 0xb0c], 7
004da915  e8 26 e6 fb ff                     call       0x498f40
004da91a  8b c8                              mov        ecx, eax
004da91c  e8 af 2f 00 00                     call       0x4dd8d0
004da921  6a 00                              push       0
004da923  e8 48 43 fe ff                     call       0x4bec70
004da928  83 c4 04                           add        esp, 4
004da92b  e9 49 01 00 00                     jmp        0x4daa79
004da930  8b 0d 28 a8 5b 00                  mov        ecx, dword ptr [0x5ba828]
004da936  51                                 push       ecx
004da937  e8 94 2a fe ff                     call       0x4bd3d0
004da93c  83 c4 04                           add        esp, 4
004da93f  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da942  c7 82 18 0b 00 00 01 00 00 00      mov        dword ptr [edx + 0xb18], 1
004da94c  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da94f  c7 80 1c 0b 00 00 00 00 00 00      mov        dword ptr [eax + 0xb1c], 0
004da959  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da95c  c7 81 0c 0b 00 00 07 00 00 00      mov        dword ptr [ecx + 0xb0c], 7
004da966  e8 d5 e5 fb ff                     call       0x498f40
004da96b  8b c8                              mov        ecx, eax
004da96d  e8 5e 2f 00 00                     call       0x4dd8d0
004da972  6a 01                              push       1
004da974  e8 f7 42 fe ff                     call       0x4bec70
004da979  83 c4 04                           add        esp, 4
004da97c  e9 f8 00 00 00                     jmp        0x4daa79
004da981  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004da987  52                                 push       edx
004da988  e8 43 2a fe ff                     call       0x4bd3d0
004da98d  83 c4 04                           add        esp, 4
004da990  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004da993  c7 80 18 0b 00 00 01 00 00 00      mov        dword ptr [eax + 0xb18], 1
004da99d  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da9a0  c7 81 1c 0b 00 00 01 00 00 00      mov        dword ptr [ecx + 0xb1c], 1
004da9aa  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da9ad  c7 82 0c 0b 00 00 07 00 00 00      mov        dword ptr [edx + 0xb0c], 7
004da9b7  e8 84 e5 fb ff                     call       0x498f40
004da9bc  8b c8                              mov        ecx, eax
004da9be  e8 0d 2f 00 00                     call       0x4dd8d0
004da9c3  6a 00                              push       0
004da9c5  e8 a6 42 fe ff                     call       0x4bec70
004da9ca  83 c4 04                           add        esp, 4
004da9cd  e9 a7 00 00 00                     jmp        0x4daa79
004da9d2  a1 28 a8 5b 00                     mov        eax, dword ptr [0x5ba828]
004da9d7  50                                 push       eax
004da9d8  e8 f3 29 fe ff                     call       0x4bd3d0
004da9dd  83 c4 04                           add        esp, 4
004da9e0  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004da9e3  c7 81 18 0b 00 00 01 00 00 00      mov        dword ptr [ecx + 0xb18], 1
004da9ed  8b 55 e8                           mov        edx, dword ptr [ebp - 0x18]
004da9f0  c7 82 0c 0b 00 00 07 00 00 00      mov        dword ptr [edx + 0xb0c], 7
004da9fa  6a 00                              push       0
004da9fc  e8 6f 42 fe ff                     call       0x4bec70
004daa01  83 c4 04                           add        esp, 4
004daa04  eb 73                              jmp        0x4daa79
004daa06  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004daa09  8b 88 08 0b 00 00                  mov        ecx, dword ptr [eax + 0xb08]
004daa0f  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
004daa12  83 7d c4 07                        cmp        dword ptr [ebp - 0x3c], 7
004daa16  74 02                              je         0x4daa1a
004daa18  eb 0f                              jmp        0x4daa29
004daa1a  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004daa20  52                                 push       edx
004daa21  e8 aa 29 fe ff                     call       0x4bd3d0
004daa26  83 c4 04                           add        esp, 4
004daa29  e8 e2 65 fc ff                     call       0x4a1010
004daa2e  90                                 nop        
004daa2f  eb 48                              jmp        0x4daa79
004daa31  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004daa34  e8 f7 2c 00 00                     call       0x4dd730
004daa39  90                                 nop        
004daa3a  c7 45 c0 04 00 00 00               mov        dword ptr [ebp - 0x40], 4
004daa41  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004daa48  8d 4d ec                           lea        ecx, [ebp - 0x14]
004daa4b  e8 b0 0e f3 ff                     call       0x40b900
004daa50  8b 45 c0                           mov        eax, dword ptr [ebp - 0x40]
004daa53  eb 4b                              jmp        0x4daaa0
004daa55  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004daa58  e8 d3 2c 00 00                     call       0x4dd730
004daa5d  90                                 nop        
004daa5e  c7 45 bc 05 00 00 00               mov        dword ptr [ebp - 0x44], 5
004daa65  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004daa6c  8d 4d ec                           lea        ecx, [ebp - 0x14]
004daa6f  e8 8c 0e f3 ff                     call       0x40b900
004daa74  8b 45 bc                           mov        eax, dword ptr [ebp - 0x44]
004daa77  eb 27                              jmp        0x4daaa0
004daa79  8b 45 e8                           mov        eax, dword ptr [ebp - 0x18]
004daa7c  8b 4d e8                           mov        ecx, dword ptr [ebp - 0x18]
004daa7f  8b 91 0c 0b 00 00                  mov        edx, dword ptr [ecx + 0xb0c]
004daa85  89 90 08 0b 00 00                  mov        dword ptr [eax + 0xb08], edx
004daa8b  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004daa92  8d 4d ec                           lea        ecx, [ebp - 0x14]
004daa95  e8 66 0e f3 ff                     call       0x40b900
004daa9a  90                                 nop        
004daa9b  b8 01 00 00 00                     mov        eax, 1
004daaa0  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004daaa3  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004daaaa  59                                 pop        ecx
004daaab  5e                                 pop        esi
004daaac  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004daaaf  33 cd                              xor        ecx, ebp
004daab1  e8 38 7f 06 00                     call       0x5429ee
004daab6  8b e5                              mov        esp, ebp
004daab8  5d                                 pop        ebp
004daab9  c3                                 ret        
