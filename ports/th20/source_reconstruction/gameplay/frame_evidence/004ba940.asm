004ba940 55                       push       ebp
004ba941 8bec                     mov        ebp, esp
004ba943 81eca8000000             sub        esp, 0xa8
004ba949 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004ba94e 33c5                     xor        eax, ebp
004ba950 8945fc                   mov        dword ptr [ebp - 4], eax
004ba953 894da4                   mov        dword ptr [ebp - 0x5c], ecx
004ba956 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
004ba959 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004ba95f 81c900000004             or         ecx, 0x4000000
004ba965 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
004ba968 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004ba96e 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
004ba971 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004ba977 c1e903                   shr        ecx, 3
004ba97a 83e101                   and        ecx, 1
004ba97d 7420                     je         0x4ba99f
004ba97f b9404d5c00               mov        ecx, 0x5c4d40
004ba984 e8a7f40100               call       0x4d9e30
004ba989 6a03                     push       3
004ba98b b9404d5c00               mov        ecx, 0x5c4d40
004ba990 e81b66feff               call       0x4a0fb0
004ba995 b801000000               mov        eax, 1
004ba99a e988030000               jmp        0x4bad27
004ba99f 8b159c065c00             mov        edx, dword ptr [0x5c069c]
004ba9a5 8b02                     mov        eax, dword ptr [edx]
004ba9a7 8b0d9c065c00             mov        ecx, dword ptr [0x5c069c]
004ba9ad 8b5004                   mov        edx, dword ptr [eax + 4]
004ba9b0 ffd2                     call       edx
004ba9b2 90                       nop        
004ba9b3 833da0065c0000           cmp        dword ptr [0x5c06a0], 0
004ba9ba 743f                     je         0x4ba9fb
004ba9bc 8b0da0065c00             mov        ecx, dword ptr [0x5c06a0]
004ba9c2 e849cffbff               call       0x477910
004ba9c7 8b0d9c065c00             mov        ecx, dword ptr [0x5c069c]
004ba9cd e80ecffbff               call       0x4778e0
004ba9d2 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
004ba9d5 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004ba9db 81c900100000             or         ecx, 0x1000
004ba9e1 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
004ba9e4 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004ba9ea 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004ba9f0 e8fb2e0000               call       0x4bd8f0
004ba9f5 90                       nop        
004ba9f6 e92a030000               jmp        0x4bad25
004ba9fb 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
004ba9fe 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004baa04 81e1ffefffff             and        ecx, 0xffffefff
004baa0a 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
004baa0d 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004baa13 6a00                     push       0
004baa15 e866e4fbff               call       0x478e80
004baa1a 83c404                   add        esp, 4
004baa1d 8bc8                     mov        ecx, eax
004baa1f e8ac90fcff               call       0x483ad0
004baa24 6a00                     push       0
004baa26 e8055efaff               call       0x460830
004baa2b 83c404                   add        esp, 4
004baa2e 8bc8                     mov        ecx, eax
004baa30 e81b0a0400               call       0x4fb450
004baa35 6a00                     push       0
004baa37 e894e5fdff               call       0x498fd0
004baa3c 83c404                   add        esp, 4
004baa3f 8bc8                     mov        ecx, eax
004baa41 e8da170000               call       0x4bc220
004baa46 6a00                     push       0
004baa48 e813d6fbff               call       0x478060
004baa4d 83c404                   add        esp, 4
004baa50 8bc8                     mov        ecx, eax
004baa52 e899d6feff               call       0x4a80f0
004baa57 6a00                     push       0
004baa59 e882e4fbff               call       0x478ee0
004baa5e 83c404                   add        esp, 4
004baa61 8bc8                     mov        ecx, eax
004baa63 e8181a0000               call       0x4bc480
004baa68 e8d3e4fdff               call       0x498f40
004baa6d 898564ffffff             mov        dword ptr [ebp - 0x9c], eax
004baa73 6a00                     push       0
004baa75 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
004baa7b e800310000               call       0x4bdb80
004baa80 e8bbe4fdff               call       0x498f40
004baa85 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
004baa8b 6a00                     push       0
004baa8d 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
004baa93 e898ddffff               call       0x4b8830
004baa98 90                       nop        
004baa99 833dfc605c0000           cmp        dword ptr [0x5c60fc], 0
004baaa0 7413                     je         0x4baab5
004baaa2 a1fc605c00               mov        eax, dword ptr [0x5c60fc]
004baaa7 8b10                     mov        edx, dword ptr [eax]
004baaa9 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004baaaf 8b4204                   mov        eax, dword ptr [edx + 4]
004baab2 ffd0                     call       eax
004baab4 90                       nop        
004baab5 8d4da8                   lea        ecx, [ebp - 0x58]
004baab8 e87310fcff               call       0x47bb30
004baabd 90                       nop        
004baabe c7855cffffffc80b5700     mov        dword ptr [ebp - 0xa4], 0x570bc8
004baac8 6a00                     push       0
004baaca e891d5fbff               call       0x478060
004baacf 83c404                   add        esp, 4
004baad2 898558ffffff             mov        dword ptr [ebp - 0xa8], eax
004baad8 6a00                     push       0
004baada 8d4da8                   lea        ecx, [ebp - 0x58]
004baadd 51                       push       ecx
004baade 8b955cffffff             mov        edx, dword ptr [ebp - 0xa4]
004baae4 52                       push       edx
004baae5 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
004baaeb e830defeff               call       0x4a8920
004baaf0 90                       nop        
004baaf1 e86a2a0000               call       0x4bd560
004baaf6 89459c                   mov        dword ptr [ebp - 0x64], eax
004baaf9 8b459c                   mov        eax, dword ptr [ebp - 0x64]
004baafc 8b10                     mov        edx, dword ptr [eax]
004baafe 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
004bab01 8b4204                   mov        eax, dword ptr [edx + 4]
004bab04 ffd0                     call       eax
004bab06 e8852a0000               call       0x4bd590
004bab0b 894598                   mov        dword ptr [ebp - 0x68], eax
004bab0e 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004bab11 8b11                     mov        edx, dword ptr [ecx]
004bab13 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004bab16 8b4204                   mov        eax, dword ptr [edx + 4]
004bab19 ffd0                     call       eax
004bab1b 90                       nop        
004bab1c c745a000000000           mov        dword ptr [ebp - 0x60], 0
004bab23 eb09                     jmp        0x4bab2e
004bab25 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004bab28 83c101                   add        ecx, 1
004bab2b 894da0                   mov        dword ptr [ebp - 0x60], ecx
004bab2e 837da001                 cmp        dword ptr [ebp - 0x60], 1
004bab32 0f8d2e010000             jge        0x4bac66
004bab38 6a00                     push       0
004bab3a e881e3fbff               call       0x478ec0
004bab3f 83c404                   add        esp, 4
004bab42 894594                   mov        dword ptr [ebp - 0x6c], eax
004bab45 8b5594                   mov        edx, dword ptr [ebp - 0x6c]
004bab48 8b02                     mov        eax, dword ptr [edx]
004bab4a 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004bab4d 8b5004                   mov        edx, dword ptr [eax + 4]
004bab50 ffd2                     call       edx
004bab52 6a00                     push       0
004bab54 e8d75cfaff               call       0x460830
004bab59 83c404                   add        esp, 4
004bab5c 894590                   mov        dword ptr [ebp - 0x70], eax
004bab5f 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004bab62 8b10                     mov        edx, dword ptr [eax]
004bab64 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004bab67 8b4204                   mov        eax, dword ptr [edx + 4]
004bab6a ffd0                     call       eax
004bab6c 6a00                     push       0
004bab6e e80de3fbff               call       0x478e80
004bab73 83c404                   add        esp, 4
004bab76 89458c                   mov        dword ptr [ebp - 0x74], eax
004bab79 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004bab7c 8b11                     mov        edx, dword ptr [ecx]
004bab7e 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004bab81 8b4204                   mov        eax, dword ptr [edx + 4]
004bab84 ffd0                     call       eax
004bab86 6a00                     push       0
004bab88 e8d3d4fbff               call       0x478060
004bab8d 83c404                   add        esp, 4
004bab90 894588                   mov        dword ptr [ebp - 0x78], eax
004bab93 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004bab96 8b11                     mov        edx, dword ptr [ecx]
004bab98 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004bab9b 8b4204                   mov        eax, dword ptr [edx + 4]
004bab9e ffd0                     call       eax
004baba0 6a00                     push       0
004baba2 e829e4fdff               call       0x498fd0
004baba7 83c404                   add        esp, 4
004babaa 894584                   mov        dword ptr [ebp - 0x7c], eax
004babad 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004babb0 8b11                     mov        edx, dword ptr [ecx]
004babb2 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004babb5 8b4204                   mov        eax, dword ptr [edx + 4]
004babb8 ffd0                     call       eax
004babba 6a00                     push       0
004babbc e81fe3fbff               call       0x478ee0
004babc1 83c404                   add        esp, 4
004babc4 894580                   mov        dword ptr [ebp - 0x80], eax
004babc7 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004babca 8b11                     mov        edx, dword ptr [ecx]
004babcc 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
004babcf 8b4204                   mov        eax, dword ptr [edx + 4]
004babd2 ffd0                     call       eax
004babd4 6a00                     push       0
004babd6 e865d4fbff               call       0x478040
004babdb 83c404                   add        esp, 4
004babde 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004babe4 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004babea 8b11                     mov        edx, dword ptr [ecx]
004babec 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004babf2 8b4204                   mov        eax, dword ptr [edx + 4]
004babf5 ffd0                     call       eax
004babf7 6a00                     push       0
004babf9 e8a2290000               call       0x4bd5a0
004babfe 83c404                   add        esp, 4
004bac01 898578ffffff             mov        dword ptr [ebp - 0x88], eax
004bac07 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004bac0d 8b11                     mov        edx, dword ptr [ecx]
004bac0f 8b8d78ffffff             mov        ecx, dword ptr [ebp - 0x88]
004bac15 8b4204                   mov        eax, dword ptr [edx + 4]
004bac18 ffd0                     call       eax
004bac1a 6a00                     push       0
004bac1c e87fe2fbff               call       0x478ea0
004bac21 83c404                   add        esp, 4
004bac24 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004bac2a 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004bac30 8b11                     mov        edx, dword ptr [ecx]
004bac32 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004bac38 8b4204                   mov        eax, dword ptr [edx + 4]
004bac3b ffd0                     call       eax
004bac3d 6a00                     push       0
004bac3f e8dcc8f7ff               call       0x437520
004bac44 83c404                   add        esp, 4
004bac47 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004bac4d 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004bac53 8b11                     mov        edx, dword ptr [ecx]
004bac55 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004bac5b 8b4204                   mov        eax, dword ptr [edx + 4]
004bac5e ffd0                     call       eax
004bac60 90                       nop        
004bac61 e9bffeffff               jmp        0x4bab25
004bac66 b968a55b00               mov        ecx, 0x5ba568
004bac6b e8e0dbfcff               call       0x488850
004bac70 0fb6c8                   movzx      ecx, al
004bac73 85c9                     test       ecx, ecx
004bac75 752e                     jne        0x4baca5
004bac77 8b15d4a55b00             mov        edx, dword ptr [0x5ba5d4]
004bac7d c1ea05                   shr        edx, 5
004bac80 83e201                   and        edx, 1
004bac83 7520                     jne        0x4baca5
004bac85 b804000000               mov        eax, 4
004bac8a 6bc800                   imul       ecx, eax, 0
004bac8d 8b1510615c00             mov        edx, dword ptr [0x5c6110]
004bac93 8b440a58                 mov        eax, dword ptr [edx + ecx + 0x58]
004bac97 50                       push       eax
004bac98 6a00                     push       0
004bac9a b9404d5c00               mov        ecx, 0x5c4d40
004bac9f e8acee0100               call       0x4d9b50
004baca4 90                       nop        
004baca5 6a00                     push       0
004baca7 b968a55b00               mov        ecx, 0x5ba568
004bacac e86f350000               call       0x4be220
004bacb1 6a00                     push       0
004bacb3 b968a55b00               mov        ecx, 0x5ba568
004bacb8 e8a3300000               call       0x4bdd60
004bacbd 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004bacc3 81c148a20100             add        ecx, 0x1a248
004bacc9 e872e3fbff               call       0x479040
004bacce 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004bacd4 e8f758feff               call       0x4a05d0
004bacd9 b9044f5c00               mov        ecx, 0x5c4f04
004bacde e85de3fbff               call       0x479040
004bace3 6a00                     push       0
004bace5 e84695faff               call       0x464230
004bacea 83c404                   add        esp, 4
004baced 89856cffffff             mov        dword ptr [ebp - 0x94], eax
004bacf3 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004bacf9 8b11                     mov        edx, dword ptr [ecx]
004bacfb 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004bad01 8b4204                   mov        eax, dword ptr [edx + 4]
004bad04 ffd0                     call       eax
004bad06 e8550c0600               call       0x51b960
004bad0b 898568ffffff             mov        dword ptr [ebp - 0x98], eax
004bad11 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004bad17 8b11                     mov        edx, dword ptr [ecx]
004bad19 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
004bad1f 8b4204                   mov        eax, dword ptr [edx + 4]
004bad22 ffd0                     call       eax
004bad24 90                       nop        
004bad25 33c0                     xor        eax, eax
004bad27 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bad2a 33cd                     xor        ecx, ebp
004bad2c e8bd7c0800               call       0x5429ee
004bad31 8be5                     mov        esp, ebp
004bad33 5d                       pop        ebp
004bad34 c3                       ret        
004bad35 cc                       int3       
004bad36 cc                       int3       
004bad37 cc                       int3       
004bad38 cc                       int3       
004bad39 cc                       int3       
004bad3a cc                       int3       
004bad3b cc                       int3       
004bad3c cc                       int3       
004bad3d cc                       int3       
004bad3e cc                       int3       
004bad3f cc                       int3       
