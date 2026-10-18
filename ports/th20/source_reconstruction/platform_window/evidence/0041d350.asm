0041d350  55                                 push       ebp
0041d351  8b ec                              mov        ebp, esp
0041d353  83 ec 6c                           sub        esp, 0x6c
0041d356  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041d35b  33 c5                              xor        eax, ebp
0041d35d  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041d360  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0041d363  89 45 b4                           mov        dword ptr [ebp - 0x4c], eax
0041d366  83 7d b4 1c                        cmp        dword ptr [ebp - 0x4c], 0x1c
0041d36a  77 2e                              ja         0x41d39a
0041d36c  83 7d b4 1c                        cmp        dword ptr [ebp - 0x4c], 0x1c
0041d370  0f 84 c5 00 00 00                  je         0x41d43b
0041d376  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
0041d379  83 e9 05                           sub        ecx, 5
0041d37c  89 4d b4                           mov        dword ptr [ebp - 0x4c], ecx
0041d37f  83 7d b4 0f                        cmp        dword ptr [ebp - 0x4c], 0xf
0041d383  0f 87 31 04 00 00                  ja         0x41d7ba
0041d389  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
0041d38c  0f b6 82 f4 d7 41 00               movzx      eax, byte ptr [edx + 0x41d7f4]
0041d393  ff 24 85 e0 d7 41 00               jmp        dword ptr [eax*4 + 0x41d7e0]
0041d39a  8b 4d b4                           mov        ecx, dword ptr [ebp - 0x4c]
0041d39d  83 e9 20                           sub        ecx, 0x20
0041d3a0  89 4d b4                           mov        dword ptr [ebp - 0x4c], ecx
0041d3a3  81 7d b4 f2 00 00 00               cmp        dword ptr [ebp - 0x4c], 0xf2
0041d3aa  0f 87 0a 04 00 00                  ja         0x41d7ba
0041d3b0  8b 55 b4                           mov        edx, dword ptr [ebp - 0x4c]
0041d3b3  0f b6 82 18 d8 41 00               movzx      eax, byte ptr [edx + 0x41d818]
0041d3ba  ff 24 85 04 d8 41 00               jmp        dword ptr [eax*4 + 0x41d804]
0041d3c1  b8 01 00 00 00                     mov        eax, 1
0041d3c6  e9 05 04 00 00                     jmp        0x41d7d0
0041d3cb  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041d3d1  c1 e9 07                           shr        ecx, 7
0041d3d4  83 e1 01                           and        ecx, 1
0041d3d7  75 58                              jne        0x41d431
0041d3d9  8d 55 bc                           lea        edx, [ebp - 0x44]
0041d3dc  52                                 push       edx
0041d3dd  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041d3e0  50                                 push       eax
0041d3e1  ff 15 54 c2 56 00                  call       dword ptr [0x56c254] ; USER32.dll!BeginPaint
0041d3e7  89 45 9c                           mov        dword ptr [ebp - 0x64], eax
0041d3ea  6a 00                              push       0
0041d3ec  ff 15 18 c0 56 00                  call       dword ptr [0x56c018] ; GDI32.dll!CreateSolidBrush
0041d3f2  89 45 ac                           mov        dword ptr [ebp - 0x54], eax
0041d3f5  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
0041d3f8  51                                 push       ecx
0041d3f9  8d 55 c4                           lea        edx, [ebp - 0x3c]
0041d3fc  52                                 push       edx
0041d3fd  8b 45 9c                           mov        eax, dword ptr [ebp - 0x64]
0041d400  50                                 push       eax
0041d401  ff 15 70 c2 56 00                  call       dword ptr [0x56c270] ; USER32.dll!FillRect
0041d407  8b 4d ac                           mov        ecx, dword ptr [ebp - 0x54]
0041d40a  51                                 push       ecx
0041d40b  ff 15 38 c0 56 00                  call       dword ptr [0x56c038] ; GDI32.dll!DeleteObject
0041d411  8d 55 bc                           lea        edx, [ebp - 0x44]
0041d414  52                                 push       edx
0041d415  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041d418  50                                 push       eax
0041d419  ff 15 a4 c2 56 00                  call       dword ptr [0x56c2a4] ; USER32.dll!EndPaint
0041d41f  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041d425  81 c9 80 00 00 00                  or         ecx, 0x80
0041d42b  89 0d e8 87 5b 00                  mov        dword ptr [0x5b87e8], ecx
0041d431  e9 84 03 00 00                     jmp        0x41d7ba
0041d43b  83 7d 10 00                        cmp        dword ptr [ebp + 0x10], 0
0041d43f  74 06                              je         0x41d447
0041d441  c6 45 bb 01                        mov        byte ptr [ebp - 0x45], 1
0041d445  eb 04                              jmp        0x41d44b
0041d447  c6 45 bb 00                        mov        byte ptr [ebp - 0x45], 0
0041d44b  8a 55 bb                           mov        dl, byte ptr [ebp - 0x45]
0041d44e  88 15 c0 67 5b 00                  mov        byte ptr [0x5b67c0], dl
0041d454  0f b6 05 c0 67 5b 00               movzx      eax, byte ptr [0x5b67c0]
0041d45b  85 c0                              test       eax, eax
0041d45d  74 20                              je         0x41d47f
0041d45f  c6 05 c1 67 5b 00 00               mov        byte ptr [0x5b67c1], 0
0041d466  6a 03                              push       3
0041d468  6a 00                              push       0
0041d46a  6a 00                              push       0
0041d46c  6a 00                              push       0
0041d46e  6a 00                              push       0
0041d470  6a ff                              push       -1
0041d472  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041d475  51                                 push       ecx
0041d476  ff 15 2c c2 56 00                  call       dword ptr [0x56c22c] ; USER32.dll!SetWindowPos
0041d47c  90                                 nop        
0041d47d  eb 1e                              jmp        0x41d49d
0041d47f  c6 05 c1 67 5b 00 01               mov        byte ptr [0x5b67c1], 1
0041d486  6a 03                              push       3
0041d488  6a 00                              push       0
0041d48a  6a 00                              push       0
0041d48c  6a 00                              push       0
0041d48e  6a 00                              push       0
0041d490  6a fe                              push       -2
0041d492  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041d495  52                                 push       edx
0041d496  ff 15 2c c2 56 00                  call       dword ptr [0x56c22c] ; USER32.dll!SetWindowPos
0041d49c  90                                 nop        
0041d49d  e9 18 03 00 00                     jmp        0x41d7ba
0041d4a2  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041d4a7  e8 54 83 ff ff                     call       0x415800
0041d4ac  85 c0                              test       eax, eax
0041d4ae  75 0c                              jne        0x41d4bc
0041d4b0  68 b8 0b 00 00                     push       0xbb8
0041d4b5  ff 15 6c c1 56 00                  call       dword ptr [0x56c16c] ; KERNEL32.dll!Sleep
0041d4bb  90                                 nop        
0041d4bc  c7 05 c4 67 5b 00 2c 01 00 00      mov        dword ptr [0x5b67c4], 0x12c
0041d4c6  e9 ef 02 00 00                     jmp        0x41d7ba
0041d4d0  8b 45 10                           mov        eax, dword ptr [ebp + 0x10]
0041d4d3  25 f0 ff 00 00                     and        eax, 0xfff0
0041d4d8  89 45 a8                           mov        dword ptr [ebp - 0x58], eax
0041d4db  81 7d a8 90 f0 00 00               cmp        dword ptr [ebp - 0x58], 0xf090
0041d4e2  74 15                              je         0x41d4f9
0041d4e4  81 7d a8 00 f1 00 00               cmp        dword ptr [ebp - 0x58], 0xf100
0041d4eb  74 02                              je         0x41d4ef
0041d4ed  eb 14                              jmp        0x41d503
0041d4ef  b8 01 00 00 00                     mov        eax, 1
0041d4f4  e9 d7 02 00 00                     jmp        0x41d7d0
0041d4f9  b8 01 00 00 00                     mov        eax, 1
0041d4fe  e9 cd 02 00 00                     jmp        0x41d7d0
0041d503  e9 b2 02 00 00                     jmp        0x41d7ba
0041d508  8b 0d e8 87 5b 00                  mov        ecx, dword ptr [0x5b87e8]
0041d50e  83 e1 01                           and        ecx, 1
0041d511  74 7d                              je         0x41d590
0041d513  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0041d516  89 55 98                           mov        dword ptr [ebp - 0x68], edx
0041d519  83 7d 98 02                        cmp        dword ptr [ebp - 0x68], 2
0041d51d  74 04                              je         0x41d523
0041d51f  eb 6f                              jmp        0x41d590
0041d523  6a 01                              push       1
0041d525  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d52a  e8 d1 08 00 00                     call       0x41de00
0041d52f  90                                 nop        
0041d530  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d535  e8 36 f7 ff ff                     call       0x41cc70
0041d53a  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
0041d53d  8b 45 b0                           mov        eax, dword ptr [ebp - 0x50]
0041d540  83 e8 03                           sub        eax, 3
0041d543  89 45 b0                           mov        dword ptr [ebp - 0x50], eax
0041d546  83 7d b0 03                        cmp        dword ptr [ebp - 0x50], 3
0041d54a  77 28                              ja         0x41d574
0041d54c  8b 4d b0                           mov        ecx, dword ptr [ebp - 0x50]
0041d54f  ff 24 8d 0c d9 41 00               jmp        dword ptr [ecx*4 + 0x41d90c]
0041d556  6a 00                              push       0
0041d558  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d55d  e8 ce 0a 00 00                     call       0x41e030
0041d562  90                                 nop        
0041d563  eb 2b                              jmp        0x41d590
0041d565  6a 01                              push       1
0041d567  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d56c  e8 bf 0a 00 00                     call       0x41e030
0041d571  90                                 nop        
0041d572  eb 1c                              jmp        0x41d590
0041d574  6a 02                              push       2
0041d576  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d57b  e8 b0 0a 00 00                     call       0x41e030
0041d580  90                                 nop        
0041d581  eb 0d                              jmp        0x41d590
0041d583  6a 02                              push       2
0041d585  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d58a  e8 a1 0a 00 00                     call       0x41e030
0041d58f  90                                 nop        
0041d590  e9 25 02 00 00                     jmp        0x41d7ba
0041d595  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0041d598  89 55 94                           mov        dword ptr [ebp - 0x6c], edx
0041d59b  83 7d 94 0d                        cmp        dword ptr [ebp - 0x6c], 0xd
0041d59f  74 05                              je         0x41d5a6
0041d5a1  e9 63 01 00 00                     jmp        0x41d709
0041d5a6  6a 01                              push       1
0041d5a8  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d5ad  e8 4e 08 00 00                     call       0x41de00
0041d5b2  90                                 nop        
0041d5b3  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d5b8  e8 b3 f6 ff ff                     call       0x41cc70
0041d5bd  89 45 a4                           mov        dword ptr [ebp - 0x5c], eax
0041d5c0  83 7d a4 09                        cmp        dword ptr [ebp - 0x5c], 9
0041d5c4  0f 87 3f 01 00 00                  ja         0x41d709
0041d5ca  8b 45 a4                           mov        eax, dword ptr [ebp - 0x5c]
0041d5cd  ff 24 85 1c d9 41 00               jmp        dword ptr [eax*4 + 0x41d91c]
0041d5d4  6a 00                              push       0
0041d5d6  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d5db  e8 50 0a 00 00                     call       0x41e030
0041d5e0  90                                 nop        
0041d5e1  e9 23 01 00 00                     jmp        0x41d709
0041d5e6  6a 01                              push       1
0041d5e8  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d5ed  e8 3e 0a 00 00                     call       0x41e030
0041d5f2  90                                 nop        
0041d5f3  e9 11 01 00 00                     jmp        0x41d709
0041d5f8  6a 02                              push       2
0041d5fa  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d5ff  e8 2c 0a 00 00                     call       0x41e030
0041d604  90                                 nop        
0041d605  e9 ff 00 00 00                     jmp        0x41d709
0041d60a  6a 02                              push       2
0041d60c  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d611  e8 1a 0a 00 00                     call       0x41e030
0041d616  90                                 nop        
0041d617  e9 ed 00 00 00                     jmp        0x41d709
0041d61c  6a 02                              push       2
0041d61e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d623  e8 08 0a 00 00                     call       0x41e030
0041d628  90                                 nop        
0041d629  e9 db 00 00 00                     jmp        0x41d709
0041d62e  6a 03                              push       3
0041d630  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d635  e8 f6 09 00 00                     call       0x41e030
0041d63a  90                                 nop        
0041d63b  e9 c9 00 00 00                     jmp        0x41d709
0041d640  6a 04                              push       4
0041d642  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d647  e8 e4 09 00 00                     call       0x41e030
0041d64c  90                                 nop        
0041d64d  e9 b7 00 00 00                     jmp        0x41d709
0041d652  0f b6 0d 8a 4f 5c 00               movzx      ecx, byte ptr [0x5c4f8a]
0041d659  83 f9 04                           cmp        ecx, 4
0041d65c  75 0f                              jne        0x41d66d
0041d65e  6a 07                              push       7
0041d660  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d665  e8 c6 09 00 00                     call       0x41e030
0041d66a  90                                 nop        
0041d66b  eb 28                              jmp        0x41d695
0041d66d  0f b6 15 8a 4f 5c 00               movzx      edx, byte ptr [0x5c4f8a]
0041d674  83 fa 03                           cmp        edx, 3
0041d677  75 0f                              jne        0x41d688
0041d679  6a 06                              push       6
0041d67b  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d680  e8 ab 09 00 00                     call       0x41e030
0041d685  90                                 nop        
0041d686  eb 0d                              jmp        0x41d695
0041d688  6a 05                              push       5
0041d68a  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d68f  e8 9c 09 00 00                     call       0x41e030
0041d694  90                                 nop        
0041d695  eb 72                              jmp        0x41d709
0041d697  0f b6 05 8a 4f 5c 00               movzx      eax, byte ptr [0x5c4f8a]
0041d69e  89 45 a0                           mov        dword ptr [ebp - 0x60], eax
0041d6a1  83 7d a0 05                        cmp        dword ptr [ebp - 0x60], 5
0041d6a5  77 62                              ja         0x41d709
0041d6a7  8b 4d a0                           mov        ecx, dword ptr [ebp - 0x60]
0041d6aa  ff 24 8d 44 d9 41 00               jmp        dword ptr [ecx*4 + 0x41d944]
0041d6b1  6a 03                              push       3
0041d6b3  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d6b8  e8 73 09 00 00                     call       0x41e030
0041d6bd  90                                 nop        
0041d6be  eb 49                              jmp        0x41d709
0041d6c0  6a 04                              push       4
0041d6c2  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d6c7  e8 64 09 00 00                     call       0x41e030
0041d6cc  90                                 nop        
0041d6cd  eb 3a                              jmp        0x41d709
0041d6cf  6a 05                              push       5
0041d6d1  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d6d6  e8 55 09 00 00                     call       0x41e030
0041d6db  90                                 nop        
0041d6dc  eb 2b                              jmp        0x41d709
0041d6de  6a 06                              push       6
0041d6e0  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d6e5  e8 46 09 00 00                     call       0x41e030
0041d6ea  90                                 nop        
0041d6eb  eb 1c                              jmp        0x41d709
0041d6ed  6a 07                              push       7
0041d6ef  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d6f4  e8 37 09 00 00                     call       0x41e030
0041d6f9  90                                 nop        
0041d6fa  eb 0d                              jmp        0x41d709
0041d6fc  6a 05                              push       5
0041d6fe  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d703  e8 28 09 00 00                     call       0x41e030
0041d708  90                                 nop        
0041d709  e9 ac 00 00 00                     jmp        0x41d7ba
0041d70e  b9 40 4d 5c 00                     mov        ecx, 0x5c4d40
0041d713  e8 e8 80 ff ff                     call       0x415800
0041d718  85 c0                              test       eax, eax
0041d71a  74 1e                              je         0x41d73a
0041d71c  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d721  e8 4a f5 ff ff                     call       0x41cc70
0041d726  83 f8 08                           cmp        eax, 8
0041d729  74 0f                              je         0x41d73a
0041d72b  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041d730  e8 3b f5 ff ff                     call       0x41cc70
0041d735  83 f8 09                           cmp        eax, 9
0041d738  75 45                              jne        0x41d77f
0041d73a  0f b6 15 c1 67 5b 00               movzx      edx, byte ptr [0x5b67c1]
0041d741  85 d2                              test       edx, edx
0041d743  74 23                              je         0x41d768
0041d745  68 00 7f 00 00                     push       0x7f00
0041d74a  6a 00                              push       0
0041d74c  ff 15 78 c2 56 00                  call       dword ptr [0x56c278] ; USER32.dll!LoadCursorW
0041d752  50                                 push       eax
0041d753  ff 15 6c c2 56 00                  call       dword ptr [0x56c26c] ; USER32.dll!SetCursor
0041d759  90                                 nop        
0041d75a  6a 01                              push       1
0041d75c  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041d762  85 c0                              test       eax, eax
0041d764  7c f4                              jl         0x41d75a
0041d766  eb 15                              jmp        0x41d77d
0041d768  6a 00                              push       0
0041d76a  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041d770  85 c0                              test       eax, eax
0041d772  7d f4                              jge        0x41d768
0041d774  6a 00                              push       0
0041d776  ff 15 6c c2 56 00                  call       dword ptr [0x56c26c] ; USER32.dll!SetCursor
0041d77c  90                                 nop        
0041d77d  eb 1d                              jmp        0x41d79c
0041d77f  68 00 7f 00 00                     push       0x7f00
0041d784  6a 00                              push       0
0041d786  ff 15 78 c2 56 00                  call       dword ptr [0x56c278] ; USER32.dll!LoadCursorW
0041d78c  50                                 push       eax
0041d78d  ff 15 6c c2 56 00                  call       dword ptr [0x56c26c] ; USER32.dll!SetCursor
0041d793  6a 01                              push       1
0041d795  ff 15 68 c2 56 00                  call       dword ptr [0x56c268] ; USER32.dll!ShowCursor
0041d79b  90                                 nop        
0041d79c  b8 01 00 00 00                     mov        eax, 1
0041d7a1  eb 2d                              jmp        0x41d7d0
0041d7a3  a1 88 58 5c 00                     mov        eax, dword ptr [0x5c5888]
0041d7a8  83 e0 9f                           and        eax, 0xffffff9f
0041d7ab  83 c8 20                           or         eax, 0x20
0041d7ae  a3 88 58 5c 00                     mov        dword ptr [0x5c5888], eax
0041d7b3  b8 01 00 00 00                     mov        eax, 1
0041d7b8  eb 16                              jmp        0x41d7d0
0041d7ba  8b 4d 14                           mov        ecx, dword ptr [ebp + 0x14]
0041d7bd  51                                 push       ecx
0041d7be  8b 55 10                           mov        edx, dword ptr [ebp + 0x10]
0041d7c1  52                                 push       edx
0041d7c2  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
0041d7c5  50                                 push       eax
0041d7c6  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
0041d7c9  51                                 push       ecx
0041d7ca  ff 15 9c c2 56 00                  call       dword ptr [0x56c29c] ; USER32.dll!DefWindowProcW
0041d7d0  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041d7d3  33 cd                              xor        ecx, ebp
0041d7d5  e8 14 52 12 00                     call       0x5429ee
0041d7da  8b e5                              mov        esp, ebp
0041d7dc  5d                                 pop        ebp
0041d7dd  c2 10 00                           ret        0x10
