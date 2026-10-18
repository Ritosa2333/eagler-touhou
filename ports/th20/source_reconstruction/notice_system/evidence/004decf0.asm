004decf0 55                       push       ebp
004decf1 8bec                     mov        ebp, esp
004decf3 81ec40010000             sub        esp, 0x140
004decf9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004decfe 33c5                     xor        eax, ebp
004ded00 8945fc                   mov        dword ptr [ebp - 4], eax
004ded03 894dec                   mov        dword ptr [ebp - 0x14], ecx
004ded06 51                       push       ecx
004ded07 0f57c0                   xorps      xmm0, xmm0
004ded0a f30f110424               movss      dword ptr [esp], xmm0
004ded0f 51                       push       ecx
004ded10 0f57c0                   xorps      xmm0, xmm0
004ded13 f30f110424               movss      dword ptr [esp], xmm0
004ded18 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004ded1b 51                       push       ecx
004ded1c f30f108094000000         movss      xmm0, dword ptr [eax + 0x94]
004ded24 f30f110424               movss      dword ptr [esp], xmm0
004ded29 8d4df0                   lea        ecx, [ebp - 0x10]
004ded2c e89f40f4ff               call       0x422dd0
004ded31 90                       nop        
004ded32 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004ded35 8b5110                   mov        edx, dword ptr [ecx + 0x10]
004ded38 8955e0                   mov        dword ptr [ebp - 0x20], edx
004ded3b 837de003                 cmp        dword ptr [ebp - 0x20], 3
004ded3f 0f87ca080000             ja         0x4df60f
004ded45 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004ded48 ff24853cf64d00           jmp        dword ptr [eax*4 + 0x4df63c]
004ded4f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004ded52 c7411001000000           mov        dword ptr [ecx + 0x10], 1
004ded59 e9b1080000               jmp        0x4df60f
004ded5e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004ded61 8b82a8000000             mov        eax, dword ptr [edx + 0xa8]
004ded67 8945dc                   mov        dword ptr [ebp - 0x24], eax
004ded6a 837ddc07                 cmp        dword ptr [ebp - 0x24], 7
004ded6e 0f873a080000             ja         0x4df5ae
004ded74 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004ded77 ff248d4cf64d00           jmp        dword ptr [ecx*4 + 0x4df64c]
004ded7e 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004ded81 83c224                   add        edx, 0x24
004ded84 89951cffffff             mov        dword ptr [ebp - 0xe4], edx
004ded8a 6a63                     push       0x63
004ded8c 8b8d1cffffff             mov        ecx, dword ptr [ebp - 0xe4]
004ded92 e849d7fbff               call       0x49c4e0
004ded97 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004ded9a 8b8898000000             mov        ecx, dword ptr [eax + 0x98]
004deda0 51                       push       ecx
004deda1 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004deda4 83c124                   add        ecx, 0x24
004deda7 e8a4fffdff               call       0x4bed50
004dedac 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004dedaf 83c224                   add        edx, 0x24
004dedb2 8955cc                   mov        dword ptr [ebp - 0x34], edx
004dedb5 6a01                     push       1
004dedb7 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004dedba e8210efeff               call       0x4bfbe0
004dedbf 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004dedc2 8b889c000000             mov        ecx, dword ptr [eax + 0x9c]
004dedc8 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004dedcb 6a01                     push       1
004dedcd 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004dedd0 e80b0dfeff               call       0x4bfae0
004dedd5 8bc8                     mov        ecx, eax
004dedd7 e8a4aff6ff               call       0x449d80
004deddc 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004deddf c782a800000001000000     mov        dword ptr [edx + 0xa8], 1
004dede9 6a00                     push       0
004dedeb 6a53                     push       0x53
004deded b930a85b00               mov        ecx, 0x5ba830
004dedf2 e8797ff4ff               call       0x426d70
004dedf7 90                       nop        
004dedf8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004dedfb c780a800000004000000     mov        dword ptr [eax + 0xa8], 4
004dee05 6a00                     push       0
004dee07 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004dee0a e8210d0000               call       0x4dfb30
004dee0f 6a04                     push       4
004dee11 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004dee14 e8170d0000               call       0x4dfb30
004dee19 6a05                     push       5
004dee1b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004dee1e e80d0d0000               call       0x4dfb30
004dee23 6a06                     push       6
004dee25 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004dee28 e8030d0000               call       0x4dfb30
004dee2d 90                       nop        
004dee2e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004dee31 83c124                   add        ecx, 0x24
004dee34 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
004dee37 6a01                     push       1
004dee39 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004dee3c e83ffeffff               call       0x4dec80
004dee41 85c0                     test       eax, eax
004dee43 0f84d2000000             je         0x4def1b
004dee49 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004dee4c 83c224                   add        edx, 0x24
004dee4f 8955c0                   mov        dword ptr [ebp - 0x40], edx
004dee52 6a08                     push       8
004dee54 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
004dee57 e8f4fdffff               call       0x4dec50
004dee5c 85c0                     test       eax, eax
004dee5e 0f84b7000000             je         0x4def1b
004dee64 e8f7ca0300               call       0x51b960
004dee69 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004dee6c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004dee6f 83c024                   add        eax, 0x24
004dee72 8945bc                   mov        dword ptr [ebp - 0x44], eax
004dee75 6a01                     push       1
004dee77 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004dee7a e8b1fdffff               call       0x4dec30
004dee7f 8945b8                   mov        dword ptr [ebp - 0x48], eax
004dee82 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004dee85 51                       push       ecx
004dee86 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004dee89 e8420c0000               call       0x4dfad0
004dee8e 8945ac                   mov        dword ptr [ebp - 0x54], eax
004dee91 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004dee94 83c124                   add        ecx, 0x24
004dee97 e864d4f2ff               call       0x40c300
004dee9c 6bd038                   imul       edx, eax, 0x38
004dee9f b81c000000               mov        eax, 0x1c
004deea4 6bc800                   imul       ecx, eax, 0
004deea7 8d8c0a405b5c00           lea        ecx, [edx + ecx + 0x5c5b40]
004deeae e83dccf6ff               call       0x44baf0
004deeb3 8945a8                   mov        dword ptr [ebp - 0x58], eax
004deeb6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004deeb9 83c270                   add        edx, 0x70
004deebc 8955b0                   mov        dword ptr [ebp - 0x50], edx
004deebf 6a05                     push       5
004deec1 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004deec4 e8b756f3ff               call       0x414580
004deec9 8bc8                     mov        ecx, eax
004deecb e800e0f6ff               call       0x44ced0
004deed0 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004deed3 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
004deed6 50                       push       eax
004deed7 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004deeda 51                       push       ecx
004deedb 83ec28                   sub        esp, 0x28
004deede 8bcc                     mov        ecx, esp
004deee0 89a5f8feffff             mov        dword ptr [ebp - 0x108], esp
004deee6 8d55eb                   lea        edx, [ebp - 0x15]
004deee9 52                       push       edx
004deeea e811f7ffff               call       0x4de600
004deeef 6a00                     push       0
004deef1 6a00                     push       0
004deef3 6a07                     push       7
004deef5 6a00                     push       0
004deef7 6880d0ff00               push       0xffd080
004deefc 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
004deeff 50                       push       eax
004def00 8d8de0feffff             lea        ecx, [ebp - 0x120]
004def06 51                       push       ecx
004def07 8b1528005c00             mov        edx, dword ptr [0x5c0028]
004def0d 52                       push       edx
004def0e e8bdbef6ff               call       0x44add0
004def13 83c450                   add        esp, 0x50
004def16 e9a3020000               jmp        0x4df1be
004def1b 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004def1e 83c024                   add        eax, 0x24
004def21 8945a0                   mov        dword ptr [ebp - 0x60], eax
004def24 6a09                     push       9
004def26 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004def29 e852fdffff               call       0x4dec80
004def2e 85c0                     test       eax, eax
004def30 0f84d2000000             je         0x4df008
004def36 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004def39 83c124                   add        ecx, 0x24
004def3c 894d9c                   mov        dword ptr [ebp - 0x64], ecx
004def3f 6a10                     push       0x10
004def41 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004def44 e807fdffff               call       0x4dec50
004def49 85c0                     test       eax, eax
004def4b 0f84b7000000             je         0x4df008
004def51 e80aca0300               call       0x51b960
004def56 894590                   mov        dword ptr [ebp - 0x70], eax
004def59 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004def5c 83c224                   add        edx, 0x24
004def5f 895598                   mov        dword ptr [ebp - 0x68], edx
004def62 6a09                     push       9
004def64 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004def67 e8c4fcffff               call       0x4dec30
004def6c 894594                   mov        dword ptr [ebp - 0x6c], eax
004def6f 8b4594                   mov        eax, dword ptr [ebp - 0x6c]
004def72 50                       push       eax
004def73 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004def76 e8550b0000               call       0x4dfad0
004def7b 894588                   mov        dword ptr [ebp - 0x78], eax
004def7e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004def81 83c124                   add        ecx, 0x24
004def84 e877d3f2ff               call       0x40c300
004def89 6bc838                   imul       ecx, eax, 0x38
004def8c ba1c000000               mov        edx, 0x1c
004def91 6bc200                   imul       eax, edx, 0
004def94 8d8c01405b5c00           lea        ecx, [ecx + eax + 0x5c5b40]
004def9b e850cbf6ff               call       0x44baf0
004defa0 894584                   mov        dword ptr [ebp - 0x7c], eax
004defa3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004defa6 83c170                   add        ecx, 0x70
004defa9 894d8c                   mov        dword ptr [ebp - 0x74], ecx
004defac 6a05                     push       5
004defae 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004defb1 e8ca55f3ff               call       0x414580
004defb6 8bc8                     mov        ecx, eax
004defb8 e813dff6ff               call       0x44ced0
004defbd 894580                   mov        dword ptr [ebp - 0x80], eax
004defc0 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004defc3 52                       push       edx
004defc4 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
004defc7 50                       push       eax
004defc8 83ec28                   sub        esp, 0x28
004defcb 8bcc                     mov        ecx, esp
004defcd 89a5fcfeffff             mov        dword ptr [ebp - 0x104], esp
004defd3 8d55ea                   lea        edx, [ebp - 0x16]
004defd6 52                       push       edx
004defd7 e894f6ffff               call       0x4de670
004defdc 6a00                     push       0
004defde 6a00                     push       0
004defe0 6a07                     push       7
004defe2 6a00                     push       0
004defe4 6880d0ff00               push       0xffd080
004defe9 8b4580                   mov        eax, dword ptr [ebp - 0x80]
004defec 50                       push       eax
004defed 8d8dd8feffff             lea        ecx, [ebp - 0x128]
004deff3 51                       push       ecx
004deff4 8b1528005c00             mov        edx, dword ptr [0x5c0028]
004deffa 52                       push       edx
004deffb e8d0bdf6ff               call       0x44add0
004df000 83c450                   add        esp, 0x50
004df003 e9b6010000               jmp        0x4df1be
004df008 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df00b 83c024                   add        eax, 0x24
004df00e 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004df014 6a11                     push       0x11
004df016 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004df01c e85ffcffff               call       0x4dec80
004df021 85c0                     test       eax, eax
004df023 0f8402010000             je         0x4df12b
004df029 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df02c 83c124                   add        ecx, 0x24
004df02f 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
004df035 6a18                     push       0x18
004df037 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004df03d e80efcffff               call       0x4dec50
004df042 85c0                     test       eax, eax
004df044 0f84e1000000             je         0x4df12b
004df04a e811c90300               call       0x51b960
004df04f 89856cffffff             mov        dword ptr [ebp - 0x94], eax
004df055 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df058 83c224                   add        edx, 0x24
004df05b 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
004df061 6a11                     push       0x11
004df063 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004df069 e8c2fbffff               call       0x4dec30
004df06e 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004df074 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
004df07a 50                       push       eax
004df07b 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004df081 e84a0a0000               call       0x4dfad0
004df086 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
004df08c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df08f 83c124                   add        ecx, 0x24
004df092 e869d2f2ff               call       0x40c300
004df097 6bc838                   imul       ecx, eax, 0x38
004df09a ba1c000000               mov        edx, 0x1c
004df09f 6bc200                   imul       eax, edx, 0
004df0a2 8d8c01405b5c00           lea        ecx, [ecx + eax + 0x5c5b40]
004df0a9 e842caf6ff               call       0x44baf0
004df0ae 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
004df0b4 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df0b7 83c170                   add        ecx, 0x70
004df0ba 898d68ffffff             mov        dword ptr [ebp - 0x98], ecx
004df0c0 6a05                     push       5
004df0c2 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004df0c8 e8b354f3ff               call       0x414580
004df0cd 8bc8                     mov        ecx, eax
004df0cf e8fcddf6ff               call       0x44ced0
004df0d4 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
004df0da 8b9564ffffff             mov        edx, dword ptr [ebp - 0x9c]
004df0e0 52                       push       edx
004df0e1 8b8560ffffff             mov        eax, dword ptr [ebp - 0xa0]
004df0e7 50                       push       eax
004df0e8 83ec28                   sub        esp, 0x28
004df0eb 8bcc                     mov        ecx, esp
004df0ed 89a5e8feffff             mov        dword ptr [ebp - 0x118], esp
004df0f3 8d55e9                   lea        edx, [ebp - 0x17]
004df0f6 52                       push       edx
004df0f7 e8e4f5ffff               call       0x4de6e0
004df0fc 6a00                     push       0
004df0fe 6a00                     push       0
004df100 6a07                     push       7
004df102 6a00                     push       0
004df104 6880d0ff00               push       0xffd080
004df109 8b855cffffff             mov        eax, dword ptr [ebp - 0xa4]
004df10f 50                       push       eax
004df110 8d8dd0feffff             lea        ecx, [ebp - 0x130]
004df116 51                       push       ecx
004df117 8b1528005c00             mov        edx, dword ptr [0x5c0028]
004df11d 52                       push       edx
004df11e e8adbcf6ff               call       0x44add0
004df123 83c450                   add        esp, 0x50
004df126 e993000000               jmp        0x4df1be
004df12b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df12e 83c124                   add        ecx, 0x24
004df131 e8cad1f2ff               call       0x40c300
004df136 6bc038                   imul       eax, eax, 0x38
004df139 b91c000000               mov        ecx, 0x1c
004df13e 6bd100                   imul       edx, ecx, 0
004df141 8d8c10405b5c00           lea        ecx, [eax + edx + 0x5c5b40]
004df148 e8a3c9f6ff               call       0x44baf0
004df14d 898554ffffff             mov        dword ptr [ebp - 0xac], eax
004df153 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df156 83c070                   add        eax, 0x70
004df159 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
004df15f 6a05                     push       5
004df161 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
004df167 e81454f3ff               call       0x414580
004df16c 8bc8                     mov        ecx, eax
004df16e e85dddf6ff               call       0x44ced0
004df173 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
004df179 8b8d54ffffff             mov        ecx, dword ptr [ebp - 0xac]
004df17f 51                       push       ecx
004df180 83ec28                   sub        esp, 0x28
004df183 8bcc                     mov        ecx, esp
004df185 89a5f4feffff             mov        dword ptr [ebp - 0x10c], esp
004df18b 8d55e8                   lea        edx, [ebp - 0x18]
004df18e 52                       push       edx
004df18f e8bcf5ffff               call       0x4de750
004df194 6a00                     push       0
004df196 6a00                     push       0
004df198 6a07                     push       7
004df19a 6a00                     push       0
004df19c 6880d0ff00               push       0xffd080
004df1a1 8b8550ffffff             mov        eax, dword ptr [ebp - 0xb0]
004df1a7 50                       push       eax
004df1a8 8d8dc8feffff             lea        ecx, [ebp - 0x138]
004df1ae 51                       push       ecx
004df1af 8b1528005c00             mov        edx, dword ptr [0x5c0028]
004df1b5 52                       push       edx
004df1b6 e815bcf6ff               call       0x44add0
004df1bb 83c44c                   add        esp, 0x4c
004df1be 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df1c1 83c124                   add        ecx, 0x24
004df1c4 e837d1f2ff               call       0x40c300
004df1c9 6bc038                   imul       eax, eax, 0x38
004df1cc b91c000000               mov        ecx, 0x1c
004df1d1 c1e100                   shl        ecx, 0
004df1d4 8d8c08405b5c00           lea        ecx, [eax + ecx + 0x5c5b40]
004df1db e810c9f6ff               call       0x44baf0
004df1e0 898548ffffff             mov        dword ptr [ebp - 0xb8], eax
004df1e6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df1e9 83c270                   add        edx, 0x70
004df1ec 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
004df1f2 6a06                     push       6
004df1f4 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
004df1fa e88153f3ff               call       0x414580
004df1ff 8bc8                     mov        ecx, eax
004df201 e8cadcf6ff               call       0x44ced0
004df206 898544ffffff             mov        dword ptr [ebp - 0xbc], eax
004df20c 8b8548ffffff             mov        eax, dword ptr [ebp - 0xb8]
004df212 50                       push       eax
004df213 83ec28                   sub        esp, 0x28
004df216 8bcc                     mov        ecx, esp
004df218 89a5f0feffff             mov        dword ptr [ebp - 0x110], esp
004df21e 8d55e7                   lea        edx, [ebp - 0x19]
004df221 52                       push       edx
004df222 e899f5ffff               call       0x4de7c0
004df227 6a00                     push       0
004df229 6a00                     push       0
004df22b 6a08                     push       8
004df22d 6a00                     push       0
004df22f 6880d0ff00               push       0xffd080
004df234 8b8544ffffff             mov        eax, dword ptr [ebp - 0xbc]
004df23a 50                       push       eax
004df23b 8d8dc0feffff             lea        ecx, [ebp - 0x140]
004df241 51                       push       ecx
004df242 8b1528005c00             mov        edx, dword ptr [0x5c0028]
004df248 52                       push       edx
004df249 e882bbf6ff               call       0x44add0
004df24e 83c44c                   add        esp, 0x4c
004df251 6a00                     push       0
004df253 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df256 83c114                   add        ecx, 0x14
004df259 e8c242f4ff               call       0x423520
004df25e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df261 83c124                   add        ecx, 0x24
004df264 e897d0f2ff               call       0x40c300
004df269 50                       push       eax
004df26a 68e81f5700               push       0x571fe8
004df26f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df272 05ac000000               add        eax, 0xac
004df277 50                       push       eax
004df278 e8c3fbfdff               call       0x4bee40
004df27d 83c40c                   add        esp, 0xc
004df280 e929030000               jmp        0x4df5ae
004df285 e924030000               jmp        0x4df5ae
004df28a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df28d 8b912c010000             mov        edx, dword ptr [ecx + 0x12c]
004df293 89953cffffff             mov        dword ptr [ebp - 0xc4], edx
004df299 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df29c 8b88a4000000             mov        ecx, dword ptr [eax + 0xa4]
004df2a2 898d38ffffff             mov        dword ptr [ebp - 0xc8], ecx
004df2a8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df2ab 8b829c000000             mov        eax, dword ptr [edx + 0x9c]
004df2b1 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
004df2b7 6a01                     push       1
004df2b9 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
004df2bf e81c08feff               call       0x4bfae0
004df2c4 898534ffffff             mov        dword ptr [ebp - 0xcc], eax
004df2ca 6a00                     push       0
004df2cc 6a00                     push       0
004df2ce 6a01                     push       1
004df2d0 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
004df2d6 51                       push       ecx
004df2d7 8b9538ffffff             mov        edx, dword ptr [ebp - 0xc8]
004df2dd 52                       push       edx
004df2de 8b8534ffffff             mov        eax, dword ptr [ebp - 0xcc]
004df2e4 50                       push       eax
004df2e5 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004df2eb e830e9f6ff               call       0x44dc20
004df2f0 90                       nop        
004df2f1 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df2f4 83b9a400000000           cmp        dword ptr [ecx + 0xa4], 0
004df2fb 7422                     je         0x4df31f
004df2fd 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df300 8b82a4000000             mov        eax, dword ptr [edx + 0xa4]
004df306 50                       push       eax
004df307 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004df30d e85e03f4ff               call       0x41f670
004df312 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df315 c781a400000000000000     mov        dword ptr [ecx + 0xa4], 0
004df31f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df322 c782a400000000000000     mov        dword ptr [edx + 0xa4], 0
004df32c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df32f 8b889c000000             mov        ecx, dword ptr [eax + 0x9c]
004df335 898d30ffffff             mov        dword ptr [ebp - 0xd0], ecx
004df33b 6a01                     push       1
004df33d 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
004df343 e8b807feff               call       0x4bfb00
004df348 8945d8                   mov        dword ptr [ebp - 0x28], eax
004df34b 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004df34e 8b02                     mov        eax, dword ptr [edx]
004df350 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004df353 51                       push       ecx
004df354 8b5024                   mov        edx, dword ptr [eax + 0x24]
004df357 ffd2                     call       edx
004df359 6a00                     push       0
004df35b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df35e e8cd070000               call       0x4dfb30
004df363 6a04                     push       4
004df365 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df368 e8c3070000               call       0x4dfb30
004df36d 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df370 c780a800000004000000     mov        dword ptr [eax + 0xa8], 4
004df37a 6a00                     push       0
004df37c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df37f 83c114                   add        ecx, 0x14
004df382 e89941f4ff               call       0x423520
004df387 90                       nop        
004df388 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df38b 83c114                   add        ecx, 0x14
004df38e 898d2cffffff             mov        dword ptr [ebp - 0xd4], ecx
004df394 6a14                     push       0x14
004df396 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
004df39c e8bf41f4ff               call       0x423560
004df3a1 0fb6d0                   movzx      edx, al
004df3a4 85d2                     test       edx, edx
004df3a6 7405                     je         0x4df3ad
004df3a8 e901020000               jmp        0x4df5ae
004df3ad 33c0                     xor        eax, eax
004df3af 7538                     jne        0x4df3e9
004df3b1 b904000000               mov        ecx, 4
004df3b6 d1e1                     shl        ecx, 1
004df3b8 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
004df3bf 7428                     je         0x4df3e9
004df3c1 ba04000000               mov        edx, 4
004df3c6 d1e2                     shl        edx, 1
004df3c8 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
004df3ce 898528ffffff             mov        dword ptr [ebp - 0xd8], eax
004df3d4 6801000800               push       0x80001
004df3d9 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
004df3df e81ca8f3ff               call       0x419c00
004df3e4 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004df3e7 eb07                     jmp        0x4df3f0
004df3e9 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
004df3f0 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
004df3f4 7460                     je         0x4df456
004df3f6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df3f9 c7411002000000           mov        dword ptr [ecx + 0x10], 2
004df400 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df403 c782a800000000000000     mov        dword ptr [edx + 0xa8], 0
004df40d 6a00                     push       0
004df40f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df412 83c114                   add        ecx, 0x14
004df415 e80641f4ff               call       0x423520
004df41a 6a00                     push       0
004df41c 6a07                     push       7
004df41e b930a85b00               mov        ecx, 0x5ba830
004df423 e84879f4ff               call       0x426d70
004df428 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df42b 83c024                   add        eax, 0x24
004df42e 898524ffffff             mov        dword ptr [ebp - 0xdc], eax
004df434 6a00                     push       0
004df436 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
004df43c e8dffbfdff               call       0x4bf020
004df441 6a04                     push       4
004df443 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df446 e8a5060000               call       0x4dfaf0
004df44b 6a00                     push       0
004df44d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df450 e89b060000               call       0x4dfaf0
004df455 90                       nop        
004df456 e953010000               jmp        0x4df5ae
004df45b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df45e 83c114                   add        ecx, 0x14
004df461 898d20ffffff             mov        dword ptr [ebp - 0xe0], ecx
004df467 6a14                     push       0x14
004df469 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
004df46f e84c41f4ff               call       0x4235c0
004df474 0fb6d0                   movzx      edx, al
004df477 85d2                     test       edx, edx
004df479 7405                     je         0x4df480
004df47b e978f9ffff               jmp        0x4dedf8
004df480 e929010000               jmp        0x4df5ae
004df485 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df488 8b88a0000000             mov        ecx, dword ptr [eax + 0xa0]
004df48e 898d00ffffff             mov        dword ptr [ebp - 0x100], ecx
004df494 6a00                     push       0
004df496 6aff                     push       -1
004df498 6a01                     push       1
004df49a 6a00                     push       0
004df49c 8d95ecfeffff             lea        edx, [ebp - 0x114]
004df4a2 52                       push       edx
004df4a3 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
004df4a9 e8c217f7ff               call       0x450c70
004df4ae 8b00                     mov        eax, dword ptr [eax]
004df4b0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df4b3 89818c000000             mov        dword ptr [ecx + 0x8c], eax
004df4b9 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df4bc c782a800000007000000     mov        dword ptr [edx + 0xa8], 7
004df4c6 6a00                     push       0
004df4c8 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df4cb 83c114                   add        ecx, 0x14
004df4ce e84d40f4ff               call       0x423520
004df4d3 90                       nop        
004df4d4 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df4d7 83c014                   add        eax, 0x14
004df4da 898518ffffff             mov        dword ptr [ebp - 0xe8], eax
004df4e0 68f0000000               push       0xf0
004df4e5 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
004df4eb e87040f4ff               call       0x423560
004df4f0 0fb6c8                   movzx      ecx, al
004df4f3 85c9                     test       ecx, ecx
004df4f5 7405                     je         0x4df4fc
004df4f7 e9b2000000               jmp        0x4df5ae
004df4fc 33d2                     xor        edx, edx
004df4fe 7538                     jne        0x4df538
004df500 b804000000               mov        eax, 4
004df505 d1e0                     shl        eax, 1
004df507 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
004df50e 7428                     je         0x4df538
004df510 b904000000               mov        ecx, 4
004df515 d1e1                     shl        ecx, 1
004df517 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
004df51d 899514ffffff             mov        dword ptr [ebp - 0xec], edx
004df523 6801000800               push       0x80001
004df528 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
004df52e e8cda6f3ff               call       0x419c00
004df533 8945d0                   mov        dword ptr [ebp - 0x30], eax
004df536 eb07                     jmp        0x4df53f
004df538 c745d000000000           mov        dword ptr [ebp - 0x30], 0
004df53f 837dd000                 cmp        dword ptr [ebp - 0x30], 0
004df543 7469                     je         0x4df5ae
004df545 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df548 83c124                   add        ecx, 0x24
004df54b e8b0cdf2ff               call       0x40c300
004df550 85c0                     test       eax, eax
004df552 7e5a                     jle        0x4df5ae
004df554 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df557 c7401003000000           mov        dword ptr [eax + 0x10], 3
004df55e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df561 c781a800000000000000     mov        dword ptr [ecx + 0xa8], 0
004df56b 6a00                     push       0
004df56d 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df570 83c114                   add        ecx, 0x14
004df573 e8a83ff4ff               call       0x423520
004df578 6a00                     push       0
004df57a 6a07                     push       7
004df57c b930a85b00               mov        ecx, 0x5ba830
004df581 e8ea77f4ff               call       0x426d70
004df586 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df589 83c224                   add        edx, 0x24
004df58c 899510ffffff             mov        dword ptr [ebp - 0xf0], edx
004df592 6a00                     push       0
004df594 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
004df59a e881fafdff               call       0x4bf020
004df59f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004df5a2 81c18c000000             add        ecx, 0x8c
004df5a8 e8939af9ff               call       0x479040
004df5ad 90                       nop        
004df5ae eb5f                     jmp        0x4df60f
004df5b0 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df5b3 83c014                   add        eax, 0x14
004df5b6 89850cffffff             mov        dword ptr [ebp - 0xf4], eax
004df5bc 6a1e                     push       0x1e
004df5be 8b8d0cffffff             mov        ecx, dword ptr [ebp - 0xf4]
004df5c4 e8f73ff4ff               call       0x4235c0
004df5c9 0fb6c8                   movzx      ecx, al
004df5cc 85c9                     test       ecx, ecx
004df5ce 740d                     je         0x4df5dd
004df5d0 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df5d3 c7829000000001000000     mov        dword ptr [edx + 0x90], 1
004df5dd eb30                     jmp        0x4df60f
004df5df 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df5e2 83c014                   add        eax, 0x14
004df5e5 898508ffffff             mov        dword ptr [ebp - 0xf8], eax
004df5eb 68f0000000               push       0xf0
004df5f0 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
004df5f6 e8c53ff4ff               call       0x4235c0
004df5fb 0fb6c8                   movzx      ecx, al
004df5fe 85c9                     test       ecx, ecx
004df600 740d                     je         0x4df60f
004df602 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004df605 c7829000000001000000     mov        dword ptr [edx + 0x90], 1
004df60f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004df612 83c014                   add        eax, 0x14
004df615 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
004df61b 6a00                     push       0
004df61d 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
004df623 e8183ff4ff               call       0x423540
004df628 b801000000               mov        eax, 1
004df62d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004df630 33cd                     xor        ecx, ebp
004df632 e8b7330600               call       0x5429ee
004df637 8be5                     mov        esp, ebp
004df639 5d                       pop        ebp
004df63a c3                       ret        
004df63b 90                       nop        
004df63c 4f                       dec        edi
004df63d ed                       in         eax, dx
004df63e 4d                       dec        ebp
004df63f 005eed                   add        byte ptr [esi - 0x13], bl
004df642 4d                       dec        ebp
004df643 00b0f54d00df             add        byte ptr [eax - 0x20ffb20b], dh
004df649 f5                       cmc        
004df64a 4d                       dec        ebp
004df64b 007eed                   add        byte ptr [esi - 0x13], bh
004df64e 4d                       dec        ebp
004df64f 00aef54d00ae             add        byte ptr [esi - 0x51ffb20b], ch
004df655 f5                       cmc        
004df656 4d                       dec        ebp
004df657 008af24d0088             add        byte ptr [edx - 0x77ffb20e], cl
004df65d f34d                     dec        ebp
004df65f 005bf4                   add        byte ptr [ebx - 0xc], bl
004df662 4d                       dec        ebp
004df663 0085f44d00d4             add        byte ptr [ebp - 0x2bffb20c], al
004df669 f4                       hlt        
004df66a 4d                       dec        ebp
004df66b 00cc                     add        ah, cl
004df66d cc                       int3       
004df66e cc                       int3       
004df66f cc                       int3       
