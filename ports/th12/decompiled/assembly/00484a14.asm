
// block 00484a14
00484a14  mov        edi, edi
00484a16  push       ebp
00484a17  mov        ebp, esp
00484a19  sub        esp, 0x1c
00484a1c  push       ebx
00484a1d  push       esi
00484a1e  push       edi
00484a1f  push       dword ptr [ebp + 8]
00484a22  lea        ecx, [ebp - 0x1c]
00484a25  xor        ebx, ebx
00484a27  call       0x46d3b4

// block 00484a2c
00484a2c  mov        eax, dword ptr [ebp - 0x1c]
00484a2f  mov        edi, dword ptr [eax + 0xd4]
00484a35  lea        esi, [edi + 0x38]
00484a38  mov        dword ptr [ebp - 4], esi
00484a3b  mov        dword ptr [ebp - 8], 0xc

// block 00484a42
00484a42  push       dword ptr [esi + 0x30]
00484a45  call       0x475860

// block 00484a4a
00484a4a  push       dword ptr [esi]
00484a4c  mov        dword ptr [ebp - 0xc], eax
00484a4f  call       0x475860

// block 00484a54
00484a54  mov        esi, dword ptr [ebp - 4]
00484a57  pop        ecx
00484a58  pop        ecx
00484a59  mov        ecx, dword ptr [ebp - 0xc]
00484a5c  add        eax, ebx
00484a5e  add        esi, 4
00484a61  dec        dword ptr [ebp - 8]
00484a64  lea        ebx, [eax + ecx + 2]
00484a68  mov        dword ptr [ebp - 4], esi
00484a6b  jne        0x484a42

// block 00484a6d
00484a6d  lea        eax, [ebx + 1]
00484a70  push       eax
00484a71  call       0x4777ce

// block 00484a76
00484a76  mov        esi, eax
00484a78  pop        ecx
00484a79  mov        dword ptr [ebp - 4], esi
00484a7c  test       esi, esi
00484a7e  je         0x484aff

// block 00484a80
00484a80  add        edi, 0x68
00484a83  mov        dword ptr [ebp - 8], 0xc

// block 00484a8a
00484a8a  mov        eax, dword ptr [ebp - 4]
00484a8d  mov        byte ptr [esi], 0x3a
00484a90  push       dword ptr [edi - 0x30]
00484a93  inc        esi
00484a94  sub        eax, esi
00484a96  lea        eax, [eax + ebx + 1]
00484a9a  push       eax
00484a9b  push       esi
00484a9c  call       0x47a2b9

// block 00484aa1
00484aa1  add        esp, 0xc
00484aa4  test       eax, eax
00484aa6  je         0x484ab7

// block 00484aa8
00484aa8  xor        eax, eax
00484aaa  push       eax
00484aab  push       eax
00484aac  push       eax
00484aad  push       eax
00484aae  push       eax
00484aaf  call       0x470dc6

// block 00484ab4
00484ab4  add        esp, 0x14

// block 00484ab7
00484ab7  push       esi
00484ab8  call       0x475860

// block 00484abd
00484abd  add        esi, eax
00484abf  mov        eax, dword ptr [ebp - 4]
00484ac2  mov        byte ptr [esi], 0x3a
00484ac5  push       dword ptr [edi]
00484ac7  inc        esi
00484ac8  sub        eax, esi
00484aca  lea        eax, [eax + ebx + 1]
00484ace  push       eax
00484acf  push       esi
00484ad0  call       0x47a2b9

// block 00484ad5
00484ad5  add        esp, 0x10
00484ad8  test       eax, eax
00484ada  je         0x484aeb

// block 00484adc
00484adc  xor        eax, eax
00484ade  push       eax
00484adf  push       eax
00484ae0  push       eax
00484ae1  push       eax
00484ae2  push       eax
00484ae3  call       0x470dc6

// block 00484ae8
00484ae8  add        esp, 0x14

// block 00484aeb
00484aeb  push       esi
00484aec  call       0x475860

// block 00484af1
00484af1  add        esi, eax
00484af3  add        edi, 4
00484af6  dec        dword ptr [ebp - 8]
00484af9  pop        ecx
00484afa  jne        0x484a8a

// block 00484afc
00484afc  mov        byte ptr [esi], 0

// block 00484aff
00484aff  cmp        byte ptr [ebp - 0x10], 0
00484b03  pop        edi
00484b04  pop        esi
00484b05  pop        ebx
00484b06  je         0x484b0f

// block 00484b08
00484b08  mov        eax, dword ptr [ebp - 0x14]
00484b0b  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00484b0f
00484b0f  mov        eax, dword ptr [ebp - 4]
00484b12  leave      
00484b13  ret        
