
// block 004227c0
004227c0  push       ebp
004227c1  mov        ebp, esp
004227c3  and        esp, 0xfffffff8
004227c6  mov        eax, dword ptr [0x4b43e4]
004227cb  sub        esp, 0x50
004227ce  test       dword ptr [eax + 0x6d18], 0x200
004227d8  push       ebx
004227d9  push       esi
004227da  je         0x4227e5

// block 004227dc
004227dc  cmp        dword ptr [eax + 0x6d20], 0x78
004227e3  jl         0x422821

// block 004227e5
004227e5  mov        eax, dword ptr [edi + 0x14]
004227e8  xor        ebx, ebx
004227ea  mov        esi, 2
004227ef  cmp        eax, ebx
004227f1  jne        0x422963

// block 004227f7
004227f7  call       0x421430

// block 004227fc
004227fc  test       byte ptr [edi + 0x60], 8
00422800  je         0x42282c

// block 00422802
00422802  mov        esi, 0x4cf0d8
00422807  call       0x464c40

// block 0042280c
0042280c  mov        eax, dword ptr [0x4cee78]
00422811  shr        eax, 0xd
00422814  not        eax
00422816  and        eax, 1
00422819  or         eax, 2
0042281c  mov        dword ptr [0x4cee40], eax

// block 00422821
00422821  mov        eax, 1
00422826  pop        esi
00422827  pop        ebx
00422828  mov        esp, ebp
0042282a  pop        ebp
0042282b  ret        

// block 0042282c
0042282c  call       0x4056e0

// block 00422831
00422831  cmp        dword ptr [0x4b43bc], ebx
00422837  je         0x422868

// block 00422839
00422839  call       0x4057e0

// block 0042283e
0042283e  call       0x405890

// block 00422843
00422843  mov        ecx, dword ptr [0x4b43e4]
00422849  or         dword ptr [edi + 0x60], 0x800
00422850  mov        edx, dword ptr [ecx + 0x6c68]
00422856  push       edx
00422857  mov        ebx, 1
0042285c  call       0x461970

// block 00422861
00422861  xor        ebx, ebx
00422863  jmp        0x422a5f

// block 00422868
00422868  and        dword ptr [edi + 0x60], 0xfffff7ff
0042286f  call       0x409630

// block 00422874
00422874  call       0x436100

// block 00422879
00422879  call       0x40e8b0

// block 0042287e
0042287e  mov        eax, dword ptr [0x4b43dc]
00422883  push       eax
00422884  call       0x40e940

// block 00422889
00422889  call       0x421bf0

// block 0042288e
0042288e  mov        dword ptr [0x4b0cbc], ebx
00422894  mov        dword ptr [0x4b0cc0], ebx
0042289a  call       0x43c590

// block 0042289f
0042289f  push       0x50
004228a1  lea        ecx, [esp + 0xc]
004228a5  push       ebx
004228a6  push       ecx
004228a7  call       0x477420

// block 004228ac
004228ac  add        esp, 0xc
004228af  lea        edx, [esp + 8]
004228b3  push       edx
004228b4  push       0x49fe14
004228b9  call       0x412990

// block 004228be
004228be  call       0x41d560

// block 004228c3
004228c3  call       0x431ea0

// block 004228c8
004228c8  call       0x40ea10

// block 004228cd
004228cd  mov        eax, dword ptr [0x4b43c8]
004228d2  call       0x40e810

// block 004228d7
004228d7  call       0x412ee0

// block 004228dc
004228dc  mov        eax, dword ptr [0x4b44f0]
004228e1  call       0x40e810

// block 004228e6
004228e6  mov        eax, dword ptr [0x4b44f4]
004228eb  call       0x40e810

// block 004228f0
004228f0  mov        ecx, dword ptr [0x4b43d4]
004228f6  mov        eax, dword ptr [ecx + 8]
004228f9  or         dword ptr [eax + 4], esi
004228fc  mov        eax, dword ptr [ecx + 0xc]
004228ff  or         dword ptr [eax + 4], esi
00422902  mov        eax, dword ptr [0x4b43c4]
00422907  call       0x40e810

// block 0042290c
0042290c  mov        ecx, dword ptr [0x4b4524]
00422912  mov        eax, dword ptr [ecx + 8]
00422915  or         dword ptr [eax + 4], esi
00422918  mov        eax, dword ptr [ecx + 0xc]
0042291b  or         dword ptr [eax + 4], esi
0042291e  call       0x40d990

