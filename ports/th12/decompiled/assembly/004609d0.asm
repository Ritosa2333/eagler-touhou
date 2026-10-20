
// block 004609d0
004609d0  sub        esp, 0x2c
004609d3  push       esi
004609d4  push       edi
004609d5  mov        edi, eax
004609d7  mov        eax, dword ptr [edi]
004609d9  mov        esi, ecx
004609db  mov        ecx, dword ptr [esi + eax*4 + 0x4b50c0]
004609e2  mov        eax, dword ptr [edi + 4]
004609e5  lea        edx, [eax + eax*4]
004609e8  mov        eax, dword ptr [ecx + 0x120]
004609ee  cmp        dword ptr [eax + edx*4], 0
004609f2  je         0x460b03

// block 004609f8
004609f8  call       0x45a3c0

// block 004609fd
004609fd  mov        eax, dword ptr [0x4ce8f0]
00460a02  mov        ecx, dword ptr [eax]
00460a04  lea        edx, [esp + 8]
00460a08  push       edx
00460a09  push       0
00460a0b  push       0
00460a0d  push       0
00460a0f  push       eax
00460a10  mov        eax, dword ptr [ecx + 0x48]
00460a13  call       eax

// block 00460a15
00460a15  test       eax, eax
00460a17  jne        0x460b03

// block 00460a1d
00460a1d  mov        ecx, dword ptr [edi]
00460a1f  mov        edx, dword ptr [esi + ecx*4 + 0x4b50c0]
00460a26  mov        ecx, dword ptr [edx + 0x120]
00460a2c  mov        eax, dword ptr [edi + 4]
00460a2f  lea        eax, [eax + eax*4]
00460a32  mov        edx, dword ptr [ecx + eax*4]
00460a35  lea        eax, [ecx + eax*4]
00460a38  mov        ecx, dword ptr [edx]
00460a3a  mov        eax, dword ptr [eax]
00460a3c  mov        ecx, dword ptr [ecx + 0x48]
00460a3f  lea        edx, [esp + 0xc]
00460a43  push       edx
00460a44  push       0
00460a46  push       eax
00460a47  call       ecx

// block 00460a49
00460a49  test       eax, eax
00460a4b  je         0x460a5f

// block 00460a4d
00460a4d  mov        eax, dword ptr [esp + 8]
00460a51  mov        edx, dword ptr [eax]
00460a53  push       eax
00460a54  mov        eax, dword ptr [edx + 8]
00460a57  call       eax

// block 00460a59
00460a59  pop        edi
00460a5a  pop        esi
00460a5b  add        esp, 0x2c
00460a5e  ret        

// block 00460a5f
00460a5f  mov        eax, dword ptr [edi + 8]
00460a62  mov        edx, dword ptr [edi + 0x10]
00460a65  mov        ecx, dword ptr [edi + 0xc]
00460a68  add        edx, eax
00460a6a  mov        dword ptr [esp + 0x10], eax
00460a6e  mov        eax, dword ptr [edi + 0x14]
00460a71  add        eax, ecx
00460a73  mov        dword ptr [esp + 0x1c], eax
00460a77  mov        eax, dword ptr [edi + 0x18]
00460a7a  mov        dword ptr [esp + 0x18], edx
00460a7e  mov        edx, dword ptr [edi + 0x20]
00460a81  add        edx, eax
00460a83  push       0
00460a85  mov        dword ptr [esp + 0x18], ecx
00460a89  mov        ecx, dword ptr [edi + 0x1c]
00460a8c  mov        dword ptr [esp + 0x24], eax
00460a90  mov        eax, dword ptr [edi + 0x24]
00460a93  push       2
00460a95  add        eax, ecx
00460a97  mov        dword ptr [esp + 0x2c], ecx
00460a9b  lea        ecx, [esp + 0x18]
00460a9f  push       ecx
00460aa0  mov        ecx, dword ptr [esp + 0x18]
00460aa4  push       0
00460aa6  mov        dword ptr [esp + 0x38], edx
00460aaa  mov        edx, dword ptr [esp + 0x18]
00460aae  push       edx
00460aaf  mov        dword ptr [esp + 0x40], eax
00460ab3  lea        eax, [esp + 0x34]
00460ab7  push       eax
00460ab8  push       0
00460aba  push       ecx
00460abb  call       0x46c71c

// block 00460ac0
00460ac0  test       eax, eax
00460ac2  jne        0x460aeb

// block 00460ac4
00460ac4  mov        eax, dword ptr [edi]
00460ac6  mov        ecx, dword ptr [esi + eax*4 + 0x4b50c0]
00460acd  mov        edi, dword ptr [edi + 4]
00460ad0  mov        eax, dword ptr [ecx + 0x120]
00460ad6  lea        edx, [edi + edi*4]
00460ad9  mov        ecx, dword ptr [eax + edx*4]
00460adc  lea        eax, [eax + edx*4]
00460adf  mov        edx, dword ptr [ecx]
00460ae1  mov        eax, ecx
00460ae3  mov        ecx, dword ptr [edx + 0x54]
00460ae6  push       0
00460ae8  push       eax
00460ae9  call       ecx

// block 00460aeb
00460aeb  mov        eax, dword ptr [esp + 0xc]
00460aef  mov        edx, dword ptr [eax]
00460af1  push       eax
00460af2  mov        eax, dword ptr [edx + 8]
00460af5  call       eax

// block 00460af7
00460af7  mov        eax, dword ptr [esp + 8]
00460afb  mov        ecx, dword ptr [eax]
00460afd  mov        edx, dword ptr [ecx + 8]
00460b00  push       eax
00460b01  call       edx

// block 00460b03
00460b03  pop        edi
00460b04  pop        esi
00460b05  add        esp, 0x2c
00460b08  ret        
