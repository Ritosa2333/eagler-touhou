00529430 55                       push       ebp
00529431 8bec                     mov        ebp, esp
00529433 83ec64                   sub        esp, 0x64
00529436 894dfc                   mov        dword ptr [ebp - 4], ecx
00529439 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052943c 8b4820                   mov        ecx, dword ptr [eax + 0x20]
0052943f 894df4                   mov        dword ptr [ebp - 0xc], ecx
00529442 837df404                 cmp        dword ptr [ebp - 0xc], 4
00529446 0f87c9060000             ja         0x529b15
0052944c 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0052944f ff2495209b5200           jmp        dword ptr [edx*4 + 0x529b20]
00529456 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529459 83c024                   add        eax, 0x24
0052945c 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052945f 6a06                     push       6
00529461 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00529464 e87730f7ff               call       0x49c4e0
00529469 8b0d500a5b00             mov        ecx, dword ptr [0x5b0a50]
0052946f 51                       push       ecx
00529470 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529473 83c124                   add        ecx, 0x24
00529476 e8d558f9ff               call       0x4bed50
0052947b 6a2b                     push       0x2b
0052947d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529480 e8fb3a0000               call       0x52cf80
00529485 6a01                     push       1
00529487 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052948a e8c1390000               call       0x52ce50
0052948f 90                       nop        
00529490 833d28615c0004           cmp        dword ptr [0x5c6128], 4
00529497 750a                     jne        0x5294a3
00529499 c70528615c0001000000     mov        dword ptr [0x5c6128], 1
005294a3 8b55fc                   mov        edx, dword ptr [ebp - 4]
005294a6 81c254010000             add        edx, 0x154
005294ac 8955dc                   mov        dword ptr [ebp - 0x24], edx
005294af 6a0a                     push       0xa
005294b1 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
005294b4 e8b77bf3ff               call       0x461070
005294b9 0fb6c0                   movzx      eax, al
005294bc 85c0                     test       eax, eax
005294be 740b                     je         0x5294cb
005294c0 6a02                     push       2
005294c2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005294c5 e886390000               call       0x52ce50
005294ca 90                       nop        
005294cb e945060000               jmp        0x529b15
005294d0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005294d3 83c124                   add        ecx, 0x24
005294d6 e82567f9ff               call       0x4bfc00
005294db 90                       nop        
005294dc 33c9                     xor        ecx, ecx
005294de 7531                     jne        0x529511
005294e0 ba04000000               mov        edx, 4
005294e5 6bc200                   imul       eax, edx, 0
005294e8 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
005294ef 7420                     je         0x529511
005294f1 b904000000               mov        ecx, 4
005294f6 6bd100                   imul       edx, ecx, 0
005294f9 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
005294ff 8945d8                   mov        dword ptr [ebp - 0x28], eax
00529502 6a10                     push       0x10
00529504 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00529507 e8740defff               call       0x41a280
0052950c 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052950f eb07                     jmp        0x529518
00529511 c745f000000000           mov        dword ptr [ebp - 0x10], 0
00529518 837df000                 cmp        dword ptr [ebp - 0x10], 0
0052951c 7414                     je         0x529532
0052951e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529521 83c124                   add        ecx, 0x24
00529524 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00529527 6a00                     push       0
00529529 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052952c e80f5bf9ff               call       0x4bf040
00529531 90                       nop        
00529532 33d2                     xor        edx, edx
00529534 7531                     jne        0x529567
00529536 b804000000               mov        eax, 4
0052953b 6bc800                   imul       ecx, eax, 0
0052953e 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
00529545 7420                     je         0x529567
00529547 ba04000000               mov        edx, 4
0052954c 6bc200                   imul       eax, edx, 0
0052954f 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
00529555 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00529558 6a20                     push       0x20
0052955a 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0052955d e81e0defff               call       0x41a280
00529562 8945ec                   mov        dword ptr [ebp - 0x14], eax
00529565 eb07                     jmp        0x52956e
00529567 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0052956e 837dec00                 cmp        dword ptr [ebp - 0x14], 0
00529572 7414                     je         0x529588
00529574 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529577 83c224                   add        edx, 0x24
0052957a 8955cc                   mov        dword ptr [ebp - 0x34], edx
0052957d 6a00                     push       0
0052957f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00529582 e8995af9ff               call       0x4bf020
00529587 90                       nop        
00529588 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052958b 83c124                   add        ecx, 0x24
0052958e e89d3af3ff               call       0x45d030
00529593 85c0                     test       eax, eax
00529595 740f                     je         0x5295a6
00529597 6a00                     push       0
00529599 6a0a                     push       0xa
0052959b b930a85b00               mov        ecx, 0x5ba830
005295a0 e8cbd7efff               call       0x426d70
005295a5 90                       nop        
005295a6 33c0                     xor        eax, eax
005295a8 7534                     jne        0x5295de
005295aa b904000000               mov        ecx, 4
005295af 6bd100                   imul       edx, ecx, 0
005295b2 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
005295b9 7423                     je         0x5295de
005295bb b804000000               mov        eax, 4
005295c0 6bc800                   imul       ecx, eax, 0
005295c3 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
005295c9 8955c8                   mov        dword ptr [ebp - 0x38], edx
005295cc 6806010000               push       0x106
005295d1 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
005295d4 e82706efff               call       0x419c00
005295d9 8945e8                   mov        dword ptr [ebp - 0x18], eax
005295dc eb07                     jmp        0x5295e5
005295de c745e800000000           mov        dword ptr [ebp - 0x18], 0
005295e5 837de800                 cmp        dword ptr [ebp - 0x18], 0
005295e9 742d                     je         0x529618
005295eb 6a04                     push       4
005295ed 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005295f0 e85b380000               call       0x52ce50
005295f5 6a00                     push       0
005295f7 6a09                     push       9
005295f9 b930a85b00               mov        ecx, 0x5ba830
005295fe e86dd7efff               call       0x426d70
00529603 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529606 83c124                   add        ecx, 0x24
00529609 e8f22ceeff               call       0x40c300
0052960e a3500a5b00               mov        dword ptr [0x5b0a50], eax
00529613 e9fd040000               jmp        0x529b15
00529618 33c0                     xor        eax, eax
0052961a 7534                     jne        0x529650
0052961c b904000000               mov        ecx, 4
00529621 6bd100                   imul       edx, ecx, 0
00529624 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0052962b 7423                     je         0x529650
0052962d b804000000               mov        eax, 4
00529632 6bc800                   imul       ecx, eax, 0
00529635 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0052963b 8955c4                   mov        dword ptr [ebp - 0x3c], edx
0052963e 6801000800               push       0x80001
00529643 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
00529646 e8b505efff               call       0x419c00
0052964b 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052964e eb07                     jmp        0x529657
00529650 c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00529657 837de400                 cmp        dword ptr [ebp - 0x1c], 0
0052965b 0f8448030000             je         0x5299a9
00529661 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
00529667 e8e465feff               call       0x50fc50
0052966c 05f8760000               add        eax, 0x76f8
00529671 8945c0                   mov        dword ptr [ebp - 0x40], eax
00529674 b968a55b00               mov        ecx, 0x5ba568
00529679 e822aaf3ff               call       0x4640a0
0052967e 69c090000000             imul       eax, eax, 0x90
00529684 8945bc                   mov        dword ptr [ebp - 0x44], eax
00529687 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0052968a 034dbc                   add        ecx, dword ptr [ebp - 0x44]
0052968d 894db8                   mov        dword ptr [ebp - 0x48], ecx
00529690 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529693 83c124                   add        ecx, 0x24
00529696 e8652ceeff               call       0x40c300
0052969b c1e004                   shl        eax, 4
0052969e 8945b4                   mov        dword ptr [ebp - 0x4c], eax
005296a1 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005296a4 034db4                   add        ecx, dword ptr [ebp - 0x4c]
005296a7 e884330000               call       0x52ca30
005296ac 0fb6d0                   movzx      edx, al
005296af 85d2                     test       edx, edx
005296b1 7516                     jne        0x5296c9
005296b3 6a00                     push       0
005296b5 6a10                     push       0x10
005296b7 b930a85b00               mov        ecx, 0x5ba830
005296bc e8afd6efff               call       0x426d70
005296c1 90                       nop        
005296c2 e94e040000               jmp        0x529b15
005296c7 eb27                     jmp        0x5296f0
005296c9 6a03                     push       3
005296cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005296ce e87d370000               call       0x52ce50
005296d3 6a00                     push       0
005296d5 6a07                     push       7
005296d7 b930a85b00               mov        ecx, 0x5ba830
005296dc e88fd6efff               call       0x426d70
005296e1 6a00                     push       0
005296e3 6a32                     push       0x32
005296e5 b930a85b00               mov        ecx, 0x5ba830
005296ea e881d6efff               call       0x426d70
005296ef 90                       nop        
005296f0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005296f3 83c124                   add        ecx, 0x24
005296f6 e8052ceeff               call       0x40c300
005296fb a3500a5b00               mov        dword ptr [0x5b0a50], eax
00529700 c745f800000000           mov        dword ptr [ebp - 8], 0
00529707 6a00                     push       0
00529709 b968a55b00               mov        ecx, 0x5ba568
0052970e e89d370000               call       0x52ceb0
00529713 90                       nop        
00529714 6848615c00               push       0x5c6148
00529719 e8f25bffff               call       0x51f310
0052971e 83c404                   add        esp, 4
00529721 85c0                     test       eax, eax
00529723 0f852f010000             jne        0x529858
00529729 b801000000               mov        eax, 1
0052972e 6bc831                   imul       ecx, eax, 0x31
00529731 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
00529738 81e280000000             and        edx, 0x80
0052973e 740c                     je         0x52974c
00529740 c745f801000000           mov        dword ptr [ebp - 8], 1
00529747 e907010000               jmp        0x529853
0052974c b801000000               mov        eax, 1
00529751 6bc832                   imul       ecx, eax, 0x32
00529754 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
0052975b 81e280000000             and        edx, 0x80
00529761 740c                     je         0x52976f
00529763 c745f802000000           mov        dword ptr [ebp - 8], 2
0052976a e9e4000000               jmp        0x529853
0052976f b801000000               mov        eax, 1
00529774 6bc833                   imul       ecx, eax, 0x33
00529777 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
0052977e 81e280000000             and        edx, 0x80
00529784 740c                     je         0x529792
00529786 c745f803000000           mov        dword ptr [ebp - 8], 3
0052978d e9c1000000               jmp        0x529853
00529792 b801000000               mov        eax, 1
00529797 6bc834                   imul       ecx, eax, 0x34
0052979a 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
005297a1 81e280000000             and        edx, 0x80
005297a7 740c                     je         0x5297b5
005297a9 c745f804000000           mov        dword ptr [ebp - 8], 4
005297b0 e99e000000               jmp        0x529853
005297b5 b801000000               mov        eax, 1
005297ba 6bc835                   imul       ecx, eax, 0x35
005297bd 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
005297c4 81e280000000             and        edx, 0x80
005297ca 7409                     je         0x5297d5
005297cc c745f805000000           mov        dword ptr [ebp - 8], 5
005297d3 eb7e                     jmp        0x529853
005297d5 b801000000               mov        eax, 1
005297da 6bc836                   imul       ecx, eax, 0x36
005297dd 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
005297e4 81e280000000             and        edx, 0x80
005297ea 7409                     je         0x5297f5
005297ec c745f806000000           mov        dword ptr [ebp - 8], 6
005297f3 eb5e                     jmp        0x529853
005297f5 b801000000               mov        eax, 1
005297fa 6bc837                   imul       ecx, eax, 0x37
005297fd 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
00529804 81e280000000             and        edx, 0x80
0052980a 7409                     je         0x529815
0052980c c745f807000000           mov        dword ptr [ebp - 8], 7
00529813 eb3e                     jmp        0x529853
00529815 b801000000               mov        eax, 1
0052981a 6bc838                   imul       ecx, eax, 0x38
0052981d 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
00529824 81e280000000             and        edx, 0x80
0052982a 7409                     je         0x529835
0052982c c745f808000000           mov        dword ptr [ebp - 8], 8
00529833 eb1e                     jmp        0x529853
00529835 b801000000               mov        eax, 1
0052983a 6bc839                   imul       ecx, eax, 0x39
0052983d 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
00529844 81e280000000             and        edx, 0x80
0052984a 7407                     je         0x529853
0052984c c745f809000000           mov        dword ptr [ebp - 8], 9
00529853 e925010000               jmp        0x52997d
00529858 b801000000               mov        eax, 1
0052985d d1e0                     shl        eax, 1
0052985f 0fb68848615c00           movzx      ecx, byte ptr [eax + 0x5c6148]
00529866 81e180000000             and        ecx, 0x80
0052986c 740c                     je         0x52987a
0052986e c745f801000000           mov        dword ptr [ebp - 8], 1
00529875 e903010000               jmp        0x52997d
0052987a ba01000000               mov        edx, 1
0052987f 6bc203                   imul       eax, edx, 3
00529882 0fb68848615c00           movzx      ecx, byte ptr [eax + 0x5c6148]
00529889 81e180000000             and        ecx, 0x80
0052988f 740c                     je         0x52989d
00529891 c745f802000000           mov        dword ptr [ebp - 8], 2
00529898 e9e0000000               jmp        0x52997d
0052989d ba01000000               mov        edx, 1
005298a2 c1e202                   shl        edx, 2
005298a5 0fb68248615c00           movzx      eax, byte ptr [edx + 0x5c6148]
005298ac 2580000000               and        eax, 0x80
005298b1 740c                     je         0x5298bf
005298b3 c745f803000000           mov        dword ptr [ebp - 8], 3
005298ba e9be000000               jmp        0x52997d
005298bf b901000000               mov        ecx, 1
005298c4 6bd105                   imul       edx, ecx, 5
005298c7 0fb68248615c00           movzx      eax, byte ptr [edx + 0x5c6148]
005298ce 2580000000               and        eax, 0x80
005298d3 740c                     je         0x5298e1
005298d5 c745f804000000           mov        dword ptr [ebp - 8], 4
005298dc e99c000000               jmp        0x52997d
005298e1 b901000000               mov        ecx, 1
005298e6 6bd106                   imul       edx, ecx, 6
005298e9 0fb68248615c00           movzx      eax, byte ptr [edx + 0x5c6148]
005298f0 2580000000               and        eax, 0x80
005298f5 7409                     je         0x529900
005298f7 c745f805000000           mov        dword ptr [ebp - 8], 5
005298fe eb7d                     jmp        0x52997d
00529900 b901000000               mov        ecx, 1
00529905 6bd107                   imul       edx, ecx, 7
00529908 0fb68248615c00           movzx      eax, byte ptr [edx + 0x5c6148]
0052990f 2580000000               and        eax, 0x80
00529914 7409                     je         0x52991f
00529916 c745f806000000           mov        dword ptr [ebp - 8], 6
0052991d eb5e                     jmp        0x52997d
0052991f b901000000               mov        ecx, 1
00529924 c1e103                   shl        ecx, 3
00529927 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
0052992e 81e280000000             and        edx, 0x80
00529934 7409                     je         0x52993f
00529936 c745f807000000           mov        dword ptr [ebp - 8], 7
0052993d eb3e                     jmp        0x52997d
0052993f b801000000               mov        eax, 1
00529944 6bc809                   imul       ecx, eax, 9
00529947 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
0052994e 81e280000000             and        edx, 0x80
00529954 7409                     je         0x52995f
00529956 c745f808000000           mov        dword ptr [ebp - 8], 8
0052995d eb1e                     jmp        0x52997d
0052995f b801000000               mov        eax, 1
00529964 6bc80a                   imul       ecx, eax, 0xa
00529967 0fb69148615c00           movzx      edx, byte ptr [ecx + 0x5c6148]
0052996e 81e280000000             and        edx, 0x80
00529974 7407                     je         0x52997d
00529976 c745f809000000           mov        dword ptr [ebp - 8], 9
0052997d 8b45f8                   mov        eax, dword ptr [ebp - 8]
00529980 50                       push       eax
00529981 b968a55b00               mov        ecx, 0x5ba568
00529986 e825350000               call       0x52ceb0
0052998b 51                       push       ecx
0052998c f30f10055cfe5600         movss      xmm0, dword ptr [0x56fe5c]
00529994 f30f110424               movss      dword ptr [esp], xmm0
00529999 b9404d5c00               mov        ecx, 0x5c4d40
0052999e e82d00fbff               call       0x4d99d0
005299a3 90                       nop        
005299a4 e96c010000               jmp        0x529b15
005299a9 e967010000               jmp        0x529b15
005299ae 6a00                     push       0
005299b0 e86bdbf0ff               call       0x437520
005299b5 83c404                   add        esp, 4
005299b8 8bc8                     mov        ecx, eax
005299ba e861ebf0ff               call       0x438520
005299bf 85c0                     test       eax, eax
005299c1 7520                     jne        0x5299e3
005299c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005299c6 81c154010000             add        ecx, 0x154
005299cc 894db0                   mov        dword ptr [ebp - 0x50], ecx
005299cf 6a00                     push       0
005299d1 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005299d4 e847faefff               call       0x429420
005299d9 b801000000               mov        eax, 1
005299de e937010000               jmp        0x529b1a
005299e3 8b55fc                   mov        edx, dword ptr [ebp - 4]
005299e6 81c254010000             add        edx, 0x154
005299ec 8955ac                   mov        dword ptr [ebp - 0x54], edx
005299ef 6a0a                     push       0xa
005299f1 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005299f4 e8e7ebf4ff               call       0x4785e0
005299f9 0fb6c0                   movzx      eax, al
005299fc 85c0                     test       eax, eax
005299fe 745c                     je         0x529a5c
00529a00 51                       push       ecx
00529a01 f30f100588035700         movss      xmm0, dword ptr [0x570388]
00529a09 f30f110424               movss      dword ptr [esp], xmm0
00529a0e 51                       push       ecx
00529a0f f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
00529a17 f30f110424               movss      dword ptr [esp], xmm0
00529a1c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00529a22 e87970f7ff               call       0x4a0aa0
00529a27 90                       nop        
00529a28 6a00                     push       0
00529a2a e8f1daf0ff               call       0x437520
00529a2f 83c404                   add        esp, 4
00529a32 8945a8                   mov        dword ptr [ebp - 0x58], eax
00529a35 6a00                     push       0
00529a37 6a00                     push       0
00529a39 6a00                     push       0
00529a3b 8d4d9c                   lea        ecx, [ebp - 0x64]
00529a3e 51                       push       ecx
00529a3f 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00529a42 e8a942f7ff               call       0x49dcf0
00529a47 8b10                     mov        edx, dword ptr [eax]
00529a49 8915044f5c00             mov        dword ptr [0x5c4f04], edx
00529a4f 6a00                     push       0
00529a51 b9044f5c00               mov        ecx, 0x5c4f04
00529a56 e8552cffff               call       0x51c6b0
00529a5b 90                       nop        
00529a5c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00529a5f 0554010000               add        eax, 0x154
00529a64 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00529a67 6a28                     push       0x28
00529a69 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
00529a6c e84f9befff               call       0x4235c0
00529a71 0fb6c8                   movzx      ecx, al
00529a74 85c9                     test       ecx, ecx
00529a76 745e                     je         0x529ad6
00529a78 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529a7b 83c124                   add        ecx, 0x24
00529a7e e82dc6f9ff               call       0x4c60b0
00529a83 6a02                     push       2
00529a85 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529a88 e843340000               call       0x52ced0
00529a8d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529a90 83c124                   add        ecx, 0x24
00529a93 e86828eeff               call       0x40c300
00529a98 83c001                   add        eax, 1
00529a9b 50                       push       eax
00529a9c b968a55b00               mov        ecx, 0x5ba568
00529aa1 e8ba48f9ff               call       0x4be360
00529aa6 c705fcfc5a00ffffffff     mov        dword ptr [0x5afcfc], 0xffffffff
00529ab0 6a07                     push       7
00529ab2 b9404d5c00               mov        ecx, 0x5c4d40
00529ab7 e89447f9ff               call       0x4be250
00529abc c70528615c0004000000     mov        dword ptr [0x5c6128], 4
00529ac6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529ac9 83c124                   add        ecx, 0x24
00529acc e82f28eeff               call       0x40c300
00529ad1 a3500a5b00               mov        dword ptr [0x5b0a50], eax
00529ad6 eb3d                     jmp        0x529b15
00529ad8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00529adb 81c254010000             add        edx, 0x154
00529ae1 8955a0                   mov        dword ptr [ebp - 0x60], edx
00529ae4 6a06                     push       6
00529ae6 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00529ae9 e8d29aefff               call       0x4235c0
00529aee 0fb6c0                   movzx      eax, al
00529af1 85c0                     test       eax, eax
00529af3 7420                     je         0x529b15
00529af5 6a2b                     push       0x2b
00529af7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529afa e831310000               call       0x52cc30
00529aff 6a07                     push       7
00529b01 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529b04 e8c7330000               call       0x52ced0
00529b09 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00529b0c 83c124                   add        ecx, 0x24
00529b0f e8fcc4f9ff               call       0x4c6010
00529b14 90                       nop        
00529b15 b801000000               mov        eax, 1
00529b1a 8be5                     mov        esp, ebp
00529b1c 5d                       pop        ebp
00529b1d c3                       ret        
00529b1e 6690                     nop        
00529b20 56                       push       esi
00529b21 94                       xchg       esp, eax
00529b22 52                       push       edx
00529b23 00a3945200d0             add        byte ptr [ebx - 0x2fffad6c], ah
00529b29 94                       xchg       esp, eax
00529b2a 52                       push       edx
00529b2b 00ae995200d8             add        byte ptr [esi - 0x27ffad67], ch
00529b31 9a5200cccccccc           lcall      0xcccc, 0xcccc0052
00529b38 cc                       int3       
00529b39 cc                       int3       
00529b3a cc                       int3       
00529b3b cc                       int3       
00529b3c cc                       int3       
00529b3d cc                       int3       
00529b3e cc                       int3       
00529b3f cc                       int3       