// block 00422923
00422923  call       0x44a0f0

// block 00422928
00422928  test       byte ptr [0x4b0ce0], 0x20
0042292f  jne        0x422940

// block 00422931
00422931  mov        eax, dword ptr [0x4b452c]
00422936  mov        ecx, dword ptr [eax + 0x34]
00422939  push       ecx
0042293a  push       ebx
0042293b  call       0x430150

// block 00422940
00422940  mov        edx, dword ptr [0x4b43b8]
00422946  mov        eax, dword ptr [edx + 0x18fb8]
0042294c  push       eax
0042294d  mov        ebx, 1
00422952  call       0x461970

// block 00422957
00422957  call       0x411b00

// block 0042295c
0042295c  xor        ebx, ebx
0042295e  jmp        0x422a5f

// block 00422963
00422963  cmp        eax, 0x1e
00422966  jne        0x422a5f

// block 0042296c
0042296c  mov        eax, dword ptr [edi + 0x60]
0042296f  test       eax, 0x800
00422974  je         0x422a5f

// block 0042297a
0042297a  and        eax, 0xfffff7ff
0042297f  mov        dword ptr [edi + 0x60], eax
00422982  call       0x409630

// block 00422987
00422987  call       0x436100

// block 0042298c
0042298c  call       0x40e8b0

// block 00422991
00422991  mov        ecx, dword ptr [0x4b43dc]
00422997  push       ecx
00422998  call       0x40e940

// block 0042299d
0042299d  call       0x421bf0

// block 004229a2
004229a2  mov        dword ptr [0x4b0cbc], ebx
004229a8  mov        dword ptr [0x4b0cc0], ebx
004229ae  call       0x43c590

// block 004229b3
004229b3  push       0x50
004229b5  lea        edx, [esp + 0xc]
004229b9  push       ebx
004229ba  push       edx
004229bb  call       0x477420

// block 004229c0
004229c0  add        esp, 0xc
004229c3  lea        eax, [esp + 8]
004229c7  push       eax
004229c8  push       0x49fe14
004229cd  call       0x412990

// block 004229d2
004229d2  call       0x41d560

// block 004229d7
004229d7  call       0x431ea0

// block 004229dc
004229dc  call       0x40ea10

// block 004229e1
004229e1  mov        eax, dword ptr [0x4b43c8]
004229e6  call       0x40e810

// block 004229eb
004229eb  call       0x412ee0

// block 004229f0
004229f0  mov        eax, dword ptr [0x4b44f0]
004229f5  call       0x40e810

// block 004229fa
004229fa  mov        eax, dword ptr [0x4b44f4]
004229ff  call       0x40e810

// block 00422a04
00422a04  mov        ecx, dword ptr [0x4b43d4]
00422a0a  mov        eax, dword ptr [ecx + 8]
00422a0d  or         dword ptr [eax + 4], esi
00422a10  mov        eax, dword ptr [ecx + 0xc]
00422a13  or         dword ptr [eax + 4], esi
00422a16  mov        eax, dword ptr [0x4b43c4]
00422a1b  call       0x40e810

// block 00422a20
00422a20  mov        ecx, dword ptr [0x4b4524]
00422a26  mov        eax, dword ptr [ecx + 8]
00422a29  or         dword ptr [eax + 4], esi
00422a2c  mov        eax, dword ptr [ecx + 0xc]
00422a2f  or         dword ptr [eax + 4], esi
00422a32  call       0x40d990

// block 00422a37
00422a37  call       0x44a0f0

// block 00422a3c
00422a3c  call       0x430240

// block 00422a41
00422a41  mov        ecx, dword ptr [0x4b452c]
00422a47  mov        edx, dword ptr [ecx + 0x34]
00422a4a  push       edx
00422a4b  push       ebx
00422a4c  call       0x430150

// block 00422a51
00422a51  call       0x411b00

// block 00422a56
00422a56  push       ebx
00422a57  lea        eax, [edi + 0x10]
00422a5a  call       0x4067e0

// block 00422a5f
00422a5f  cmp        dword ptr [edi + 0x14], 5
00422a63  jne        0x422a6b

// block 00422a65
00422a65  mov        dword ptr [0x4cf468], esi

// block 00422a6b
00422a6b  mov        esi, dword ptr [0x4b43bc]
00422a71  cmp        esi, ebx
00422a73  je         0x422a91

