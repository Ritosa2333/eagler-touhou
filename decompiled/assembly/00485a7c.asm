
// block 00485a7c
00485a7c  mov        edi, edi
00485a7e  push       ebp
00485a7f  mov        ebp, esp
00485a81  sub        esp, 0xc
00485a84  mov        eax, dword ptr [0x4ad138]
00485a89  xor        eax, ebp
00485a8b  mov        dword ptr [ebp - 4], eax
00485a8e  push       esi
00485a8f  mov        esi, ecx
00485a91  test       esi, esi
00485a93  je         0x485ae8

// block 00485a95
00485a95  cmp        byte ptr [esi], 0
00485a98  je         0x485ae8

// block 00485a9a
00485a9a  push       0x49f2f4
00485a9f  push       esi
00485aa0  call       0x472ac0

// block 00485aa5
00485aa5  pop        ecx
00485aa6  pop        ecx
00485aa7  test       eax, eax
00485aa9  je         0x485ae8

// block 00485aab
00485aab  push       0x49f2f0
00485ab0  push       esi
00485ab1  call       0x472ac0

// block 00485ab6
00485ab6  pop        ecx
00485ab7  pop        ecx
00485ab8  test       eax, eax
00485aba  jne        0x485ad4

// block 00485abc
00485abc  push       8
00485abe  lea        eax, [ebp - 0xc]
00485ac1  push       eax
00485ac2  push       0xb
00485ac4  push       dword ptr [edi + 0x1c]
00485ac7  call       dword ptr [0x4980e0]

// block 00485acd
00485acd  test       eax, eax
00485acf  je         0x485b00

// block 00485ad1
00485ad1  lea        esi, [ebp - 0xc]

// block 00485ad4
00485ad4  push       esi
00485ad5  call       0x46d114

// block 00485ada
00485ada  pop        ecx

// block 00485adb
00485adb  mov        ecx, dword ptr [ebp - 4]
00485ade  xor        ecx, ebp
00485ae0  pop        esi
00485ae1  call       0x46c932

// block 00485ae6
00485ae6  leave      
00485ae7  ret        

// block 00485ae8
00485ae8  push       8
00485aea  lea        eax, [ebp - 0xc]
00485aed  push       eax
00485aee  push       0x1004
00485af3  push       dword ptr [edi + 0x1c]
00485af6  call       dword ptr [0x4980e0]

// block 00485afc
00485afc  test       eax, eax
00485afe  jne        0x485b04

// block 00485b00
00485b00  xor        eax, eax
00485b02  jmp        0x485adb

// block 00485b04
00485b04  lea        eax, [ebp - 0xc]
00485b07  push       0x49f2ec
00485b0c  push       eax
00485b0d  call       0x472ac0

// block 00485b12
00485b12  pop        ecx
00485b13  pop        ecx
00485b14  test       eax, eax
00485b16  jne        0x485ad1

// block 00485b18
00485b18  call       dword ptr [0x498158]

// block 00485b1e
00485b1e  jmp        0x485adb
