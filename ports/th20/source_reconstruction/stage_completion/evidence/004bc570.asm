004bc570 55                       push       ebp
004bc571 8bec                     mov        ebp, esp
004bc573 81ecac000000             sub        esp, 0xac
004bc579 56                       push       esi
004bc57a 894dec                   mov        dword ptr [ebp - 0x14], ecx
004bc57d 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bc583 e8a8c2fcff               call       0x488830
004bc588 85c0                     test       eax, eax
004bc58a 7444                     je         0x4bc5d0
004bc58c b968a55b00               mov        ecx, 0x5ba568
004bc591 e8ca87fbff               call       0x474d60
004bc596 83f807                   cmp        eax, 7
004bc599 7423                     je         0x4bc5be
004bc59b b968a55b00               mov        ecx, 0x5ba568
004bc5a0 e8bb87fbff               call       0x474d60
004bc5a5 83c001                   add        eax, 1
004bc5a8 8945e8                   mov        dword ptr [ebp - 0x18], eax
004bc5ab 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004bc5ae 50                       push       eax
004bc5af 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004bc5b5 e8b6c1fcff               call       0x488770
004bc5ba 85c0                     test       eax, eax
004bc5bc 7512                     jne        0x4bc5d0
004bc5be 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
004bc5c4 e867990200               call       0x4e5f30
004bc5c9 33c0                     xor        eax, eax
004bc5cb e9c7060000               jmp        0x4bcc97
004bc5d0 6a00                     push       0
004bc5d2 e85942faff               call       0x460830
004bc5d7 83c404                   add        esp, 4
004bc5da 8bc8                     mov        ecx, eax
004bc5dc e8df0a0000               call       0x4bd0c0
004bc5e1 90                       nop        
004bc5e2 b968a55b00               mov        ecx, 0x5ba568
004bc5e7 e834120000               call       0x4bd820
004bc5ec 0fb6c8                   movzx      ecx, al
004bc5ef 85c9                     test       ecx, ecx
004bc5f1 0f8415010000             je         0x4bc70c
004bc5f7 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bc5fd e82ec2fcff               call       0x488830
004bc602 85c0                     test       eax, eax
004bc604 7411                     je         0x4bc617
004bc606 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
004bc60c e81f990200               call       0x4e5f30
004bc611 90                       nop        
004bc612 e9f0000000               jmp        0x4bc707
004bc617 b968a55b00               mov        ecx, 0x5ba568
004bc61c e82fc2fcff               call       0x488850
004bc621 0fb6d0                   movzx      edx, al
004bc624 85d2                     test       edx, edx
004bc626 7467                     je         0x4bc68f
004bc628 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc62e e81d360500               call       0x50fc50
004bc633 05080b0000               add        eax, 0xb08
004bc638 8945e0                   mov        dword ptr [ebp - 0x20], eax
004bc63b b968a55b00               mov        ecx, 0x5ba568
004bc640 e81bcefdff               call       0x499460
004bc645 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004bc648 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004bc64b 50                       push       eax
004bc64c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004bc64f e8bca7fcff               call       0x486e10
004bc654 8945dc                   mov        dword ptr [ebp - 0x24], eax
004bc657 b968a55b00               mov        ecx, 0x5ba568
004bc65c e82fbcffff               call       0x4b8290
004bc661 6a00                     push       0
004bc663 6a0a                     push       0xa
004bc665 52                       push       edx
004bc666 50                       push       eax
004bc667 e8946b0800               call       0x543200
004bc66c 898554ffffff             mov        dword ptr [ebp - 0xac], eax
004bc672 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
004bc678 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
004bc67e 51                       push       ecx
004bc67f 8b9554ffffff             mov        edx, dword ptr [ebp - 0xac]
004bc685 52                       push       edx
004bc686 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004bc689 e8421c0000               call       0x4be2d0
004bc68e 90                       nop        
004bc68f 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bc695 e896c1fcff               call       0x488830
004bc69a 85c0                     test       eax, eax
004bc69c 755d                     jne        0x4bc6fb
004bc69e b968a55b00               mov        ecx, 0x5ba568
004bc6a3 e8a8c1fcff               call       0x488850
004bc6a8 0fb6c0                   movzx      eax, al
004bc6ab 85c0                     test       eax, eax
004bc6ad 754c                     jne        0x4bc6fb
004bc6af 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc6b5 e896350500               call       0x50fc50
004bc6ba 05f8760000               add        eax, 0x76f8
004bc6bf 8945d8                   mov        dword ptr [ebp - 0x28], eax
004bc6c2 b968a55b00               mov        ecx, 0x5ba568
004bc6c7 e8d479faff               call       0x4640a0
004bc6cc 69c890000000             imul       ecx, eax, 0x90
004bc6d2 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004bc6d5 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004bc6d8 0355d4                   add        edx, dword ptr [ebp - 0x2c]
004bc6db 8955d0                   mov        dword ptr [ebp - 0x30], edx
004bc6de b968a55b00               mov        ecx, 0x5ba568
004bc6e3 e87886fbff               call       0x474d60
004bc6e8 83e801                   sub        eax, 1
004bc6eb c1e004                   shl        eax, 4
004bc6ee 8945cc                   mov        dword ptr [ebp - 0x34], eax
004bc6f1 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
004bc6f4 0345cc                   add        eax, dword ptr [ebp - 0x34]
004bc6f7 c6400801                 mov        byte ptr [eax + 8], 1
004bc6fb 8b0dbc605c00             mov        ecx, dword ptr [0x5c60bc]
004bc701 e8da920200               call       0x4e59e0
004bc706 90                       nop        
004bc707 e989050000               jmp        0x4bcc95
004bc70c b968a55b00               mov        ecx, 0x5ba568
004bc711 e84a86fbff               call       0x474d60
004bc716 83f806                   cmp        eax, 6
004bc719 0f8592020000             jne        0x4bc9b1
004bc71f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bc722 e859120000               call       0x4bd980
004bc727 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004bc72d e89e140000               call       0x4bdbd0
004bc732 b968a55b00               mov        ecx, 0x5ba568
004bc737 e8d4060000               call       0x4bce10
004bc73c 90                       nop        
004bc73d b968a55b00               mov        ecx, 0x5ba568
004bc742 e859b8ffff               call       0x4b7fa0
004bc747 85c0                     test       eax, eax
004bc749 7516                     jne        0x4bc761
004bc74b b968a55b00               mov        ecx, 0x5ba568
004bc750 e84b79faff               call       0x4640a0
004bc755 83c022                   add        eax, 0x22
004bc758 50                       push       eax
004bc759 e8a2310700               call       0x52f900
004bc75e 83c404                   add        esp, 4
004bc761 e8dac7fdff               call       0x498f40
004bc766 8bc8                     mov        ecx, eax
004bc768 e813cefdff               call       0x499580
004bc76d 85c0                     test       eax, eax
004bc76f 7523                     jne        0x4bc794
004bc771 6a26                     push       0x26
004bc773 e888310700               call       0x52f900
004bc778 83c404                   add        esp, 4
004bc77b b968a55b00               mov        ecx, 0x5ba568
004bc780 e81b79faff               call       0x4640a0
004bc785 83f803                   cmp        eax, 3
004bc788 750a                     jne        0x4bc794
004bc78a 6a27                     push       0x27
004bc78c e86f310700               call       0x52f900
004bc791 83c404                   add        esp, 4
004bc794 6a00                     push       0
004bc796 e8e578faff               call       0x464080
004bc79b 83c404                   add        esp, 4
004bc79e 8bc8                     mov        ecx, eax
004bc7a0 e80b03f6ff               call       0x41cab0
004bc7a5 8945c8                   mov        dword ptr [ebp - 0x38], eax
004bc7a8 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004bc7ab 51                       push       ecx
004bc7ac 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc7b2 e8590e0000               call       0x4bd610
004bc7b7 83f809                   cmp        eax, 9
004bc7ba 7d4a                     jge        0x4bc806
004bc7bc 6a00                     push       0
004bc7be e8bd78faff               call       0x464080
004bc7c3 83c404                   add        esp, 4
004bc7c6 8bc8                     mov        ecx, eax
004bc7c8 e8e302f6ff               call       0x41cab0
004bc7cd 83c001                   add        eax, 1
004bc7d0 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004bc7d3 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
004bc7d6 52                       push       edx
004bc7d7 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc7dd e8de150000               call       0x4bddc0
004bc7e2 6a00                     push       0
004bc7e4 e89778faff               call       0x464080
004bc7e9 83c404                   add        esp, 4
004bc7ec 8bc8                     mov        ecx, eax
004bc7ee e8bd02f6ff               call       0x41cab0
004bc7f3 8945c0                   mov        dword ptr [ebp - 0x40], eax
004bc7f6 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
004bc7f9 50                       push       eax
004bc7fa 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc800 e85b070000               call       0x4bcf60
004bc805 90                       nop        
004bc806 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bc80c e81fc0fcff               call       0x488830
004bc811 85c0                     test       eax, eax
004bc813 0f8593010000             jne        0x4bc9ac
004bc819 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc81f e82c340500               call       0x50fc50
004bc824 05b8760000               add        eax, 0x76b8
004bc829 8945b8                   mov        dword ptr [ebp - 0x48], eax
004bc82c b968a55b00               mov        ecx, 0x5ba568
004bc831 e86a78faff               call       0x4640a0
004bc836 8945bc                   mov        dword ptr [ebp - 0x44], eax
004bc839 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004bc83c 51                       push       ecx
004bc83d 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004bc840 e83b7df5ff               call       0x414580
004bc845 81389f860100             cmp        dword ptr [eax], 0x1869f
004bc84b 7d3c                     jge        0x4bc889
004bc84d 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc853 e8f8330500               call       0x50fc50
004bc858 05b8760000               add        eax, 0x76b8
004bc85d 8945b0                   mov        dword ptr [ebp - 0x50], eax
004bc860 b968a55b00               mov        ecx, 0x5ba568
004bc865 e83678faff               call       0x4640a0
004bc86a 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004bc86d 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
004bc870 52                       push       edx
004bc871 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
004bc874 e8077df5ff               call       0x414580
004bc879 8945fc                   mov        dword ptr [ebp - 4], eax
004bc87c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bc87f 8b08                     mov        ecx, dword ptr [eax]
004bc881 83c101                   add        ecx, 1
004bc884 8b55fc                   mov        edx, dword ptr [ebp - 4]
004bc887 890a                     mov        dword ptr [edx], ecx
004bc889 b968a55b00               mov        ecx, 0x5ba568
004bc88e e80db7ffff               call       0x4b7fa0
004bc893 85c0                     test       eax, eax
004bc895 0f8511010000             jne        0x4bc9ac
004bc89b 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc8a1 e8aa330500               call       0x50fc50
004bc8a6 05d4760000               add        eax, 0x76d4
004bc8ab 8945a8                   mov        dword ptr [ebp - 0x58], eax
004bc8ae b968a55b00               mov        ecx, 0x5ba568
004bc8b3 e8e877faff               call       0x4640a0
004bc8b8 8945ac                   mov        dword ptr [ebp - 0x54], eax
004bc8bb 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
004bc8be 50                       push       eax
004bc8bf 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004bc8c2 e8b97cf5ff               call       0x414580
004bc8c7 81389f860100             cmp        dword ptr [eax], 0x1869f
004bc8cd 7d3c                     jge        0x4bc90b
004bc8cf 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc8d5 e876330500               call       0x50fc50
004bc8da 05d4760000               add        eax, 0x76d4
004bc8df 8945a0                   mov        dword ptr [ebp - 0x60], eax
004bc8e2 b968a55b00               mov        ecx, 0x5ba568
004bc8e7 e8b477faff               call       0x4640a0
004bc8ec 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004bc8ef 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004bc8f2 51                       push       ecx
004bc8f3 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
004bc8f6 e8857cf5ff               call       0x414580
004bc8fb 8945f8                   mov        dword ptr [ebp - 8], eax
004bc8fe 8b55f8                   mov        edx, dword ptr [ebp - 8]
004bc901 8b02                     mov        eax, dword ptr [edx]
004bc903 83c001                   add        eax, 1
004bc906 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bc909 8901                     mov        dword ptr [ecx], eax
004bc90b b968a55b00               mov        ecx, 0x5ba568
004bc910 e88b77faff               call       0x4640a0
004bc915 83f801                   cmp        eax, 1
004bc918 0f8c8e000000             jl         0x4bc9ac
004bc91e 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc924 e827330500               call       0x50fc50
004bc929 c780f076000001000000     mov        dword ptr [eax + 0x76f0], 1
004bc933 6a00                     push       0
004bc935 e84677faff               call       0x464080
004bc93a 83c404                   add        esp, 4
004bc93d 8bc8                     mov        ecx, eax
004bc93f e8ec5df5ff               call       0x412730
004bc944 8bf0                     mov        esi, eax
004bc946 6a00                     push       0
004bc948 e83377faff               call       0x464080
004bc94d 83c404                   add        esp, 4
004bc950 8bc8                     mov        ecx, eax
004bc952 e85901f6ff               call       0x41cab0
004bc957 8d54f009                 lea        edx, [eax + esi*8 + 9]
004bc95b 89559c                   mov        dword ptr [ebp - 0x64], edx
004bc95e 8b459c                   mov        eax, dword ptr [ebp - 0x64]
004bc961 50                       push       eax
004bc962 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc968 e8130e0000               call       0x4bd780
004bc96d 85c0                     test       eax, eax
004bc96f 753b                     jne        0x4bc9ac
004bc971 6a00                     push       0
004bc973 e80877faff               call       0x464080
004bc978 83c404                   add        esp, 4
004bc97b 8bc8                     mov        ecx, eax
004bc97d e8ae5df5ff               call       0x412730
004bc982 8bf0                     mov        esi, eax
004bc984 6a00                     push       0
004bc986 e8f576faff               call       0x464080
004bc98b 83c404                   add        esp, 4
004bc98e 8bc8                     mov        ecx, eax
004bc990 e81b01f6ff               call       0x41cab0
004bc995 8d4cf009                 lea        ecx, [eax + esi*8 + 9]
004bc999 894d98                   mov        dword ptr [ebp - 0x68], ecx
004bc99c 8b5598                   mov        edx, dword ptr [ebp - 0x68]
004bc99f 52                       push       edx
004bc9a0 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bc9a6 e815140000               call       0x4bddc0
004bc9ab 90                       nop        
004bc9ac e9e4020000               jmp        0x4bcc95
004bc9b1 b968a55b00               mov        ecx, 0x5ba568
004bc9b6 e8a583fbff               call       0x474d60
004bc9bb 83f807                   cmp        eax, 7
004bc9be 0f852a020000             jne        0x4bcbee
004bc9c4 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004bc9ca e801120000               call       0x4bdbd0
004bc9cf 90                       nop        
004bc9d0 833dfcfc5a0000           cmp        dword ptr [0x5afcfc], 0
004bc9d7 7c1d                     jl         0x4bc9f6
004bc9d9 6a04                     push       4
004bc9db b9404d5c00               mov        ecx, 0x5c4d40
004bc9e0 e8cb45feff               call       0x4a0fb0
004bc9e5 c705fcfc5a00ffffffff     mov        dword ptr [0x5afcfc], 0xffffffff
004bc9ef 33c0                     xor        eax, eax
004bc9f1 e9a1020000               jmp        0x4bcc97
004bc9f6 b968a55b00               mov        ecx, 0x5ba568
004bc9fb e870b5ffff               call       0x4b7f70
004bca00 8bf0                     mov        esi, eax
004bca02 6a00                     push       0
004bca04 e87776faff               call       0x464080
004bca09 83c404                   add        esp, 4
004bca0c 8bc8                     mov        ecx, eax
004bca0e e89d00f6ff               call       0x41cab0
004bca13 8d44f012                 lea        eax, [eax + esi*8 + 0x12]
004bca17 50                       push       eax
004bca18 e8e32e0700               call       0x52f900
004bca1d 83c404                   add        esp, 4
004bca20 6a00                     push       0
004bca22 e85976faff               call       0x464080
004bca27 83c404                   add        esp, 4
004bca2a 8bc8                     mov        ecx, eax
004bca2c e87f00f6ff               call       0x41cab0
004bca31 894594                   mov        dword ptr [ebp - 0x6c], eax
004bca34 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004bca37 51                       push       ecx
004bca38 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bca3e e8cd0b0000               call       0x4bd610
004bca43 83f809                   cmp        eax, 9
004bca46 7d4a                     jge        0x4bca92
004bca48 6a00                     push       0
004bca4a e83176faff               call       0x464080
004bca4f 83c404                   add        esp, 4
004bca52 8bc8                     mov        ecx, eax
004bca54 e85700f6ff               call       0x41cab0
004bca59 83c001                   add        eax, 1
004bca5c 894590                   mov        dword ptr [ebp - 0x70], eax
004bca5f 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004bca62 52                       push       edx
004bca63 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bca69 e852130000               call       0x4bddc0
004bca6e 6a00                     push       0
004bca70 e80b76faff               call       0x464080
004bca75 83c404                   add        esp, 4
004bca78 8bc8                     mov        ecx, eax
004bca7a e83100f6ff               call       0x41cab0
004bca7f 89458c                   mov        dword ptr [ebp - 0x74], eax
004bca82 8b458c                   mov        eax, dword ptr [ebp - 0x74]
004bca85 50                       push       eax
004bca86 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bca8c e8cf040000               call       0x4bcf60
004bca91 90                       nop        
004bca92 e8a9c4fdff               call       0x498f40
004bca97 8bc8                     mov        ecx, eax
004bca99 e8e2cafdff               call       0x499580
004bca9e 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bcaa4 e887bdfcff               call       0x488830
004bcaa9 85c0                     test       eax, eax
004bcaab 0f8525010000             jne        0x4bcbd6
004bcab1 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bcab7 e894310500               call       0x50fc50
004bcabc 05b8760000               add        eax, 0x76b8
004bcac1 894584                   mov        dword ptr [ebp - 0x7c], eax
004bcac4 b968a55b00               mov        ecx, 0x5ba568
004bcac9 e8d275faff               call       0x4640a0
004bcace 894588                   mov        dword ptr [ebp - 0x78], eax
004bcad1 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004bcad4 51                       push       ecx
004bcad5 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
004bcad8 e8a37af5ff               call       0x414580
004bcadd 81389f860100             cmp        dword ptr [eax], 0x1869f
004bcae3 7d42                     jge        0x4bcb27
004bcae5 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bcaeb e860310500               call       0x50fc50
004bcaf0 05b8760000               add        eax, 0x76b8
004bcaf5 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004bcafb b968a55b00               mov        ecx, 0x5ba568
004bcb00 e89b75faff               call       0x4640a0
004bcb05 894580                   mov        dword ptr [ebp - 0x80], eax
004bcb08 8b5580                   mov        edx, dword ptr [ebp - 0x80]
004bcb0b 52                       push       edx
004bcb0c 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004bcb12 e8697af5ff               call       0x414580
004bcb17 8945f4                   mov        dword ptr [ebp - 0xc], eax
004bcb1a 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004bcb1d 8b08                     mov        ecx, dword ptr [eax]
004bcb1f 83c101                   add        ecx, 1
004bcb22 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004bcb25 890a                     mov        dword ptr [edx], ecx
004bcb27 b968a55b00               mov        ecx, 0x5ba568
004bcb2c e86fb4ffff               call       0x4b7fa0
004bcb31 85c0                     test       eax, eax
004bcb33 0f859d000000             jne        0x4bcbd6
004bcb39 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bcb3f e80c310500               call       0x50fc50
004bcb44 05d4760000               add        eax, 0x76d4
004bcb49 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
004bcb4f b968a55b00               mov        ecx, 0x5ba568
004bcb54 e84775faff               call       0x4640a0
004bcb59 898578ffffff             mov        dword ptr [ebp - 0x88], eax
004bcb5f 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
004bcb65 50                       push       eax
004bcb66 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
004bcb6c e80f7af5ff               call       0x414580
004bcb71 81389f860100             cmp        dword ptr [eax], 0x1869f
004bcb77 7d48                     jge        0x4bcbc1
004bcb79 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bcb7f e8cc300500               call       0x50fc50
004bcb84 05d4760000               add        eax, 0x76d4
004bcb89 89856cffffff             mov        dword ptr [ebp - 0x94], eax
004bcb8f b968a55b00               mov        ecx, 0x5ba568
004bcb94 e80775faff               call       0x4640a0
004bcb99 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004bcb9f 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004bcba5 51                       push       ecx
004bcba6 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004bcbac e8cf79f5ff               call       0x414580
004bcbb1 8945f0                   mov        dword ptr [ebp - 0x10], eax
004bcbb4 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004bcbb7 8b02                     mov        eax, dword ptr [edx]
004bcbb9 83c001                   add        eax, 1
004bcbbc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004bcbbf 8901                     mov        dword ptr [ecx], eax
004bcbc1 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bcbc7 e884300500               call       0x50fc50
004bcbcc c780f476000001000000     mov        dword ptr [eax + 0x76f4], 1
004bcbd6 b968a55b00               mov        ecx, 0x5ba568
004bcbdb e830020000               call       0x4bce10
004bcbe0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004bcbe3 e8980d0000               call       0x4bd980
004bcbe8 90                       nop        
004bcbe9 e9a7000000               jmp        0x4bcc95
004bcbee 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bcbf4 e837bcfcff               call       0x488830
004bcbf9 85c0                     test       eax, eax
004bcbfb 7573                     jne        0x4bcc70
004bcbfd 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004bcc03 e828bcfcff               call       0x488830
004bcc08 85c0                     test       eax, eax
004bcc0a 7564                     jne        0x4bcc70
004bcc0c 8b0d08615c00             mov        ecx, dword ptr [0x5c6108]
004bcc12 e839300500               call       0x50fc50
004bcc17 05f8760000               add        eax, 0x76f8
004bcc1c 898568ffffff             mov        dword ptr [ebp - 0x98], eax
004bcc22 b968a55b00               mov        ecx, 0x5ba568
004bcc27 e87474faff               call       0x4640a0
004bcc2c 69d090000000             imul       edx, eax, 0x90
004bcc32 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
004bcc38 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
004bcc3e 038564ffffff             add        eax, dword ptr [ebp - 0x9c]
004bcc44 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
004bcc4a b968a55b00               mov        ecx, 0x5ba568
004bcc4f e80c81fbff               call       0x474d60
004bcc54 83e801                   sub        eax, 1
004bcc57 c1e004                   shl        eax, 4
004bcc5a 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
004bcc60 8b8d60ffffff             mov        ecx, dword ptr [ebp - 0xa0]
004bcc66 038d5cffffff             add        ecx, dword ptr [ebp - 0xa4]
004bcc6c c6410801                 mov        byte ptr [ecx + 8], 1
004bcc70 6a0c                     push       0xc
004bcc72 b9404d5c00               mov        ecx, 0x5c4d40
004bcc77 e8d4150000               call       0x4be250
004bcc7c e8bfc2fdff               call       0x498f40
004bcc81 8bc8                     mov        ecx, eax
004bcc83 e8180c0000               call       0x4bd8a0
004bcc88 6a01                     push       1
004bcc8a b968a55b00               mov        ecx, 0x5ba568
004bcc8f e8fc100000               call       0x4bdd90
004bcc94 90                       nop        
004bcc95 33c0                     xor        eax, eax
004bcc97 5e                       pop        esi
004bcc98 8be5                     mov        esp, ebp
004bcc9a 5d                       pop        ebp
004bcc9b c3                       ret        
004bcc9c cc                       int3       
004bcc9d cc                       int3       
004bcc9e cc                       int3       
004bcc9f cc                       int3       
