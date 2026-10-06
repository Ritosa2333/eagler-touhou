
// block 00424a20
00424a20  push       ebp
00424a21  mov        ebp, dword ptr [esp + 8]
00424a25  push       esi
00424a26  push       0x1000
00424a2b  push       0
00424a2d  push       ebp
00424a2e  mov        esi, eax
00424a30  call       0x477420

// block 00424a35
00424a35  add        esp, 0xc
00424a38  cmp        byte ptr [ebp], 0
00424a3c  jne        0x424aeb

// block 00424a42
00424a42  push       edi

// block 00424a43
00424a43  push       0xa
00424a45  push       esi
00424a46  call       0x46d1a0

// block 00424a4b
00424a4b  mov        edi, eax
00424a4d  add        esp, 8
00424a50  test       edi, edi
00424a52  jne        0x424a65

// block 00424a54
00424a54  push       0xd
00424a56  push       esi
00424a57  call       0x46d1a0

// block 00424a5c
00424a5c  mov        edi, eax
00424a5e  add        esp, 8
00424a61  test       edi, edi
00424a63  je         0x424ad7

// block 00424a65
00424a65  mov        eax, edi
00424a67  sub        eax, esi
00424a69  cmp        eax, 0xfff
00424a6e  jl         0x424a75

// block 00424a70
00424a70  mov        eax, 0xfff

// block 00424a75
00424a75  push       eax
00424a76  push       esi
00424a77  push       ebp
00424a78  call       0x47a330

// block 00424a7d
00424a7d  sub        esi, edi
00424a7f  add        esp, 0xc
00424a82  add        dword ptr [ebx], esi
00424a84  mov        esi, edi

// block 00424a86
00424a86  mov        al, byte ptr [esi]
00424a88  cmp        al, 0xa
00424a8a  je         0x424a90

// block 00424a8c
00424a8c  cmp        al, 0xd
00424a8e  jne        0x424a95

// block 00424a90
00424a90  inc        esi
00424a91  dec        dword ptr [ebx]
00424a93  jmp        0x424a86

// block 00424a95
00424a95  push       0x23
00424a97  push       ebp
00424a98  call       0x46d1a0

// block 00424a9d
00424a9d  mov        edi, eax
00424a9f  add        esp, 8
00424aa2  test       edi, edi
00424aa4  je         0x424abe

// block 00424aa6
00424aa6  mov        eax, ebp
00424aa8  sub        eax, edi
00424aaa  add        eax, 0x1000
00424aaf  push       eax
00424ab0  push       0
00424ab2  push       edi
00424ab3  call       0x477420

// block 00424ab8
00424ab8  add        esp, 0xc
00424abb  mov        byte ptr [edi], 0

// block 00424abe
00424abe  mov        eax, ebp
00424ac0  call       0x424b50

// block 00424ac5
00424ac5  cmp        byte ptr [ebp], 0
00424ac9  je         0x424a43

// block 00424acf
00424acf  pop        edi
00424ad0  mov        eax, esi
00424ad2  pop        esi
00424ad3  pop        ebp
00424ad4  ret        4

// block 00424ad7
00424ad7  mov        ecx, dword ptr [ebx]
00424ad9  push       ecx
00424ada  push       esi
00424adb  push       ebp
00424adc  call       0x47a330

// block 00424ae1
00424ae1  add        esp, 0xc
00424ae4  mov        dword ptr [ebx], 0
00424aea  pop        edi

// block 00424aeb
00424aeb  mov        eax, esi
00424aed  pop        esi
00424aee  pop        ebp
00424aef  ret        4