// block 00422a75
00422a75  test       byte ptr [esi + 0x35bc], 8
00422a7c  je         0x422a91

// block 00422a7e
00422a7e  cmp        esi, ebx
00422a80  je         0x422a91

// block 00422a82
00422a82  push       esi
00422a83  call       0x402cc0

// block 00422a88
00422a88  push       esi
00422a89  call       0x46ca4f

// block 00422a8e
00422a8e  add        esp, 4

// block 00422a91
00422a91  mov        eax, dword ptr [edi + 0x60]
00422a94  test       al, 4
00422a96  je         0x422aab

// block 00422a98
00422a98  or         eax, 0x80
00422a9d  mov        dword ptr [edi + 0x60], eax
00422aa0  mov        eax, 1
00422aa5  pop        esi
00422aa6  pop        ebx
00422aa7  mov        esp, ebp
00422aa9  pop        ebp
00422aaa  ret        

// block 00422aab
00422aab  mov        esi, 0x4cf0d8
00422ab0  call       0x464c40

// block 00422ab5
00422ab5  test       byte ptr [0x4b0ce0], 0x20
00422abc  je         0x422b1c

// block 00422abe
00422abe  test       dword ptr [0x4d48b8], 0x80103
00422ac8  jne        0x422ad0

// block 00422aca
00422aca  test       byte ptr [edi + 0x60], 0x70
00422ace  je         0x422ae9

// block 00422ad0
00422ad0  mov        eax, dword ptr [0x4cee78]
00422ad5  and        eax, 0x2000
00422ada  neg        eax
00422adc  sbb        eax, eax
00422ade  and        eax, 0xfffffffe
00422ae1  add        eax, 4
00422ae4  mov        dword ptr [0x4cee40], eax

// block 00422ae9
00422ae9  mov        eax, dword ptr [edi + 0x14]
00422aec  cmp        eax, 0xdd4
00422af1  jne        0x422b06

// block 00422af3
00422af3  push       0x3b
00422af5  push       0
00422af7  push       0
00422af9  push       0
00422afb  push       0x3c
00422afd  push       5
00422aff  call       0x4529a0

// block 00422b04
00422b04  jmp        0x422b1c

// block 00422b06
00422b06  cmp        eax, 0xe10
00422b0b  jne        0x422b1c

// block 00422b0d
00422b0d  mov        ecx, 4
00422b12  mov        eax, 0x4ce8e8
00422b17  call       0x40f720

// block 00422b1c
00422b1c  call       0x420e70

// block 00422b21
00422b21  mov        eax, dword ptr [edi + 0x60]
00422b24  test       al, 0x10
00422b26  je         0x422b33

// block 00422b28
00422b28  mov        eax, 3
00422b2d  pop        esi
00422b2e  pop        ebx
00422b2f  mov        esp, ebp
00422b31  pop        ebp
00422b32  ret        

// block 00422b33
00422b33  test       al, 0x20
00422b35  jne        0x422b28

// block 00422b37
00422b37  test       al, 0x40
00422b39  jne        0x422b28

// block 00422b3b
00422b3b  mov        ecx, dword ptr [0x4b4518]
00422b41  mov        ebx, 1
00422b46  cmp        dword ptr [ecx + 0x10], ebx
00422b49  je         0x422b7b

// block 00422b4b
00422b4b  mov        edx, dword ptr [0x4b0c94]
00422b51  mov        eax, dword ptr [0x4b0c90]
00422b56  lea        ecx, [edx + eax*2]
00422b59  mov        edx, dword ptr [0x4b451c]
00422b5f  imul       ecx, ecx, 0x45f4
00422b65  cmp        dword ptr [ecx + edx + 0x594], 0xcdfe5ff
00422b70  lea        eax, [ecx + edx + 0x594]
00422b77  jge        0x422b7b

// block 00422b79
00422b79  add        dword ptr [eax], ebx

// block 00422b7b
00422b7b  add        dword ptr [0x4b0cbc], ebx
00422b81  add        dword ptr [0x4b0cc0], ebx
00422b87  add        dword ptr [0x4b0c60], ebx
00422b8d  lea        esi, [edi + 0x10]
00422b90  call       0x464a80

// block 00422b95
00422b95  pop        esi
00422b96  mov        eax, ebx
00422b98  pop        ebx
00422b99  mov        esp, ebp
00422b9b  pop        ebp
00422b9c  ret        
