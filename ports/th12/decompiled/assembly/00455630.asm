
// block 00455630
00455630  push       ebp
00455631  mov        ebp, esp
00455633  and        esp, 0xfffffff8
00455636  sub        esp, 0xd4
0045563c  push       ebx
0045563d  push       esi
0045563e  push       edi
0045563f  mov        edi, dword ptr [ebp + 8]
00455642  cmp        dword ptr [edi + 0x3f0], 0
00455649  je         0x455b44

// block 0045564f
0045564f  mov        eax, dword ptr [edi + 0x47c]
00455655  test       eax, 0x40000
0045565a  jne        0x458725

// block 00455660
00455660  fld        dword ptr [0x4b2ed0]
00455666  fstp       dword ptr [esp + 0x6c]
0045566a  test       eax, eax
0045566c  jns        0x455676

// block 0045566e
0045566e  fld1       
00455670  fstp       dword ptr [0x4b2ed0]

// block 00455676
00455676  movzx      ecx, word ptr [edi + 0x3c4]
0045567d  test       cx, cx
00455680  jne        0x455730

// block 00455686
00455686  mov        esi, dword ptr [edi + 0x3f0]
0045568c  movsx      eax, word ptr [esi + 4]
00455690  cmp        eax, dword ptr [edi + 0x6c]
00455693  mov        dword ptr [esp + 0x14], esi
00455697  jg         0x456471

// block 0045569d
0045569d  movsx      eax, word ptr [esi]
004556a0  inc        eax
004556a1  cmp        eax, 0x6f
004556a4  ja         0x457c91

// block 004556aa
004556aa  jmp        dword ptr [eax*4 + 0x458730]

// block 004556b1
004556b1  mov        ecx, dword ptr [esi + 0xc]
004556b4  push       ecx
004556b5  lea        eax, [edi + 0x68]
004556b8  call       0x4067e0

// block 004556bd
004556bd  mov        edx, dword ptr [edi + 0x3ec]
004556c3  add        edx, dword ptr [esi + 8]
004556c6  mov        dword ptr [edi + 0x3f0], edx
004556cc  jmp        0x455686

// block 004556ce
004556ce  test       byte ptr [esi + 6], 1
004556d2  lea        ebx, [esi + 8]
004556d5  mov        eax, ebx
004556d7  je         0x4556e0

// block 004556d9
004556d9  mov        edx, edi
004556db  call       0x455580

// block 004556e0
004556e0  dec        dword ptr [eax]
004556e2  test       byte ptr [esi + 6], 1
004556e6  mov        eax, dword ptr [ebx]
004556e8  je         0x4556f1

// block 004556ea
004556ea  mov        ecx, edi
004556ec  call       0x4553f0

// block 004556f1
004556f1  test       eax, eax
004556f3  jle        0x457c91

// block 004556f9
004556f9  mov        eax, dword ptr [esi + 0x10]
004556fc  push       eax
004556fd  lea        eax, [edi + 0x68]
00455700  call       0x4067e0

// block 00455705
00455705  mov        ecx, dword ptr [edi + 0x3ec]
0045570b  add        ecx, dword ptr [esi + 0xc]
0045570e  mov        dword ptr [edi + 0x3f0], ecx
00455714  jmp        0x455686

// block 00455719
00455719  and        dword ptr [edi + 0x47c], 0xfffffffe

// block 00455720
00455720  movzx      ecx, word ptr [edi + 0x3c4]
00455727  test       cx, cx
0045572a  je         0x456455

// block 00455730
00455730  mov        esi, dword ptr [edi + 0x3ec]
00455736  xor        edx, edx
00455738  jmp        0x455740

// block 00455740
00455740  movzx      eax, word ptr [esi]
00455743  cmp        ax, 0x40
00455747  jne        0x455751

// block 00455749
00455749  movsx      ebx, cx
0045574c  cmp        ebx, dword ptr [esi + 8]
0045574f  je         0x45576d

// block 00455751
00455751  cmp        ax, -1
00455755  je         0x45576d

// block 00455757
00455757  cmp        ax, 0x40
0045575b  jne        0x455765

// block 0045575d
0045575d  cmp        dword ptr [esi + 8], -1
00455761  jne        0x455765

// block 00455763
00455763  mov        edx, esi

// block 00455765
00455765  movzx      eax, word ptr [esi + 2]
00455769  add        esi, eax
0045576b  jmp        0x455740

// block 0045576d
0045576d  and        dword ptr [edi + 0x47c], 0xffffdfff
00455777  xor        ecx, ecx
00455779  mov        word ptr [edi + 0x3c4], cx
00455780  cmp        word ptr [esi], 0x40
00455784  je         0x455790

// block 00455786
00455786  test       edx, edx
00455788  je         0x45645f

// block 0045578e
0045578e  mov        esi, edx

// block 00455790
00455790  mov        edx, dword ptr [edi + 0x68]
00455793  mov        ecx, dword ptr [edi + 0x6c]
00455796  lea        eax, [edi + 0x68]
00455799  mov        dword ptr [edi + 0x3c8], edx
0045579f  mov        edx, dword ptr [eax + 8]
004557a2  mov        dword ptr [edi + 0x3cc], ecx
004557a8  mov        ecx, dword ptr [eax + 0xc]
004557ab  mov        dword ptr [edi + 0x3d0], edx
004557b1  mov        edx, dword ptr [eax + 0x10]
004557b4  mov        dword ptr [edi + 0x3d4], ecx
004557ba  mov        ecx, dword ptr [edi + 0x3f0]
004557c0  mov        dword ptr [edi + 0x3d8], edx
004557c6  mov        dword ptr [edi + 0x3dc], ecx
004557cc  movsx      edx, word ptr [esi + 4]
004557d0  push       edx
004557d1  call       0x4067e0

// block 004557d6
004557d6  movzx      eax, word ptr [esi + 2]
004557da  add        eax, esi
004557dc  or         dword ptr [edi + 0x47c], 1
004557e3  mov        dword ptr [edi + 0x3f0], eax
004557e9  jmp        0x455686

// block 004557ee
004557ee  mov        ecx, dword ptr [edi + 0x3c8]
004557f4  mov        edx, dword ptr [edi + 0x3cc]
004557fa  mov        eax, dword ptr [edi + 0x3d0]
00455800  mov        dword ptr [edi + 0x68], ecx
00455803  mov        ecx, dword ptr [edi + 0x3d4]
00455809  mov        dword ptr [edi + 0x6c], edx
0045580c  mov        edx, dword ptr [edi + 0x3d8]
00455812  mov        dword ptr [edi + 0x70], eax
00455815  mov        eax, dword ptr [edi + 0x3dc]
0045581b  mov        dword ptr [edi + 0x74], ecx
0045581e  mov        dword ptr [edi + 0x78], edx
00455821  mov        dword ptr [edi + 0x3f0], eax
00455827  jmp        0x455686

// block 0045582c
0045582c  test       byte ptr [esi + 6], 1
00455830  je         0x455840

// block 00455832
00455832  mov        eax, dword ptr [esi + 8]
00455835  mov        ecx, edi
00455837  call       0x4553f0

// block 0045583c
0045583c  mov        ebx, eax
0045583e  jmp        0x455843

// block 00455840
00455840  mov        ebx, dword ptr [esi + 8]

// block 00455843
00455843  test       byte ptr [esi + 6], 2
00455847  mov        eax, dword ptr [esi + 0xc]
0045584a  je         0x455853

// block 0045584c
0045584c  mov        ecx, edi
0045584e  call       0x4553f0

// block 00455853
00455853  cmp        ebx, eax
00455855  jne        0x457c91

// block 0045585b
0045585b  jmp        0x455b09

// block 00455860
00455860  test       byte ptr [esi + 6], 1
00455864  fld        dword ptr [esi + 8]
00455867  je         0x455874

// block 00455869
00455869  push       ecx
0045586a  mov        eax, edi
0045586c  fstp       dword ptr [esp]
0045586f  call       0x4551b0

// block 00455874
00455874  test       byte ptr [esi + 6], 2
00455878  fstp       dword ptr [esp + 0x14]
0045587c  fld        dword ptr [esi + 0xc]
0045587f  je         0x45588c

// block 00455881
00455881  push       ecx
00455882  mov        eax, edi
00455884  fstp       dword ptr [esp]
00455887  call       0x4551b0

// block 0045588c
0045588c  fstp       dword ptr [esp + 0x10]
00455890  fld        dword ptr [esp + 0x14]
00455894  fld        dword ptr [esp + 0x10]
00455898  fucompp    
0045589a  fnstsw     ax
0045589c  test       ah, 0x44
0045589f  jmp        0x455b03

// block 004558a4
004558a4  test       byte ptr [esi + 6], 1
004558a8  je         0x4558b8

// block 004558aa
004558aa  mov        eax, dword ptr [esi + 8]
004558ad  mov        ecx, edi
004558af  call       0x4553f0

// block 004558b4
004558b4  mov        ebx, eax
004558b6  jmp        0x4558bb

// block 004558b8
004558b8  mov        ebx, dword ptr [esi + 8]

// block 004558bb
004558bb  test       byte ptr [esi + 6], 2
004558bf  mov        eax, dword ptr [esi + 0xc]
004558c2  je         0x4558cb

// block 004558c4
004558c4  mov        ecx, edi
004558c6  call       0x4553f0

// block 004558cb
004558cb  cmp        ebx, eax
004558cd  je         0x457c91

// block 004558d3
004558d3  jmp        0x455b09

// block 004558d8
004558d8  test       byte ptr [esi + 6], 1
004558dc  fld        dword ptr [esi + 8]
004558df  je         0x4558ec

// block 004558e1
004558e1  push       ecx
004558e2  mov        eax, edi
004558e4  fstp       dword ptr [esp]
004558e7  call       0x4551b0

// block 004558ec
004558ec  test       byte ptr [esi + 6], 2
004558f0  fstp       dword ptr [esp + 0x10]
004558f4  fld        dword ptr [esi + 0xc]
004558f7  je         0x455904

// block 004558f9
004558f9  push       ecx
004558fa  mov        eax, edi
004558fc  fstp       dword ptr [esp]
004558ff  call       0x4551b0

// block 00455904
00455904  fstp       dword ptr [esp + 0x14]
00455908  fld        dword ptr [esp + 0x10]
0045590c  fld        dword ptr [esp + 0x14]
00455910  fucompp    
00455912  fnstsw     ax
00455914  test       ah, 0x44
00455917  jnp        0x457c91

// block 0045591d
0045591d  jmp        0x455b09

// block 00455922
00455922  test       byte ptr [esi + 6], 1
00455926  je         0x455936

// block 00455928
00455928  mov        eax, dword ptr [esi + 8]
0045592b  mov        ecx, edi
0045592d  call       0x4553f0

// block 00455932
00455932  mov        ebx, eax
00455934  jmp        0x455939

// block 00455936
00455936  mov        ebx, dword ptr [esi + 8]

// block 00455939
00455939  test       byte ptr [esi + 6], 2
0045593d  mov        eax, dword ptr [esi + 0xc]
00455940  je         0x455949

// block 00455942
00455942  mov        ecx, edi
00455944  call       0x4553f0

// block 00455949
00455949  cmp        ebx, eax
0045594b  jge        0x457c91

// block 00455951
00455951  jmp        0x455b09

// block 00455956
00455956  test       byte ptr [esi + 6], 1
0045595a  fld        dword ptr [esi + 8]
0045595d  je         0x45596a

// block 0045595f
0045595f  push       ecx
00455960  mov        eax, edi
00455962  fstp       dword ptr [esp]
00455965  call       0x4551b0

// block 0045596a
0045596a  test       byte ptr [esi + 6], 2
0045596e  fstp       dword ptr [esp + 0x10]
00455972  fld        dword ptr [esi + 0xc]
00455975  je         0x455982

// block 00455977
00455977  push       ecx
00455978  mov        eax, edi
0045597a  fstp       dword ptr [esp]
0045597d  call       0x4551b0

// block 00455982
00455982  fstp       dword ptr [esp + 0x14]
00455986  fld        dword ptr [esp + 0x10]
0045598a  fld        dword ptr [esp + 0x14]
0045598e  fcompp     
00455990  fnstsw     ax
00455992  test       ah, 0x41
00455995  jne        0x457c91

// block 0045599b
0045599b  jmp        0x455b09

// block 004559a0
004559a0  test       byte ptr [esi + 6], 1
004559a4  je         0x4559b4

// block 004559a6
004559a6  mov        eax, dword ptr [esi + 8]
004559a9  mov        ecx, edi
004559ab  call       0x4553f0

// block 004559b0
004559b0  mov        ebx, eax
004559b2  jmp        0x4559b7

// block 004559b4
004559b4  mov        ebx, dword ptr [esi + 8]

// block 004559b7
004559b7  test       byte ptr [esi + 6], 2
004559bb  mov        eax, dword ptr [esi + 0xc]
004559be  je         0x4559c7

// block 004559c0
004559c0  mov        ecx, edi
004559c2  call       0x4553f0

// block 004559c7
004559c7  cmp        ebx, eax
004559c9  jg         0x457c91

// block 004559cf
004559cf  jmp        0x455b09

// block 004559d4
004559d4  test       byte ptr [esi + 6], 1
004559d8  fld        dword ptr [esi + 8]
004559db  je         0x4559e8

// block 004559dd
004559dd  push       ecx
004559de  mov        eax, edi
004559e0  fstp       dword ptr [esp]
004559e3  call       0x4551b0

// block 004559e8
004559e8  test       byte ptr [esi + 6], 2
004559ec  fstp       dword ptr [esp + 0x10]
004559f0  fld        dword ptr [esi + 0xc]
004559f3  je         0x455a00

// block 004559f5
004559f5  push       ecx
004559f6  mov        eax, edi
004559f8  fstp       dword ptr [esp]
004559fb  call       0x4551b0

// block 00455a00
00455a00  fstp       dword ptr [esp + 0x14]
00455a04  fld        dword ptr [esp + 0x10]
00455a08  fld        dword ptr [esp + 0x14]
00455a0c  fcompp     
00455a0e  fnstsw     ax
00455a10  test       ah, 1
00455a13  jne        0x457c91

// block 00455a19
00455a19  jmp        0x455b09

// block 00455a1e
00455a1e  test       byte ptr [esi + 6], 1
00455a22  je         0x455a32

// block 00455a24
00455a24  mov        eax, dword ptr [esi + 8]
00455a27  mov        ecx, edi
00455a29  call       0x4553f0

// block 00455a2e
00455a2e  mov        ebx, eax
00455a30  jmp        0x455a35

// block 00455a32
00455a32  mov        ebx, dword ptr [esi + 8]

// block 00455a35
00455a35  test       byte ptr [esi + 6], 2
00455a39  mov        eax, dword ptr [esi + 0xc]
00455a3c  je         0x455a45

// block 00455a3e
00455a3e  mov        ecx, edi
00455a40  call       0x4553f0

// block 00455a45
00455a45  cmp        ebx, eax
00455a47  jle        0x457c91

// block 00455a4d
00455a4d  jmp        0x455b09

// block 00455a52
00455a52  test       byte ptr [esi + 6], 1
00455a56  fld        dword ptr [esi + 8]
00455a59  je         0x455a66

// block 00455a5b
00455a5b  push       ecx
00455a5c  mov        eax, edi
00455a5e  fstp       dword ptr [esp]
00455a61  call       0x4551b0

// block 00455a66
00455a66  test       byte ptr [esi + 6], 2
00455a6a  fstp       dword ptr [esp + 0x10]
00455a6e  fld        dword ptr [esi + 0xc]
00455a71  je         0x455a7e

// block 00455a73
00455a73  push       ecx
00455a74  mov        eax, edi
00455a76  fstp       dword ptr [esp]
00455a79  call       0x4551b0

// block 00455a7e
00455a7e  fstp       dword ptr [esp + 0x14]
00455a82  fld        dword ptr [esp + 0x10]
00455a86  fld        dword ptr [esp + 0x14]
00455a8a  fcompp     
00455a8c  fnstsw     ax
00455a8e  test       ah, 5
00455a91  jmp        0x455b03

// block 00455a93
00455a93  test       byte ptr [esi + 6], 1
00455a97  je         0x455aa7

// block 00455a99
00455a99  mov        eax, dword ptr [esi + 8]
00455a9c  mov        ecx, edi
00455a9e  call       0x4553f0

// block 00455aa3
00455aa3  mov        ebx, eax
00455aa5  jmp        0x455aaa

// block 00455aa7
00455aa7  mov        ebx, dword ptr [esi + 8]

// block 00455aaa
00455aaa  test       byte ptr [esi + 6], 2
00455aae  mov        eax, dword ptr [esi + 0xc]
00455ab1  je         0x455aba

// block 00455ab3
00455ab3  mov        ecx, edi
00455ab5  call       0x4553f0

// block 00455aba
00455aba  cmp        ebx, eax
00455abc  jl         0x457c91

// block 00455ac2
00455ac2  jmp        0x455b09

// block 00455ac4
00455ac4  test       byte ptr [esi + 6], 1
00455ac8  fld        dword ptr [esi + 8]
00455acb  je         0x455ad8

// block 00455acd
00455acd  push       ecx
00455ace  mov        eax, edi
00455ad0  fstp       dword ptr [esp]
00455ad3  call       0x4551b0

// block 00455ad8
00455ad8  test       byte ptr [esi + 6], 2
00455adc  fstp       dword ptr [esp + 0x10]
00455ae0  fld        dword ptr [esi + 0xc]
00455ae3  je         0x455af0

// block 00455ae5
00455ae5  push       ecx
00455ae6  mov        eax, edi
00455ae8  fstp       dword ptr [esp]
00455aeb  call       0x4551b0

// block 00455af0
00455af0  fstp       dword ptr [esp + 0x14]
00455af4  fld        dword ptr [esp + 0x10]
00455af8  fld        dword ptr [esp + 0x14]
00455afc  fcompp     
00455afe  fnstsw     ax
00455b00  test       ah, 0x41

// block 00455b03
00455b03  jp         0x457c91

// block 00455b09
00455b09  mov        ecx, dword ptr [esi + 0x14]
00455b0c  push       ecx
00455b0d  lea        eax, [edi + 0x68]
00455b10  call       0x4067e0

// block 00455b15
00455b15  mov        edx, dword ptr [edi + 0x3ec]
00455b1b  add        edx, dword ptr [esi + 0x10]
00455b1e  mov        dword ptr [edi + 0x3f0], edx
00455b24  jmp        0x455686

// block 00455b29
00455b29  and        dword ptr [edi + 0x47c], 0xfffffffe

// block 00455b30
00455b30  fld        dword ptr [esp + 0x6c]
00455b34  mov        dword ptr [edi + 0x3f0], 0
00455b3e  fstp       dword ptr [0x4b2ed0]

// block 00455b44
00455b44  mov        eax, 1
00455b49  pop        edi
00455b4a  pop        esi
00455b4b  pop        ebx
00455b4c  mov        esp, ebp
00455b4e  pop        ebp
00455b4f  ret        4

// block 00455b52
00455b52  test       byte ptr [esi + 6], 1
00455b56  mov        eax, dword ptr [esi + 8]
00455b59  je         0x455b62

// block 00455b5b
00455b5b  mov        ecx, edi
00455b5d  call       0x4553f0

// block 00455b62
00455b62  mov        ecx, dword ptr [edi + 0x47c]
00455b68  and        ecx, 0x7fffffff
00455b6e  shl        eax, 0x1f
00455b71  or         eax, ecx
00455b73  mov        dword ptr [edi + 0x47c], eax
00455b79  movzx      ecx, word ptr [esi + 2]
00455b7d  add        ecx, esi
00455b7f  mov        dword ptr [edi + 0x3f0], ecx
00455b85  jmp        0x455686

// block 00455b8a
00455b8a  test       byte ptr [esi + 6], 1
00455b8e  mov        eax, dword ptr [esi + 8]
00455b91  je         0x455b9a

// block 00455b93
00455b93  mov        ecx, edi
00455b95  call       0x4553f0

// block 00455b9a
00455b9a  push       edi
00455b9b  mov        edi, dword ptr [edi + 0x3f8]
00455ba1  push       eax
00455ba2  lea        edx, [esp + 0xe4]
00455ba9  push       edx
00455baa  xor        eax, eax
00455bac  call       0x461720

// block 00455bb1
00455bb1  movzx      ecx, word ptr [esi + 2]
00455bb5  mov        edi, dword ptr [ebp + 8]
00455bb8  add        ecx, esi
00455bba  mov        dword ptr [edi + 0x3f0], ecx
00455bc0  jmp        0x455686

// block 00455bc5
00455bc5  test       byte ptr [esi + 6], 1
00455bc9  mov        eax, dword ptr [esi + 8]
00455bcc  je         0x455bd5

// block 00455bce
00455bce  mov        ecx, edi
00455bd0  call       0x4553f0

// block 00455bd5
00455bd5  push       edi
00455bd6  mov        edi, dword ptr [edi + 0x3f8]
00455bdc  push       eax
00455bdd  lea        eax, [esp + 0x44]
00455be1  push       eax
00455be2  xor        eax, eax
00455be4  call       0x461720

// block 00455be9
00455be9  lea        esi, [esp + 0x3c]
00455bed  call       0x461c50

// block 00455bf2
00455bf2  mov        esi, dword ptr [esp + 0x14]
00455bf6  test       byte ptr [esi + 6], 2
00455bfa  fld        dword ptr [esi + 0xc]
00455bfd  mov        edi, eax
00455bff  je         0x455c0d

// block 00455c01
00455c01  mov        eax, dword ptr [ebp + 8]
00455c04  push       ecx
00455c05  fstp       dword ptr [esp]
00455c08  call       0x4551b0

// block 00455c0d
00455c0d  fstp       dword ptr [esp + 0x10]
00455c11  fld        dword ptr [esp + 0x10]
00455c15  fstp       dword ptr [edi + 0x43c]
00455c1b  test       byte ptr [esi + 6], 4
00455c1f  fld        dword ptr [esi + 0x10]
00455c22  je         0x455c30

// block 00455c24
00455c24  mov        eax, dword ptr [ebp + 8]
00455c27  push       ecx
00455c28  fstp       dword ptr [esp]
00455c2b  call       0x4551b0

// block 00455c30
00455c30  mov        esi, dword ptr [esp + 0x14]
00455c34  fstp       dword ptr [esp + 0x10]
00455c38  fld        dword ptr [esp + 0x10]
00455c3c  fstp       dword ptr [edi + 0x440]
00455c42  movzx      ecx, word ptr [esi + 2]
00455c46  mov        edi, dword ptr [ebp + 8]
00455c49  add        ecx, esi
00455c4b  mov        dword ptr [edi + 0x3f0], ecx
00455c51  jmp        0x455686

// block 00455c56
00455c56  test       byte ptr [esi + 6], 1
00455c5a  mov        eax, dword ptr [esi + 8]
00455c5d  je         0x455c66

// block 00455c5f
00455c5f  mov        ecx, edi
00455c61  call       0x4553f0

// block 00455c66
00455c66  push       edi
00455c67  mov        edi, dword ptr [edi + 0x3f8]
00455c6d  push       eax
00455c6e  lea        ecx, [esp + 0xdc]
00455c75  push       ecx
00455c76  mov        eax, 2
00455c7b  call       0x461720

// block 00455c80
00455c80  movzx      ecx, word ptr [esi + 2]
00455c84  mov        edi, dword ptr [ebp + 8]
00455c87  add        ecx, esi
00455c89  mov        dword ptr [edi + 0x3f0], ecx
00455c8f  jmp        0x455686

// block 00455c94
00455c94  test       byte ptr [esi + 6], 1
00455c98  mov        eax, dword ptr [esi + 8]
00455c9b  je         0x455ca4

// block 00455c9d
00455c9d  mov        ecx, edi
00455c9f  call       0x4553f0

// block 00455ca4
00455ca4  push       edi
00455ca5  mov        edi, dword ptr [edi + 0x3f8]
00455cab  push       eax
00455cac  lea        edx, [esp + 0xd4]
00455cb3  push       edx
00455cb4  mov        eax, 4
00455cb9  call       0x461720

// block 00455cbe
00455cbe  movzx      ecx, word ptr [esi + 2]
00455cc2  mov        edi, dword ptr [ebp + 8]
00455cc5  add        ecx, esi
00455cc7  mov        dword ptr [edi + 0x3f0], ecx
00455ccd  jmp        0x455686

// block 00455cd2
00455cd2  test       byte ptr [esi + 6], 1
00455cd6  mov        eax, dword ptr [esi + 8]
00455cd9  je         0x455ce2

// block 00455cdb
00455cdb  mov        ecx, edi
00455cdd  call       0x4553f0

// block 00455ce2
00455ce2  push       edi
00455ce3  mov        edi, dword ptr [edi + 0x3f8]
00455ce9  push       eax
00455cea  lea        eax, [esp + 0xe0]
00455cf1  push       eax
00455cf2  mov        eax, 6
00455cf7  call       0x461720

// block 00455cfc
00455cfc  movzx      ecx, word ptr [esi + 2]
00455d00  mov        edi, dword ptr [ebp + 8]
00455d03  add        ecx, esi
00455d05  mov        dword ptr [edi + 0x3f0], ecx
00455d0b  jmp        0x455686

// block 00455d10
00455d10  test       byte ptr [esi + 6], 1
00455d14  mov        eax, dword ptr [esi + 8]
00455d17  je         0x455d20

// block 00455d19
00455d19  mov        ecx, edi
00455d1b  call       0x4553f0

// block 00455d20
00455d20  mov        ebx, dword ptr [ebp + 8]
00455d23  push       eax
00455d24  mov        eax, dword ptr [ebx + 0x3f8]
00455d2a  lea        ecx, [esp + 0x1c]
00455d2e  push       ecx
00455d2f  call       0x461840

// block 00455d34
00455d34  lea        esi, [esp + 0x18]
00455d38  call       0x461c50

// block 00455d3d
00455d3d  mov        esi, dword ptr [esp + 0x14]
00455d41  test       byte ptr [esi + 6], 2
00455d45  fld        dword ptr [esi + 0xc]
00455d48  mov        edi, eax
00455d4a  je         0x455d57

// block 00455d4c
00455d4c  push       ecx
00455d4d  mov        eax, ebx
00455d4f  fstp       dword ptr [esp]
00455d52  call       0x4551b0

// block 00455d57
00455d57  fstp       dword ptr [esp + 0x10]
00455d5b  fld        dword ptr [esp + 0x10]
00455d5f  fstp       dword ptr [edi + 0x43c]
00455d65  test       byte ptr [esi + 6], 4
00455d69  fld        dword ptr [esi + 0x10]
00455d6c  je         0x455d79

// block 00455d6e
00455d6e  push       ecx
00455d6f  mov        eax, ebx
00455d71  fstp       dword ptr [esp]
00455d74  call       0x4551b0

// block 00455d79
00455d79  mov        esi, dword ptr [esp + 0x14]
00455d7d  fstp       dword ptr [esp + 0x10]
00455d81  fld        dword ptr [esp + 0x10]
00455d85  fstp       dword ptr [edi + 0x440]
00455d8b  movzx      ecx, word ptr [esi + 2]
00455d8f  mov        edi, ebx
00455d91  add        ecx, esi
00455d93  mov        dword ptr [edi + 0x3f0], ecx
00455d99  jmp        0x455686

// block 00455d9e
00455d9e  test       byte ptr [esi + 6], 1
00455da2  mov        eax, dword ptr [esi + 8]
00455da5  je         0x455dae

// block 00455da7
00455da7  mov        ecx, edi
00455da9  call       0x4553f0

// block 00455dae
00455dae  push       eax
00455daf  mov        eax, dword ptr [ebp + 8]
00455db2  mov        eax, dword ptr [eax + 0x3f8]
00455db8  lea        edx, [esp + 0xd4]
00455dbf  push       edx
00455dc0  call       0x461840

// block 00455dc5
00455dc5  movzx      ecx, word ptr [esi + 2]
00455dc9  mov        edi, dword ptr [ebp + 8]
00455dcc  add        ecx, esi
00455dce  mov        dword ptr [edi + 0x3f0], ecx
00455dd4  jmp        0x455686

// block 00455dd9
00455dd9  mov        eax, 1
00455dde  or         dword ptr [edi + 0x47c], eax
00455de4  cmp        dword ptr [edi + 0x4ac], 0
00455deb  je         0x455e0a

// block 00455ded
00455ded  test       byte ptr [esi + 6], al
00455df0  mov        eax, dword ptr [esi + 8]
00455df3  je         0x455dfc

// block 00455df5
00455df5  mov        ecx, edi
00455df7  call       0x4553f0

// block 00455dfc
00455dfc  mov        edx, eax
00455dfe  mov        eax, dword ptr [edi + 0x4ac]
00455e04  mov        ecx, edi
00455e06  call       eax

// block 00455e08
00455e08  jmp        0x455e19

// block 00455e0a
00455e0a  test       byte ptr [esi + 6], al
00455e0d  mov        eax, dword ptr [esi + 8]
00455e10  je         0x455e19

// block 00455e12
00455e12  mov        ecx, edi
00455e14  call       0x4553f0

// block 00455e19
00455e19  test       eax, eax
00455e1b  jge        0x455e30

// block 00455e1d
00455e1d  mov        edx, dword ptr [0x4b43b8]
00455e23  mov        edx, dword ptr [edx + 0x18fb4]
00455e29  mov        ecx, 0x108
00455e2e  jmp        0x455e38

// block 00455e30
00455e30  mov        edx, dword ptr [edi + 0x3f8]
00455e36  mov        ecx, eax

// block 00455e38
00455e38  mov        eax, edi
00455e3a  call       0x454b80

// block 00455e3f
00455e3f  mov        eax, dword ptr [edi + 0x6c]
00455e42  mov        dword ptr [edi + 0x3e0], eax
00455e48  movzx      ecx, word ptr [esi + 2]
00455e4c  add        ecx, esi
00455e4e  mov        dword ptr [edi + 0x3f0], ecx
00455e54  jmp        0x455686

// block 00455e59
00455e59  mov        eax, 1
00455e5e  or         dword ptr [edi + 0x47c], eax
00455e64  cmp        dword ptr [edi + 0x4ac], 0
00455e6b  je         0x455ebe

// block 00455e6d
00455e6d  test       byte ptr [esi + 6], al
00455e70  je         0x455e82

// block 00455e72
00455e72  mov        eax, dword ptr [esi + 8]
00455e75  mov        ecx, edi
00455e77  call       0x4553f0

// block 00455e7c
00455e7c  mov        dword ptr [esp + 0x10], eax
00455e80  jmp        0x455e89

// block 00455e82
00455e82  mov        ecx, dword ptr [esi + 8]
00455e85  mov        dword ptr [esp + 0x10], ecx

// block 00455e89
00455e89  test       byte ptr [esi + 6], 2
00455e8d  je         0x455e9d

// block 00455e8f
00455e8f  mov        eax, dword ptr [esi + 0xc]
00455e92  mov        ecx, edi
00455e94  call       0x4553f0

// block 00455e99
00455e99  mov        ebx, eax
00455e9b  jmp        0x455ea0

// block 00455e9d
00455e9d  mov        ebx, dword ptr [esi + 0xc]

// block 00455ea0
00455ea0  mov        esi, 0x4ce568
00455ea5  call       0x464440

// block 00455eaa
00455eaa  xor        edx, edx
00455eac  div        ebx
00455eae  mov        eax, dword ptr [edi + 0x4ac]
00455eb4  mov        ecx, edi
00455eb6  add        edx, dword ptr [esp + 0x10]
00455eba  call       eax

// block 00455ebc
00455ebc  jmp        0x455f05

// block 00455ebe
00455ebe  test       byte ptr [esi + 6], al
00455ec1  je         0x455ed3

// block 00455ec3
00455ec3  mov        eax, dword ptr [esi + 8]
00455ec6  mov        ecx, edi
00455ec8  call       0x4553f0

// block 00455ecd
00455ecd  mov        dword ptr [esp + 0x10], eax
00455ed1  jmp        0x455eda

// block 00455ed3
00455ed3  mov        ecx, dword ptr [esi + 8]
00455ed6  mov        dword ptr [esp + 0x10], ecx

// block 00455eda
00455eda  test       byte ptr [esi + 6], 2
00455ede  je         0x455eee

// block 00455ee0
00455ee0  mov        eax, dword ptr [esi + 0xc]
00455ee3  mov        ecx, edi
00455ee5  call       0x4553f0

// block 00455eea
00455eea  mov        ebx, eax
00455eec  jmp        0x455ef1

// block 00455eee
00455eee  mov        ebx, dword ptr [esi + 0xc]

// block 00455ef1
00455ef1  mov        esi, 0x4ce568
00455ef6  call       0x464440

// block 00455efb
00455efb  xor        edx, edx
00455efd  div        ebx
00455eff  mov        eax, edx
00455f01  add        eax, dword ptr [esp + 0x10]

// block 00455f05
00455f05  test       eax, eax
00455f07  jge        0x455f1c

// block 00455f09
00455f09  mov        edx, dword ptr [0x4b43b8]
00455f0f  mov        edx, dword ptr [edx + 0x18fb4]
00455f15  mov        ecx, 0x108
00455f1a  jmp        0x455f24

// block 00455f1c
00455f1c  mov        edx, dword ptr [edi + 0x3f8]
00455f22  mov        ecx, eax

// block 00455f24
00455f24  mov        eax, edi
00455f26  call       0x454b80

// block 00455f2b
00455f2b  mov        eax, dword ptr [edi + 0x6c]
00455f2e  mov        esi, dword ptr [esp + 0x14]
00455f32  mov        dword ptr [edi + 0x3e0], eax
00455f38  movzx      ecx, word ptr [esi + 2]
00455f3c  add        ecx, esi
00455f3e  mov        dword ptr [edi + 0x3f0], ecx
00455f44  jmp        0x455686

// block 00455f49
00455f49  test       byte ptr [esi + 6], 1
00455f4d  fld        dword ptr [esi + 8]
00455f50  je         0x455f5d

// block 00455f52
00455f52  push       ecx
00455f53  mov        eax, edi
00455f55  fstp       dword ptr [esp]
00455f58  call       0x4551b0

// block 00455f5d
00455f5d  fstp       dword ptr [esp + 0x10]
00455f61  fld        dword ptr [esp + 0x10]
00455f65  fstp       dword ptr [edi + 0x40]
00455f68  test       byte ptr [esi + 6], 2
00455f6c  fld        dword ptr [esi + 0xc]
00455f6f  je         0x455f7c

// block 00455f71
00455f71  push       ecx
00455f72  mov        eax, edi
00455f74  fstp       dword ptr [esp]
00455f77  call       0x4551b0

// block 00455f7c
00455f7c  or         dword ptr [edi + 0x47c], 8
00455f83  fstp       dword ptr [esp + 0x10]
00455f87  fld        dword ptr [esp + 0x10]
00455f8b  fstp       dword ptr [edi + 0x44]
00455f8e  movzx      ecx, word ptr [esi + 2]
00455f92  add        ecx, esi
00455f94  mov        dword ptr [edi + 0x3f0], ecx
00455f9a  jmp        0x455686

// block 00455f9f
00455f9f  test       byte ptr [esi + 6], 1
00455fa3  fld        dword ptr [esi + 8]
00455fa6  je         0x455fb3

// block 00455fa8
00455fa8  push       ecx
00455fa9  mov        eax, edi
00455fab  fstp       dword ptr [esp]
00455fae  call       0x4551b0

// block 00455fb3
00455fb3  fstp       dword ptr [esp + 0x10]
00455fb7  fld        dword ptr [esp + 0x10]
00455fbb  fstp       dword ptr [edi + 0x50]
00455fbe  test       byte ptr [esi + 6], 2
00455fc2  fld        dword ptr [esi + 0xc]
00455fc5  je         0x455fd2

// block 00455fc7
00455fc7  push       ecx
00455fc8  mov        eax, edi
00455fca  fstp       dword ptr [esp]
00455fcd  call       0x4551b0

// block 00455fd2
00455fd2  or         dword ptr [edi + 0x47c], 0x10
00455fd9  fstp       dword ptr [esp + 0x10]
00455fdd  fld        dword ptr [esp + 0x10]
00455fe1  fstp       dword ptr [edi + 0x54]
00455fe4  movzx      ecx, word ptr [esi + 2]
00455fe8  add        ecx, esi
00455fea  mov        dword ptr [edi + 0x3f0], ecx
00455ff0  jmp        0x455686

// block 00455ff5
00455ff5  test       byte ptr [esi + 6], 1
00455ff9  mov        eax, dword ptr [esi + 8]
00455ffc  je         0x456005

// block 00455ffe
00455ffe  mov        ecx, edi
00456000  call       0x4553f0

// block 00456005
00456005  mov        byte ptr [edi + 0x3bf], al
0045600b  movzx      ecx, word ptr [esi + 2]
0045600f  add        ecx, esi
00456011  mov        dword ptr [edi + 0x3f0], ecx
00456017  jmp        0x455686

// block 0045601c
0045601c  test       byte ptr [esi + 6], 1
00456020  mov        eax, dword ptr [esi + 8]
00456023  je         0x45602c

// block 00456025
00456025  mov        ecx, edi
00456027  call       0x4553f0

// block 0045602c
0045602c  mov        byte ptr [edi + 0x3be], al
00456032  test       byte ptr [esi + 6], 2
00456036  mov        eax, dword ptr [esi + 0xc]
00456039  je         0x456042

// block 0045603b
0045603b  mov        ecx, edi
0045603d  call       0x4553f0

// block 00456042
00456042  mov        byte ptr [edi + 0x3bd], al
00456048  test       byte ptr [esi + 6], 4
0045604c  mov        eax, dword ptr [esi + 0x10]
0045604f  je         0x456058

// block 00456051
00456051  mov        ecx, edi
00456053  call       0x4553f0

// block 00456058
00456058  mov        byte ptr [edi + 0x3bc], al
0045605e  movzx      ecx, word ptr [esi + 2]
00456062  add        ecx, esi
00456064  mov        dword ptr [edi + 0x3f0], ecx
0045606a  jmp        0x455686

// block 0045606f
0045606f  test       byte ptr [esi + 6], 1
00456073  mov        eax, dword ptr [esi + 8]
00456076  je         0x45607f

// block 00456078
00456078  mov        ecx, edi
0045607a  call       0x4553f0

// block 0045607f
0045607f  mov        byte ptr [edi + 0x3c3], al
00456085  movzx      ecx, word ptr [esi + 2]
00456089  add        ecx, esi
0045608b  mov        dword ptr [edi + 0x3f0], ecx
00456091  jmp        0x455686

// block 00456096
00456096  test       byte ptr [esi + 6], 1
0045609a  mov        eax, dword ptr [esi + 8]
0045609d  je         0x4560a6

// block 0045609f
0045609f  mov        ecx, edi
004560a1  call       0x4553f0

// block 004560a6
004560a6  mov        byte ptr [edi + 0x3c2], al
004560ac  test       byte ptr [esi + 6], 2
004560b0  mov        eax, dword ptr [esi + 0xc]
004560b3  je         0x4560bc

// block 004560b5
004560b5  mov        ecx, edi
004560b7  call       0x4553f0

// block 004560bc
004560bc  mov        byte ptr [edi + 0x3c1], al
004560c2  test       byte ptr [esi + 6], 4
004560c6  mov        eax, dword ptr [esi + 0x10]
004560c9  je         0x4560d2

// block 004560cb
004560cb  mov        ecx, edi
004560cd  call       0x4553f0

// block 004560d2
004560d2  mov        byte ptr [edi + 0x3c0], al
004560d8  movzx      ecx, word ptr [esi + 2]
004560dc  add        ecx, esi
004560de  mov        dword ptr [edi + 0x3f0], ecx
004560e4  jmp        0x455686

// block 004560e9
004560e9  mov        eax, dword ptr [edi + 0x47c]
004560ef  fld        dword ptr [edi + 0x40]
004560f2  fmul       qword ptr [0x4a4168]
004560f8  xor        eax, 0x400
004560fd  or         eax, 8
00456100  mov        dword ptr [edi + 0x47c], eax
00456106  fstp       dword ptr [edi + 0x40]
00456109  movzx      ecx, word ptr [esi + 2]
0045610d  add        ecx, esi
0045610f  mov        dword ptr [edi + 0x3f0], ecx
00456115  jmp        0x455686

// block 0045611a
0045611a  mov        eax, dword ptr [edi + 0x47c]
00456120  fld        dword ptr [edi + 0x44]
00456123  fmul       qword ptr [0x4a4168]
00456129  xor        eax, 0x800
0045612e  or         eax, 8
00456131  mov        dword ptr [edi + 0x47c], eax
00456137  fstp       dword ptr [edi + 0x44]
0045613a  movzx      ecx, word ptr [esi + 2]
0045613e  add        ecx, esi
00456140  mov        dword ptr [edi + 0x3f0], ecx
00456146  jmp        0x455686

// block 0045614b
0045614b  test       byte ptr [esi + 6], 1
0045614f  fld        dword ptr [esi + 8]
00456152  je         0x45615f

// block 00456154
00456154  push       ecx
00456155  mov        eax, edi
00456157  fstp       dword ptr [esp]
0045615a  call       0x4551b0

// block 0045615f
0045615f  fstp       dword ptr [esp + 0x10]
00456163  fld        dword ptr [esp + 0x10]
00456167  fstp       dword ptr [edi + 0x24]
0045616a  test       byte ptr [esi + 6], 2
0045616e  fld        dword ptr [esi + 0xc]
00456171  je         0x45617e

// block 00456173
00456173  push       ecx
00456174  mov        eax, edi
00456176  fstp       dword ptr [esp]
00456179  call       0x4551b0

// block 0045617e
0045617e  fstp       dword ptr [esp + 0x10]
00456182  mov        ebx, 4
00456187  fld        dword ptr [esp + 0x10]
0045618b  fstp       dword ptr [edi + 0x28]
0045618e  fld        dword ptr [esi + 0x10]
00456191  test       byte ptr [esi + 6], bl
00456194  je         0x4561a1

// block 00456196
00456196  push       ecx
00456197  mov        eax, edi
00456199  fstp       dword ptr [esp]
0045619c  call       0x4551b0

// block 004561a1
004561a1  or         dword ptr [edi + 0x47c], ebx
004561a7  fstp       dword ptr [esp + 0x10]
004561ab  fld        dword ptr [esp + 0x10]
004561af  fstp       dword ptr [edi + 0x2c]
004561b2  movzx      ecx, word ptr [esi + 2]
004561b6  add        ecx, esi
004561b8  mov        dword ptr [edi + 0x3f0], ecx
004561be  jmp        0x455686

// block 004561c3
004561c3  test       byte ptr [esi + 6], 1
004561c7  fld        dword ptr [esi + 8]
004561ca  je         0x4561d7

// block 004561cc
004561cc  push       ecx
004561cd  mov        eax, edi
004561cf  fstp       dword ptr [esp]
004561d2  call       0x4551b0

// block 004561d7
004561d7  fstp       dword ptr [esp + 0x10]
004561db  fld        dword ptr [esp + 0x10]
004561df  fstp       dword ptr [edi + 0x30]
004561e2  test       byte ptr [esi + 6], 2
004561e6  fld        dword ptr [esi + 0xc]
004561e9  je         0x4561f6

// block 004561eb
004561eb  push       ecx
004561ec  mov        eax, edi
004561ee  fstp       dword ptr [esp]
004561f1  call       0x4551b0

// block 004561f6
004561f6  fstp       dword ptr [esp + 0x10]
004561fa  mov        ebx, 4
004561ff  fld        dword ptr [esp + 0x10]
00456203  fstp       dword ptr [edi + 0x34]
00456206  fld        dword ptr [esi + 0x10]
00456209  test       byte ptr [esi + 6], bl
0045620c  je         0x456219

// block 0045620e
0045620e  push       ecx
0045620f  mov        eax, edi
00456211  fstp       dword ptr [esp]
00456214  call       0x4551b0

// block 00456219
00456219  or         dword ptr [edi + 0x47c], ebx
0045621f  fstp       dword ptr [esp + 0x10]
00456223  fld        dword ptr [esp + 0x10]
00456227  fstp       dword ptr [edi + 0x38]
0045622a  movzx      ecx, word ptr [esi + 2]
0045622e  add        ecx, esi
00456230  mov        dword ptr [edi + 0x3f0], ecx
00456236  jmp        0x455686

// block 0045623b
0045623b  test       byte ptr [esi + 6], 1
0045623f  fld        dword ptr [esi + 8]
00456242  je         0x45624f

// block 00456244
00456244  push       ecx
00456245  mov        eax, edi
00456247  fstp       dword ptr [esp]
0045624a  call       0x4551b0

// block 0045624f
0045624f  fstp       dword ptr [esp + 0x10]
00456253  fld        dword ptr [esp + 0x10]
00456257  fstp       dword ptr [edi + 0x48]
0045625a  test       byte ptr [esi + 6], 2
0045625e  fld        dword ptr [esi + 0xc]
00456261  je         0x45626e

// block 00456263
00456263  push       ecx
00456264  mov        eax, edi
00456266  fstp       dword ptr [esp]
00456269  call       0x4551b0

// block 0045626e
0045626e  fstp       dword ptr [esp + 0x10]
00456272  fld        dword ptr [esp + 0x10]
00456276  fstp       dword ptr [edi + 0x4c]
00456279  movzx      ecx, word ptr [esi + 2]
0045627d  add        ecx, esi
0045627f  mov        dword ptr [edi + 0x3f0], ecx
00456285  jmp        0x455686

// block 0045628a
0045628a  test       byte ptr [esi + 6], 2
0045628e  mov        eax, dword ptr [esi + 0xc]
00456291  je         0x45629a

// block 00456293
00456293  mov        ecx, edi
00456295  call       0x4553f0

// block 0045629a
0045629a  movzx      ecx, byte ptr [esi + 8]
0045629e  movzx      edx, byte ptr [edi + 0x3bf]
004562a5  push       ecx
004562a6  push       edx
004562a7  mov        ecx, eax
004562a9  push       0
004562ab  mov        eax, edi
004562ad  call       0x4599b0

// block 004562b2
004562b2  movzx      ecx, word ptr [esi + 2]
004562b6  add        ecx, esi
004562b8  mov        dword ptr [edi + 0x3f0], ecx
004562be  jmp        0x455686

// block 004562c3
004562c3  mov        eax, dword ptr [esi + 8]
004562c6  shl        eax, 5
004562c9  xor        eax, dword ptr [edi + 0x47c]
004562cf  and        eax, 0xe0
004562d4  xor        dword ptr [edi + 0x47c], eax
004562da  movzx      ecx, word ptr [esi + 2]
004562de  add        ecx, esi
004562e0  mov        dword ptr [edi + 0x3f0], ecx
004562e6  jmp        0x455686

// block 004562eb
004562eb  test       dword ptr [edi + 0x47c], 0x200
004562f5  fld        dword ptr [esi + 0x10]
004562f8  jne        0x45638a

// block 004562fe
004562fe  test       byte ptr [esi + 6], 4
00456302  je         0x45630f

// block 00456304
00456304  push       ecx
00456305  mov        eax, edi
00456307  fstp       dword ptr [esp]
0045630a  call       0x4551b0

// block 0045630f
0045630f  test       byte ptr [esi + 6], 2
00456313  fstp       dword ptr [esp + 0xc]
00456317  fld        dword ptr [esi + 0xc]
0045631a  je         0x456327

// block 0045631c
0045631c  push       ecx
0045631d  mov        eax, edi
0045631f  fstp       dword ptr [esp]
00456322  call       0x4551b0

// block 00456327
00456327  test       byte ptr [esi + 6], 1
0045632b  fstp       dword ptr [esp + 0x14]
0045632f  fld        dword ptr [esi + 8]
00456332  je         0x45633f

// block 00456334
00456334  push       ecx
00456335  mov        eax, edi
00456337  fstp       dword ptr [esp]
0045633a  call       0x4551b0

// block 0045633f
0045633f  fstp       dword ptr [esp + 0x10]
00456343  fld        dword ptr [esp + 0x10]
00456347  fstp       dword ptr [esp + 0x70]
0045634b  mov        ecx, dword ptr [esp + 0x70]
0045634f  fld        dword ptr [esp + 0x14]
00456353  mov        dword ptr [edi + 0x424], ecx
00456359  fstp       dword ptr [esp + 0x74]
0045635d  mov        edx, dword ptr [esp + 0x74]
00456361  fld        dword ptr [esp + 0xc]
00456365  mov        dword ptr [edi + 0x428], edx
0045636b  fstp       dword ptr [esp + 0x78]
0045636f  mov        eax, dword ptr [esp + 0x78]
00456373  mov        dword ptr [edi + 0x42c], eax
00456379  movzx      ecx, word ptr [esi + 2]
0045637d  add        ecx, esi
0045637f  mov        dword ptr [edi + 0x3f0], ecx
00456385  jmp        0x455686

// block 0045638a
0045638a  test       byte ptr [esi + 6], 4
0045638e  je         0x45639b

// block 00456390
00456390  push       ecx
00456391  mov        eax, edi
00456393  fstp       dword ptr [esp]
00456396  call       0x4551b0

// block 0045639b
0045639b  test       byte ptr [esi + 6], 2
0045639f  fstp       dword ptr [esp + 0x14]
004563a3  fld        dword ptr [esi + 0xc]
004563a6  je         0x4563b3

// block 004563a8
004563a8  push       ecx
004563a9  mov        eax, edi
004563ab  fstp       dword ptr [esp]
004563ae  call       0x4551b0

// block 004563b3
004563b3  test       byte ptr [esi + 6], 1
004563b7  fstp       dword ptr [esp + 0x10]
004563bb  fld        dword ptr [esi + 8]
004563be  je         0x4563cb

// block 004563c0
004563c0  push       ecx
004563c1  mov        eax, edi
004563c3  fstp       dword ptr [esp]
004563c6  call       0x4551b0

// block 004563cb
004563cb  fstp       dword ptr [esp + 0xc]
004563cf  fld        dword ptr [esp + 0xc]
004563d3  fstp       dword ptr [esp + 0x88]
004563da  mov        ecx, dword ptr [esp + 0x88]
004563e1  fld        dword ptr [esp + 0x10]
004563e5  mov        dword ptr [edi + 0x43c], ecx
004563eb  fstp       dword ptr [esp + 0x8c]
004563f2  mov        edx, dword ptr [esp + 0x8c]
004563f9  fld        dword ptr [esp + 0x14]
004563fd  mov        dword ptr [edi + 0x440], edx
00456403  fstp       dword ptr [esp + 0x90]
0045640a  mov        eax, dword ptr [esp + 0x90]
00456411  mov        dword ptr [edi + 0x444], eax
00456417  movzx      ecx, word ptr [esi + 2]
0045641b  add        ecx, esi
0045641d  mov        dword ptr [edi + 0x3f0], ecx
00456423  jmp        0x455686

// block 00456428
00456428  test       byte ptr [esi + 6], 1
0045642c  mov        eax, dword ptr [esi + 8]
0045642f  je         0x456438

// block 00456431
00456431  mov        ecx, edi
00456433  call       0x4553f0

// block 00456438
00456438  lea        esi, [edi + 0x68]
0045643b  call       0x43adb0

// block 00456440
00456440  mov        esi, dword ptr [esp + 0x14]
00456444  movzx      ecx, word ptr [esi + 2]
00456448  add        ecx, esi
0045644a  mov        dword ptr [edi + 0x3f0], ecx
00456450  jmp        0x455686

// block 00456455
00456455  or         dword ptr [edi + 0x47c], 0x2000

// block 0045645f
0045645f  fld        dword ptr [0x4a3da8]
00456465  push       ecx
00456466  lea        esi, [edi + 0x68]
00456469  fstp       dword ptr [esp]
0045646c  call       0x464a20

// block 00456471
00456471  fldz       
00456473  fcomp      dword ptr [edi + 0x30]
00456476  fnstsw     ax
00456478  test       ah, 0x44
0045647b  jnp        0x457ca2

// block 00456481
00456481  fld        dword ptr [edi + 0x30]
00456484  sub        esp, 8
00456487  fmul       dword ptr [0x4b2ed0]
0045648d  fstp       dword ptr [esp + 0x20]
00456491  fld        dword ptr [esp + 0x20]
00456495  fstp       dword ptr [esp + 4]
00456499  fld        dword ptr [edi + 0x24]
0045649c  fstp       dword ptr [esp]
0045649f  call       0x464640

// block 004564a4
004564a4  mov        esi, 4
004564a9  fstp       dword ptr [edi + 0x24]
004564ac  or         dword ptr [edi + 0x47c], esi
004564b2  jmp        0x457ca7

// block 004564b7
004564b7  mov        ecx, dword ptr [esi + 8]
004564ba  xor        ecx, dword ptr [edi + 0x47c]
004564c0  and        ecx, 1
004564c3  xor        dword ptr [edi + 0x47c], ecx
004564c9  movzx      ecx, word ptr [esi + 2]
004564cd  add        ecx, esi
004564cf  mov        dword ptr [edi + 0x3f0], ecx
004564d5  jmp        0x455686

// block 004564da
004564da  movzx      edx, word ptr [esi + 8]
004564de  shl        edx, 0x13
004564e1  xor        edx, dword ptr [edi + 0x47c]
004564e7  and        edx, 0x180000
004564ed  xor        dword ptr [edi + 0x47c], edx
004564f3  movzx      ecx, word ptr [esi + 0xa]
004564f7  mov        eax, dword ptr [edi + 0x47c]
004564fd  shl        ecx, 0x15
00456500  xor        ecx, eax
00456502  and        ecx, 0x600000
00456508  xor        ecx, eax
0045650a  mov        dword ptr [edi + 0x47c], ecx
00456510  movzx      ecx, word ptr [esi + 2]
00456514  add        ecx, esi
00456516  mov        dword ptr [edi + 0x3f0], ecx
0045651c  jmp        0x455686

// block 00456521
00456521  test       byte ptr [esi + 6], 1
00456525  fld        dword ptr [esi + 8]
00456528  je         0x456535

// block 0045652a
0045652a  push       ecx
0045652b  mov        eax, edi
0045652d  fstp       dword ptr [esp]
00456530  call       0x4551b0

// block 00456535
00456535  fstp       dword ptr [esp + 0xc]
00456539  fld        dword ptr [esp + 0xc]
0045653d  fstp       dword ptr [edi + 0x2f4]
00456543  movzx      ecx, word ptr [esi + 2]
00456547  add        ecx, esi
00456549  mov        dword ptr [edi + 0x3f0], ecx
0045654f  jmp        0x455686

// block 00456554
00456554  test       byte ptr [esi + 6], 1
00456558  fld        dword ptr [esi + 8]
0045655b  je         0x456568

// block 0045655d
0045655d  push       ecx
0045655e  mov        eax, edi
00456560  fstp       dword ptr [esp]
00456563  call       0x4551b0

// block 00456568
00456568  fstp       dword ptr [esp + 0xc]
0045656c  fld        dword ptr [esp + 0xc]
00456570  fstp       dword ptr [edi + 0x2f8]
00456576  movzx      ecx, word ptr [esi + 2]
0045657a  add        ecx, esi
0045657c  mov        dword ptr [edi + 0x3f0], ecx
00456582  jmp        0x455686

// block 00456587
00456587  mov        edx, dword ptr [esi + 8]
0045658a  shl        edx, 0xc
0045658d  xor        edx, dword ptr [edi + 0x47c]
00456593  and        edx, 0x1000
00456599  xor        dword ptr [edi + 0x47c], edx
0045659f  movzx      ecx, word ptr [esi + 2]
004565a3  add        ecx, esi
004565a5  mov        dword ptr [edi + 0x3f0], ecx
004565ab  jmp        0x455686

// block 004565b0
004565b0  mov        eax, dword ptr [esi + 8]
004565b3  shl        eax, 0xe
004565b6  xor        eax, dword ptr [edi + 0x47c]
004565bc  and        eax, 0x4000
004565c1  xor        dword ptr [edi + 0x47c], eax
004565c7  movzx      ecx, word ptr [esi + 2]
004565cb  add        ecx, esi
004565cd  mov        dword ptr [edi + 0x3f0], ecx
004565d3  jmp        0x455686

// block 004565d8
004565d8  mov        ecx, dword ptr [esi + 8]
004565db  add        ecx, ecx
004565dd  xor        ecx, dword ptr [edi + 0x480]
004565e3  and        ecx, 2
004565e6  xor        dword ptr [edi + 0x480], ecx
004565ec  movzx      ecx, word ptr [esi + 2]
004565f0  add        ecx, esi
004565f2  mov        dword ptr [edi + 0x3f0], ecx
004565f8  jmp        0x455686

// block 004565fd
004565fd  test       byte ptr [esi + 6], 1
00456601  mov        eax, dword ptr [esi + 8]
00456604  je         0x45660d

// block 00456606
00456606  mov        ecx, edi
00456608  call       0x4553f0

// block 0045660d
0045660d  test       dword ptr [edi + 0x47c], 0x200
00456617  mov        dword ptr [edi + 0xe0], eax
0045661d  mov        edx, dword ptr [0x4ce8d0]
00456623  mov        dword ptr [edi + 0xb4], edx
00456629  mov        eax, dword ptr [0x4ce8d4]
0045662e  mov        dword ptr [edi + 0xb8], eax
00456634  mov        ecx, dword ptr [0x4ce8d8]
0045663a  mov        dword ptr [edi + 0xbc], ecx
00456640  mov        edx, dword ptr [0x4ce8d0]
00456646  mov        dword ptr [edi + 0xc0], edx
0045664c  mov        eax, dword ptr [0x4ce8d4]
00456651  mov        dword ptr [edi + 0xc4], eax
00456657  mov        ecx, dword ptr [0x4ce8d8]
0045665d  mov        dword ptr [edi + 0xc8], ecx
00456663  mov        edx, dword ptr [esi + 0xc]
00456666  mov        dword ptr [edi + 0xe4], edx
0045666c  lea        ebx, [edi + 0x9c]
00456672  jne        0x456688

// block 00456674
00456674  mov        eax, dword ptr [edi + 0x424]
0045667a  mov        ecx, dword ptr [edi + 0x428]
00456680  mov        edx, dword ptr [edi + 0x42c]
00456686  jmp        0x45669a

// block 00456688
00456688  mov        eax, dword ptr [edi + 0x43c]
0045668e  mov        ecx, dword ptr [edi + 0x440]
00456694  mov        edx, dword ptr [edi + 0x444]

// block 0045669a
0045669a  mov        dword ptr [ebx], eax
0045669c  mov        dword ptr [ebx + 4], ecx
0045669f  mov        dword ptr [ebx + 8], edx
004566a2  fld        dword ptr [esi + 0x18]
004566a5  test       byte ptr [esi + 6], 0x10
004566a9  je         0x4566b6

// block 004566ab
004566ab  push       ecx
004566ac  mov        eax, edi
004566ae  fstp       dword ptr [esp]
004566b1  call       0x4551b0

// block 004566b6
004566b6  test       byte ptr [esi + 6], 8
004566ba  fstp       dword ptr [esp + 0x14]
004566be  fld        dword ptr [esi + 0x14]
004566c1  je         0x4566ce

// block 004566c3
004566c3  push       ecx
004566c4  mov        eax, edi
004566c6  fstp       dword ptr [esp]
004566c9  call       0x4551b0

// block 004566ce
004566ce  test       byte ptr [esi + 6], 4
004566d2  fstp       dword ptr [esp + 0x10]
004566d6  fld        dword ptr [esi + 0x10]
004566d9  je         0x4566e6

// block 004566db
004566db  push       ecx
004566dc  mov        eax, edi
004566de  fstp       dword ptr [esp]
004566e1  call       0x4551b0

// block 004566e6
004566e6  fstp       dword ptr [esp + 0xc]
004566ea  fld        dword ptr [esp + 0xc]
004566ee  fstp       dword ptr [esp + 0x7c]
004566f2  mov        eax, dword ptr [esp + 0x7c]
004566f6  fld        dword ptr [esp + 0x10]
004566fa  mov        dword ptr [edi + 0xa8], eax
00456700  fstp       dword ptr [esp + 0x80]
00456707  mov        ecx, dword ptr [esp + 0x80]
0045670e  fld        dword ptr [esp + 0x14]
00456712  mov        dword ptr [edi + 0xac], ecx
00456718  fstp       dword ptr [esp + 0x84]
0045671f  mov        edx, dword ptr [esp + 0x84]
00456726  mov        eax, ebx
00456728  mov        dword ptr [edi + 0xb0], edx
0045672e  call       0x406340

// block 00456733
00456733  movzx      ecx, word ptr [esi + 2]
00456737  add        ecx, esi
00456739  mov        dword ptr [edi + 0x3f0], ecx
0045673f  jmp        0x455686

// block 00456744
00456744  test       byte ptr [esi + 6], 2
00456748  fld        dword ptr [esi + 0xc]
0045674b  je         0x456758

// block 0045674d
0045674d  push       ecx
0045674e  mov        eax, edi
00456750  fstp       dword ptr [esp]
00456753  call       0x4551b0

// block 00456758
00456758  test       byte ptr [esi + 6], 4
0045675c  fstp       dword ptr [esp + 0x54]
00456760  fld        dword ptr [esi + 0x10]
00456763  je         0x456770

// block 00456765
00456765  push       ecx
00456766  mov        eax, edi
00456768  fstp       dword ptr [esp]
0045676b  call       0x4551b0

// block 00456770
00456770  mov        ebx, 8
00456775  fstp       dword ptr [esp + 0x58]
00456779  fld        dword ptr [esi + 0x14]
0045677c  test       byte ptr [esi + 6], bl
0045677f  je         0x45678c

// block 00456781
00456781  push       ecx
00456782  mov        eax, edi
00456784  fstp       dword ptr [esp]
00456787  call       0x4551b0

// block 0045678c
0045678c  test       byte ptr [esi + 6], 0x80
00456790  fstp       dword ptr [esp + 0x5c]
00456794  fld        dword ptr [esi + 0x24]
00456797  je         0x4567a4

// block 00456799
00456799  push       ecx
0045679a  mov        eax, edi
0045679c  fstp       dword ptr [esp]
0045679f  call       0x4551b0

// block 004567a4
004567a4  mov        eax, 0x100
004567a9  fstp       dword ptr [esp + 0x60]
004567ad  fld        dword ptr [esi + 0x28]
004567b0  test       word ptr [esi + 6], ax
004567b4  je         0x4567c1

// block 004567b6
004567b6  push       ecx
004567b7  mov        eax, edi
004567b9  fstp       dword ptr [esp]
004567bc  call       0x4551b0

// block 004567c1
004567c1  mov        ecx, 0x200
004567c6  fstp       dword ptr [esp + 0x64]
004567ca  fld        dword ptr [esi + 0x2c]
004567cd  test       word ptr [esi + 6], cx
004567d1  je         0x4567de

// block 004567d3
004567d3  push       ecx
004567d4  mov        eax, edi
004567d6  fstp       dword ptr [esp]
004567d9  call       0x4551b0

// block 004567de
004567de  test       byte ptr [esi + 6], 1
004567e2  fstp       dword ptr [esp + 0x68]
004567e6  mov        eax, dword ptr [esi + 8]
004567e9  je         0x4567f2

// block 004567eb
004567eb  mov        ecx, edi
004567ed  call       0x4553f0

// block 004567f2
004567f2  test       dword ptr [edi + 0x47c], 0x200
004567fc  mov        edx, dword ptr [esp + 0x54]
00456800  mov        ecx, dword ptr [esp + 0x5c]
00456804  mov        dword ptr [edi + 0xe0], eax
0045680a  mov        eax, dword ptr [esp + 0x58]
0045680e  mov        dword ptr [edi + 0xb4], edx
00456814  mov        edx, dword ptr [esp + 0x60]
00456818  mov        dword ptr [edi + 0xb8], eax
0045681e  mov        eax, dword ptr [esp + 0x64]
00456822  mov        dword ptr [edi + 0xbc], ecx
00456828  mov        ecx, dword ptr [esp + 0x68]
0045682c  mov        dword ptr [edi + 0xc0], edx
00456832  mov        dword ptr [edi + 0xc4], eax
00456838  mov        dword ptr [edi + 0xe4], ebx
0045683e  mov        dword ptr [edi + 0xc8], ecx
00456844  lea        ebx, [edi + 0x9c]
0045684a  jne        0x456860

// block 0045684c
0045684c  mov        edx, dword ptr [edi + 0x424]
00456852  mov        eax, dword ptr [edi + 0x428]
00456858  mov        ecx, dword ptr [edi + 0x42c]
0045685e  jmp        0x456872

// block 00456860
00456860  mov        edx, dword ptr [edi + 0x43c]
00456866  mov        eax, dword ptr [edi + 0x440]
0045686c  mov        ecx, dword ptr [edi + 0x444]

// block 00456872
00456872  mov        dword ptr [ebx], edx
00456874  mov        dword ptr [ebx + 4], eax
00456877  mov        dword ptr [ebx + 8], ecx
0045687a  fld        dword ptr [esi + 0x20]
0045687d  test       byte ptr [esi + 6], 0x40
00456881  je         0x45688e

// block 00456883
00456883  push       ecx
00456884  mov        eax, edi
00456886  fstp       dword ptr [esp]
00456889  call       0x4551b0

// block 0045688e
0045688e  test       byte ptr [esi + 6], 0x20
00456892  fstp       dword ptr [esp + 0x14]
00456896  fld        dword ptr [esi + 0x1c]
00456899  je         0x4568a6

// block 0045689b
0045689b  push       ecx
0045689c  mov        eax, edi
0045689e  fstp       dword ptr [esp]
004568a1  call       0x4551b0

// block 004568a6
004568a6  test       byte ptr [esi + 6], 0x10
004568aa  fstp       dword ptr [esp + 0x10]
004568ae  fld        dword ptr [esi + 0x18]
004568b1  je         0x4568be

// block 004568b3
004568b3  push       ecx
004568b4  mov        eax, edi
004568b6  fstp       dword ptr [esp]
004568b9  call       0x4551b0

// block 004568be
004568be  fstp       dword ptr [esp + 0xc]
004568c2  fld        dword ptr [esp + 0xc]
004568c6  fstp       dword ptr [esp + 0x40]
004568ca  mov        edx, dword ptr [esp + 0x40]
004568ce  fld        dword ptr [esp + 0x10]
004568d2  mov        dword ptr [edi + 0xa8], edx
004568d8  fstp       dword ptr [esp + 0x44]
004568dc  mov        eax, dword ptr [esp + 0x44]
004568e0  fld        dword ptr [esp + 0x14]
004568e4  mov        dword ptr [edi + 0xac], eax
004568ea  fstp       dword ptr [esp + 0x48]
004568ee  mov        ecx, dword ptr [esp + 0x48]
004568f2  mov        eax, ebx
004568f4  mov        dword ptr [edi + 0xb0], ecx
004568fa  call       0x406340

// block 004568ff
004568ff  movzx      ecx, word ptr [esi + 2]
00456903  add        ecx, esi
00456905  mov        dword ptr [edi + 0x3f0], ecx
0045690b  jmp        0x455686

// block 00456910
00456910  test       byte ptr [esi + 6], 0x10
00456914  mov        dl, byte ptr [edi + 0x3bc]
0045691a  mov        al, byte ptr [edi + 0x3bd]
00456920  mov        cl, byte ptr [edi + 0x3be]
00456926  mov        byte ptr [esp + 0x4c], dl
0045692a  mov        byte ptr [esp + 0x4d], al
0045692e  mov        byte ptr [esp + 0x4e], cl
00456932  je         0x456944

// block 00456934
00456934  mov        eax, dword ptr [esi + 0x18]
00456937  mov        ecx, edi
00456939  call       0x4553f0

// block 0045693e
0045693e  mov        dword ptr [esp + 0xc], eax
00456942  jmp        0x45694b

// block 00456944
00456944  mov        edx, dword ptr [esi + 0x18]
00456947  mov        dword ptr [esp + 0xc], edx

// block 0045694b
0045694b  test       byte ptr [esi + 6], 8
0045694f  je         0x45695f

// block 00456951
00456951  mov        eax, dword ptr [esi + 0x14]
00456954  mov        ecx, edi
00456956  call       0x4553f0

// block 0045695b
0045695b  mov        ebx, eax
0045695d  jmp        0x456962

// block 0045695f
0045695f  mov        ebx, dword ptr [esi + 0x14]

// block 00456962
00456962  test       byte ptr [esi + 6], 4
00456966  mov        eax, dword ptr [esi + 0x10]
00456969  je         0x456972

// block 0045696b
0045696b  mov        ecx, edi
0045696d  call       0x4553f0

// block 00456972
00456972  test       byte ptr [esi + 6], 1
00456976  mov        cl, byte ptr [esp + 0xc]
0045697a  mov        byte ptr [esp + 0x52], al
0045697e  mov        eax, dword ptr [esi + 8]
00456981  mov        byte ptr [esp + 0x50], cl
00456985  mov        byte ptr [esp + 0x51], bl
00456989  je         0x456992

// block 0045698b
0045698b  mov        ecx, edi
0045698d  call       0x4553f0

// block 00456992
00456992  movzx      edx, byte ptr [esi + 0xc]
00456996  push       edx
00456997  push       eax
00456998  lea        ecx, [esp + 0x58]
0045699c  lea        edx, [esp + 0x54]
004569a0  mov        eax, edi
004569a2  call       0x459830

// block 004569a7
004569a7  movzx      ecx, word ptr [esi + 2]
004569ab  add        ecx, esi
004569ad  mov        dword ptr [edi + 0x3f0], ecx
004569b3  jmp        0x455686

// block 004569b8
004569b8  test       byte ptr [esi + 6], 4
004569bc  je         0x4569cc

// block 004569be
004569be  mov        eax, dword ptr [esi + 0x10]
004569c1  mov        ecx, edi
004569c3  call       0x4553f0

// block 004569c8
004569c8  mov        ebx, eax
004569ca  jmp        0x4569cf

// block 004569cc
004569cc  mov        ebx, dword ptr [esi + 0x10]

// block 004569cf
004569cf  test       byte ptr [esi + 6], 1
004569d3  mov        eax, dword ptr [esi + 8]
004569d6  je         0x4569df

// block 004569d8
004569d8  mov        ecx, edi
004569da  call       0x4553f0

// block 004569df
004569df  movzx      ecx, byte ptr [edi + 0x3bf]
004569e6  movzx      edx, byte ptr [esi + 0xc]
004569ea  push       ebx
004569eb  push       ecx
004569ec  mov        ecx, eax
004569ee  push       edx
004569ef  mov        eax, edi
004569f1  call       0x4599b0

// block 004569f6
004569f6  movzx      ecx, word ptr [esi + 2]
004569fa  add        ecx, esi
004569fc  mov        dword ptr [edi + 0x3f0], ecx
00456a02  jmp        0x455686

// block 00456a07
00456a07  test       byte ptr [esi + 6], 0x10
00456a0b  mov        al, byte ptr [edi + 0x3c0]
00456a11  mov        cl, byte ptr [edi + 0x3c1]
00456a17  mov        dl, byte ptr [edi + 0x3c2]
00456a1d  mov        byte ptr [esp + 0x1c], al
00456a21  mov        eax, dword ptr [esi + 0x18]
00456a24  mov        byte ptr [esp + 0x1d], cl
00456a28  mov        byte ptr [esp + 0x1e], dl
00456a2c  je         0x456a35

// block 00456a2e
00456a2e  mov        ecx, edi
00456a30  call       0x4553f0

// block 00456a35
00456a35  test       byte ptr [esi + 6], 8
00456a39  mov        dword ptr [esp + 0xc], eax
00456a3d  je         0x456a4d

// block 00456a3f
00456a3f  mov        eax, dword ptr [esi + 0x14]
00456a42  mov        ecx, edi
00456a44  call       0x4553f0

// block 00456a49
00456a49  mov        ebx, eax
00456a4b  jmp        0x456a50

// block 00456a4d
00456a4d  mov        ebx, dword ptr [esi + 0x14]

// block 00456a50
00456a50  test       byte ptr [esi + 6], 4
00456a54  mov        eax, dword ptr [esi + 0x10]
00456a57  je         0x456a60

// block 00456a59
00456a59  mov        ecx, edi
00456a5b  call       0x4553f0

// block 00456a60
00456a60  test       byte ptr [esi + 6], 1
00456a64  mov        cl, byte ptr [esp + 0xc]
00456a68  mov        byte ptr [esp + 0x3a], al
00456a6c  mov        eax, dword ptr [esi + 8]
00456a6f  mov        byte ptr [esp + 0x38], cl
00456a73  mov        byte ptr [esp + 0x39], bl
00456a77  je         0x456a80

// block 00456a79
00456a79  mov        ecx, edi
00456a7b  call       0x4553f0

// block 00456a80
00456a80  movzx      edx, byte ptr [esi + 0xc]
00456a84  push       edx
00456a85  push       eax
00456a86  lea        ecx, [esp + 0x40]
00456a8a  lea        edx, [esp + 0x24]
00456a8e  mov        eax, edi
00456a90  call       0x459640

// block 00456a95
00456a95  movzx      ecx, word ptr [esi + 2]
00456a99  add        ecx, esi
00456a9b  mov        dword ptr [edi + 0x3f0], ecx
00456aa1  jmp        0x455686

// block 00456aa6
00456aa6  test       byte ptr [esi + 6], 4
00456aaa  je         0x456aba

// block 00456aac
00456aac  mov        eax, dword ptr [esi + 0x10]
00456aaf  mov        ecx, edi
00456ab1  call       0x4553f0

// block 00456ab6
00456ab6  mov        ebx, eax
00456ab8  jmp        0x456abd

// block 00456aba
00456aba  mov        ebx, dword ptr [esi + 0x10]

// block 00456abd
00456abd  test       byte ptr [esi + 6], 1
00456ac1  mov        eax, dword ptr [esi + 8]
00456ac4  je         0x456acd

// block 00456ac6
00456ac6  mov        ecx, edi
00456ac8  call       0x4553f0

// block 00456acd
00456acd  movzx      ecx, byte ptr [edi + 0x3c3]
00456ad4  mov        dl, byte ptr [esi + 0xc]
00456ad7  push       ebx
00456ad8  push       ecx
00456ad9  mov        ecx, eax
00456adb  mov        eax, edi
00456add  call       0x4595c0

// block 00456ae2
00456ae2  movzx      ecx, word ptr [esi + 2]
00456ae6  add        ecx, esi
00456ae8  mov        dword ptr [edi + 0x3f0], ecx
00456aee  jmp        0x455686

// block 00456af3
00456af3  test       byte ptr [esi + 6], 0x10
00456af7  fld        dword ptr [esi + 0x18]
00456afa  je         0x456b07

// block 00456afc
00456afc  push       ecx
00456afd  mov        eax, edi
00456aff  fstp       dword ptr [esp]
00456b02  call       0x4551b0

// block 00456b07
00456b07  test       byte ptr [esi + 6], 8
00456b0b  fstp       dword ptr [esp + 0x14]
00456b0f  fld        dword ptr [esi + 0x14]
00456b12  je         0x456b1f

// block 00456b14
00456b14  push       ecx
00456b15  mov        eax, edi
00456b17  fstp       dword ptr [esp]
00456b1a  call       0x4551b0

// block 00456b1f
00456b1f  mov        ebx, 4
00456b24  fstp       dword ptr [esp + 0x10]
00456b28  fld        dword ptr [esi + 0x10]
00456b2b  test       byte ptr [esi + 6], bl
00456b2e  je         0x456b3b

// block 00456b30
00456b30  push       ecx
00456b31  mov        eax, edi
00456b33  fstp       dword ptr [esp]
00456b36  call       0x4551b0

// block 00456b3b
00456b3b  test       byte ptr [esi + 6], 1
00456b3f  fstp       dword ptr [esp + 0xc]
00456b43  fld        dword ptr [esp + 0xc]
00456b47  mov        eax, dword ptr [esi + 8]
00456b4a  fstp       dword ptr [esp + 0x20]
00456b4e  fld        dword ptr [esp + 0x10]
00456b52  fstp       dword ptr [esp + 0x24]
00456b56  fld        dword ptr [esp + 0x14]
00456b5a  fstp       dword ptr [esp + 0x28]
00456b5e  je         0x456b67

// block 00456b60
00456b60  mov        ecx, edi
00456b62  call       0x4553f0

// block 00456b67
00456b67  mov        dword ptr [edi + 0x1a4], eax
00456b6d  mov        edx, dword ptr [0x4ce8d0]
00456b73  mov        dword ptr [edi + 0x178], edx
00456b79  mov        eax, dword ptr [0x4ce8d4]
00456b7e  mov        dword ptr [edi + 0x17c], eax
00456b84  mov        ecx, dword ptr [0x4ce8d8]
00456b8a  mov        dword ptr [edi + 0x180], ecx
00456b90  mov        edx, dword ptr [0x4ce8d0]
00456b96  mov        dword ptr [edi + 0x184], edx
00456b9c  mov        eax, dword ptr [0x4ce8d4]
00456ba1  mov        dword ptr [edi + 0x188], eax
00456ba7  mov        ecx, dword ptr [0x4ce8d8]
00456bad  mov        dword ptr [edi + 0x18c], ecx
00456bb3  mov        edx, dword ptr [esi + 0xc]
00456bb6  mov        ecx, dword ptr [edi + 0x24]
00456bb9  mov        dword ptr [edi + 0x1a8], edx
00456bbf  mov        edx, dword ptr [edi + 0x28]
00456bc2  lea        eax, [edi + 0x160]
00456bc8  mov        dword ptr [eax], ecx
00456bca  mov        ecx, dword ptr [edi + 0x2c]
00456bcd  mov        dword ptr [eax + 4], edx
00456bd0  mov        edx, dword ptr [esp + 0x20]
00456bd4  mov        dword ptr [edi + 0x16c], edx
00456bda  mov        edx, dword ptr [esp + 0x28]
00456bde  mov        dword ptr [eax + 8], ecx
00456be1  mov        ecx, dword ptr [esp + 0x24]
00456be5  mov        dword ptr [edi + 0x170], ecx
00456beb  mov        dword ptr [edi + 0x174], edx
00456bf1  call       0x406340

// block 00456bf6
00456bf6  or         dword ptr [edi + 0x47c], ebx
00456bfc  movzx      ecx, word ptr [esi + 2]
00456c00  add        ecx, esi
00456c02  mov        dword ptr [edi + 0x3f0], ecx
00456c08  jmp        0x455686

// block 00456c0d
00456c0d  fld        dword ptr [esi + 0x14]
00456c10  mov        ebx, 8
00456c15  test       byte ptr [esi + 6], bl
00456c18  je         0x456c25

// block 00456c1a
00456c1a  push       ecx
00456c1b  mov        eax, edi
00456c1d  fstp       dword ptr [esp]
00456c20  call       0x4551b0

// block 00456c25
00456c25  test       byte ptr [esi + 6], 4
00456c29  fstp       dword ptr [esp + 0x10]
00456c2d  fld        dword ptr [esi + 0x10]
00456c30  je         0x456c3d

// block 00456c32
00456c32  push       ecx
00456c33  mov        eax, edi
00456c35  fstp       dword ptr [esp]
00456c38  call       0x4551b0

// block 00456c3d
00456c3d  test       byte ptr [esi + 6], 1
00456c41  fstp       dword ptr [esp + 0xc]
00456c45  fld        dword ptr [esp + 0xc]
00456c49  mov        eax, dword ptr [esi + 8]
00456c4c  fstp       dword ptr [esp + 0x94]
00456c53  fld        dword ptr [esp + 0x10]
00456c57  fstp       dword ptr [esp + 0x98]
00456c5e  je         0x456c67

// block 00456c60
00456c60  mov        ecx, edi
00456c62  call       0x4553f0

// block 00456c67
00456c67  movzx      ecx, byte ptr [esi + 0xc]
00456c6b  push       ecx
00456c6c  push       eax
00456c6d  lea        edx, [edi + 0x40]
00456c70  lea        ecx, [esp + 0x9c]
00456c77  mov        eax, edi
00456c79  call       0x425790

// block 00456c7e
00456c7e  or         dword ptr [edi + 0x47c], ebx
00456c84  movzx      ecx, word ptr [esi + 2]
00456c88  add        ecx, esi
00456c8a  mov        dword ptr [edi + 0x3f0], ecx
00456c90  jmp        0x455686

// block 00456c95
00456c95  test       byte ptr [esi + 6], 8
00456c99  fld        dword ptr [esi + 0x14]
00456c9c  je         0x456ca9

// block 00456c9e
00456c9e  push       ecx
00456c9f  mov        eax, edi
00456ca1  fstp       dword ptr [esp]
00456ca4  call       0x4551b0

// block 00456ca9
00456ca9  test       byte ptr [esi + 6], 4
00456cad  fstp       dword ptr [esp + 0x10]
00456cb1  fld        dword ptr [esi + 0x10]
00456cb4  je         0x456cc1

// block 00456cb6
00456cb6  push       ecx
00456cb7  mov        eax, edi
00456cb9  fstp       dword ptr [esp]
00456cbc  call       0x4551b0

// block 00456cc1
00456cc1  test       byte ptr [esi + 6], 1
00456cc5  fstp       dword ptr [esp + 0xc]
00456cc9  fld        dword ptr [esp + 0xc]
00456ccd  mov        eax, dword ptr [esi + 8]
00456cd0  fstp       dword ptr [esp + 0x2c]
00456cd4  fld        dword ptr [esp + 0x10]
00456cd8  fstp       dword ptr [esp + 0x30]
00456cdc  je         0x456ce5

// block 00456cde
00456cde  mov        ecx, edi
00456ce0  call       0x4553f0

// block 00456ce5
00456ce5  movzx      ecx, byte ptr [esi + 0xc]
00456ce9  push       ecx
00456cea  push       eax
00456ceb  lea        edx, [edi + 0x50]
00456cee  lea        ecx, [esp + 0x34]
00456cf2  mov        eax, edi
00456cf4  call       0x459530

// block 00456cf9
00456cf9  or         dword ptr [edi + 0x47c], 0x10
00456d00  movzx      ecx, word ptr [esi + 2]
00456d04  add        ecx, esi
00456d06  mov        dword ptr [edi + 0x3f0], ecx
00456d0c  jmp        0x455686

// block 00456d11
00456d11  test       byte ptr [esi + 6], 4
00456d15  fld        dword ptr [esi + 0x10]
00456d18  je         0x456d25

// block 00456d1a
00456d1a  push       ecx
00456d1b  mov        eax, edi
00456d1d  fstp       dword ptr [esp]
00456d20  call       0x4551b0

// block 00456d25
00456d25  test       byte ptr [esi + 6], 1
00456d29  fstp       dword ptr [esp + 0xc]
00456d2d  mov        eax, dword ptr [esi + 8]
00456d30  je         0x456d39

// block 00456d32
00456d32  mov        ecx, edi
00456d34  call       0x4553f0

// block 00456d39
00456d39  fldz       
00456d3b  mov        dword ptr [edi + 0x2c0], eax
00456d41  fst        dword ptr [edi + 0x2a4]
00456d47  lea        eax, [edi + 0x29c]
00456d4d  fstp       dword ptr [edi + 0x2a8]
00456d53  mov        edx, dword ptr [esi + 0xc]
00456d56  fld        dword ptr [edi + 0x2f4]
00456d5c  fstp       dword ptr [eax]
00456d5e  mov        dword ptr [edi + 0x2c4], edx
00456d64  fld        dword ptr [esp + 0xc]
00456d68  fstp       dword ptr [edi + 0x2a0]
00456d6e  call       0x4592b0

// block 00456d73
00456d73  movzx      ecx, word ptr [esi + 2]
00456d77  add        ecx, esi
00456d79  mov        dword ptr [edi + 0x3f0], ecx
00456d7f  jmp        0x455686

// block 00456d84
00456d84  test       byte ptr [esi + 6], 4
00456d88  fld        dword ptr [esi + 0x10]
00456d8b  je         0x456d98

// block 00456d8d
00456d8d  push       ecx
00456d8e  mov        eax, edi
00456d90  fstp       dword ptr [esp]
00456d93  call       0x4551b0

// block 00456d98
00456d98  test       byte ptr [esi + 6], 1
00456d9c  fstp       dword ptr [esp + 0xc]
00456da0  mov        eax, dword ptr [esi + 8]
00456da3  je         0x456dac

// block 00456da5
00456da5  mov        ecx, edi
00456da7  call       0x4553f0

// block 00456dac
00456dac  fldz       
00456dae  mov        dword ptr [edi + 0x2ec], eax
00456db4  fst        dword ptr [edi + 0x2d0]
00456dba  fstp       dword ptr [edi + 0x2d4]
00456dc0  mov        eax, dword ptr [esi + 0xc]
00456dc3  fld        dword ptr [edi + 0x2f8]
00456dc9  mov        dword ptr [edi + 0x2f0], eax
00456dcf  fstp       dword ptr [edi + 0x2c8]
00456dd5  fld        dword ptr [esp + 0xc]
00456dd9  lea        eax, [edi + 0x2c8]
00456ddf  fstp       dword ptr [edi + 0x2cc]
00456de5  call       0x4592b0

// block 00456dea
00456dea  movzx      ecx, word ptr [esi + 2]
00456dee  add        ecx, esi
00456df0  mov        dword ptr [edi + 0x3f0], ecx
00456df6  jmp        0x455686

// block 00456dfb
00456dfb  mov        ecx, dword ptr [esi + 8]
00456dfe  shl        ecx, 0x17
00456e01  xor        ecx, dword ptr [edi + 0x47c]
00456e07  and        ecx, 0xf800000
00456e0d  xor        dword ptr [edi + 0x47c], ecx
00456e13  mov        eax, dword ptr [edi + 0x47c]
00456e19  and        eax, 0xf800000
00456e1e  cmp        eax, 0x5000000
00456e23  jne        0x457c91

// block 00456e29
00456e29  mov        ebx, edi
00456e2b  call       0x45d9a0

// block 00456e30
00456e30  movzx      ecx, word ptr [esi + 2]
00456e34  add        ecx, esi
00456e36  mov        dword ptr [edi + 0x3f0], ecx
00456e3c  jmp        0x455686

// block 00456e41
00456e41  mov        edx, dword ptr [edi + 0x430]
00456e47  fldz       
00456e49  mov        ecx, dword ptr [edi + 0x438]
00456e4f  mov        eax, dword ptr [edi + 0x434]
00456e55  fst        dword ptr [edi + 0x430]
00456e5b  mov        dword ptr [edi + 0x424], edx
00456e61  fst        dword ptr [edi + 0x434]
00456e67  mov        dword ptr [edi + 0x428], eax
00456e6d  fstp       dword ptr [edi + 0x438]
00456e73  mov        dword ptr [edi + 0x42c], ecx
00456e79  movzx      ecx, word ptr [esi + 2]
00456e7d  add        ecx, esi
00456e7f  mov        dword ptr [edi + 0x3f0], ecx
00456e85  jmp        0x455686

// block 00456e8a
00456e8a  mov        edx, dword ptr [edi + 0x47c]
00456e90  and        edx, 0xf4ffffff
00456e96  or         edx, 0x4800000
00456e9c  mov        dword ptr [edi + 0x47c], edx
00456ea2  test       byte ptr [esi + 6], 1
00456ea6  mov        eax, dword ptr [esi + 8]
00456ea9  je         0x456eb2

// block 00456eab
00456eab  mov        ecx, edi
00456ead  call       0x4553f0

// block 00456eb2
00456eb2  lea        ecx, [eax*8]
00456eb9  sub        ecx, eax
00456ebb  add        ecx, ecx
00456ebd  add        ecx, ecx
00456ebf  add        ecx, ecx
00456ec1  push       ecx
00456ec2  call       0x46d04a

// block 00456ec7
00456ec7  mov        dword ptr [edi + 0x478], eax
00456ecd  movzx      ecx, word ptr [esi + 2]
00456ed1  add        esp, 4
00456ed4  add        ecx, esi
00456ed6  mov        dword ptr [edi + 0x3f0], ecx
00456edc  jmp        0x455686

// block 00456ee1
00456ee1  mov        edx, dword ptr [edi + 0x47c]
00456ee7  and        edx, 0xf6ffffff
00456eed  or         edx, 0x6800000
00456ef3  mov        dword ptr [edi + 0x47c], edx
00456ef9  test       byte ptr [esi + 6], 1
00456efd  mov        eax, dword ptr [esi + 8]
00456f00  je         0x456f09

// block 00456f02
00456f02  mov        ecx, edi
00456f04  call       0x4553f0

// block 00456f09
00456f09  lea        ecx, [eax*8]
00456f10  sub        ecx, eax
00456f12  add        ecx, ecx
00456f14  add        ecx, ecx
00456f16  add        ecx, ecx
00456f18  push       ecx
00456f19  call       0x46d04a

// block 00456f1e
00456f1e  mov        dword ptr [edi + 0x478], eax
00456f24  movzx      ecx, word ptr [esi + 2]
00456f28  add        esp, 4
00456f2b  add        ecx, esi
00456f2d  mov        dword ptr [edi + 0x3f0], ecx
00456f33  jmp        0x455686

// block 00456f38
00456f38  lea        ecx, [esp + 0x9c]
00456f3f  mov        eax, edi
00456f41  call       0x45cdf0

// block 00456f46
00456f46  lea        ecx, [edi + 0x7c]
00456f49  lea        eax, [esp + 0x9c]
00456f50  call       0x4555f0

// block 00456f55
00456f55  lea        ecx, [edi + 0x84]
00456f5b  lea        eax, [esp + 0xa8]
00456f62  call       0x4555f0

// block 00456f67
00456f67  lea        ecx, [edi + 0x8c]
00456f6d  lea        eax, [esp + 0xb4]
00456f74  call       0x4555f0

// block 00456f79
00456f79  lea        ecx, [edi + 0x94]
00456f7f  lea        eax, [esp + 0xc0]
00456f86  call       0x4555f0

// block 00456f8b
00456f8b  movzx      ecx, word ptr [esi + 2]
00456f8f  add        ecx, esi
00456f91  mov        dword ptr [edi + 0x3f0], ecx
00456f97  jmp        0x455686

// block 00456f9c
00456f9c  test       byte ptr [esi + 6], 1
00456fa0  mov        eax, dword ptr [esi + 8]
00456fa3  je         0x456fac

// block 00456fa5
00456fa5  mov        ecx, edi
00456fa7  call       0x4553f0

// block 00456fac
00456fac  shl        eax, 4
00456faf  xor        eax, dword ptr [edi + 0x480]
00456fb5  and        eax, 0x10
00456fb8  jmp        0x457c8b

// block 00456fbd
00456fbd  mov        edx, dword ptr [edi + 0x47c]
00456fc3  and        edx, 0xf77fffff
00456fc9  or         edx, 0x7000000
00456fcf  mov        dword ptr [edi + 0x47c], edx
00456fd5  fld        dword ptr [esi + 8]
00456fd8  test       byte ptr [esi + 6], 1
00456fdc  je         0x456fe9

// block 00456fde
00456fde  push       ecx
00456fdf  mov        eax, edi
00456fe1  fstp       dword ptr [esp]
00456fe4  call       0x4551b0

// block 00456fe9
00456fe9  fstp       dword ptr [esp + 0xc]
00456fed  fld        dword ptr [esp + 0xc]
00456ff1  fstp       dword ptr [edi + 0x58]
00456ff4  test       byte ptr [esi + 6], 2
00456ff8  fld        dword ptr [esi + 0xc]
00456ffb  je         0x457008

// block 00456ffd
00456ffd  push       ecx
00456ffe  mov        eax, edi
00457000  fstp       dword ptr [esp]
00457003  call       0x4551b0

// block 00457008
00457008  fstp       dword ptr [esp + 0xc]
0045700c  fld        dword ptr [esp + 0xc]
00457010  fstp       dword ptr [edi + 0x5c]
00457013  movzx      ecx, word ptr [esi + 2]
00457017  add        ecx, esi
00457019  mov        dword ptr [edi + 0x3f0], ecx
0045701f  jmp        0x455686

// block 00457024
00457024  mov        eax, dword ptr [edi + 0x47c]
0045702a  and        eax, 0xf97fffff
0045702f  or         eax, 0x9000000
00457034  mov        dword ptr [edi + 0x47c], eax
0045703a  fld        dword ptr [esi + 8]
0045703d  test       byte ptr [esi + 6], 1
00457041  je         0x45704e

// block 00457043
00457043  push       ecx
00457044  mov        eax, edi
00457046  fstp       dword ptr [esp]
00457049  call       0x4551b0

// block 0045704e
0045704e  fstp       dword ptr [esp + 0xc]
00457052  fld        dword ptr [esp + 0xc]
00457056  fstp       dword ptr [edi + 0x58]
00457059  test       byte ptr [esi + 6], 2
0045705d  fld        dword ptr [esi + 0xc]
00457060  je         0x45706d

// block 00457062
00457062  push       ecx
00457063  mov        eax, edi
00457065  fstp       dword ptr [esp]
00457068  call       0x4551b0

// block 0045706d
0045706d  fstp       dword ptr [esp + 0xc]
00457071  fld        dword ptr [esp + 0xc]
00457075  fstp       dword ptr [edi + 0x5c]
00457078  movzx      ecx, word ptr [esi + 2]
0045707c  add        ecx, esi
0045707e  mov        dword ptr [edi + 0x3f0], ecx
00457084  jmp        0x455686

// block 00457089
00457089  mov        ecx, dword ptr [edi + 0x47c]
0045708f  and        ecx, 0xf9ffffff
00457095  or         ecx, 0x9800000
0045709b  mov        dword ptr [edi + 0x47c], ecx
004570a1  fld        dword ptr [esi + 8]
004570a4  test       byte ptr [esi + 6], 1
004570a8  je         0x4570b5

// block 004570aa
004570aa  push       ecx
004570ab  mov        eax, edi
004570ad  fstp       dword ptr [esp]
004570b0  call       0x4551b0

// block 004570b5
004570b5  fstp       dword ptr [esp + 0xc]
004570b9  fld        dword ptr [esp + 0xc]
004570bd  fstp       dword ptr [edi + 0x58]
004570c0  test       byte ptr [esi + 6], 2
004570c4  fld        dword ptr [esi + 0xc]
004570c7  je         0x4570d4

// block 004570c9
004570c9  push       ecx
004570ca  mov        eax, edi
004570cc  fstp       dword ptr [esp]
004570cf  call       0x4551b0

// block 004570d4
004570d4  fstp       dword ptr [esp + 0xc]
004570d8  fld        dword ptr [esp + 0xc]
004570dc  fstp       dword ptr [edi + 0x5c]
004570df  movzx      ecx, word ptr [esi + 2]
004570e3  add        ecx, esi
004570e5  mov        dword ptr [edi + 0x3f0], ecx
004570eb  jmp        0x455686

// block 004570f0
004570f0  mov        edx, dword ptr [edi + 0x47c]
004570f6  and        edx, 0xfa7fffff
004570fc  or         edx, 0xa000000
00457102  mov        dword ptr [edi + 0x47c], edx
00457108  fld        dword ptr [esi + 8]
0045710b  test       byte ptr [esi + 6], 1
0045710f  je         0x45711c

// block 00457111
00457111  push       ecx
00457112  mov        eax, edi
00457114  fstp       dword ptr [esp]
00457117  call       0x4551b0

// block 0045711c
0045711c  fstp       dword ptr [esp + 0xc]
00457120  fld        dword ptr [esp + 0xc]
00457124  fstp       dword ptr [edi + 0x58]
00457127  test       byte ptr [esi + 6], 2
0045712b  fld        dword ptr [esi + 0xc]
0045712e  je         0x45713b

// block 00457130
00457130  push       ecx
00457131  mov        eax, edi
00457133  fstp       dword ptr [esp]
00457136  call       0x4551b0

// block 0045713b
0045713b  fstp       dword ptr [esp + 0xc]
0045713f  fld        dword ptr [esp + 0xc]
00457143  fstp       dword ptr [edi + 0x5c]
00457146  movzx      ecx, word ptr [esi + 2]
0045714a  add        ecx, esi
0045714c  mov        dword ptr [edi + 0x3f0], ecx
00457152  jmp        0x455686

// block 00457157
00457157  mov        eax, dword ptr [edi + 0x47c]
0045715d  and        eax, 0xf7ffffff
00457162  or         eax, 0x7800000
00457167  mov        dword ptr [edi + 0x47c], eax
0045716d  fld        dword ptr [esi + 8]
00457170  test       byte ptr [esi + 6], 1
00457174  je         0x457181

// block 00457176
00457176  push       ecx
00457177  mov        eax, edi
00457179  fstp       dword ptr [esp]
0045717c  call       0x4551b0

// block 00457181
00457181  fstp       dword ptr [esp + 0xc]
00457185  fld        dword ptr [esp + 0xc]
00457189  jmp        0x4571bf

// block 0045718b
0045718b  mov        ecx, dword ptr [edi + 0x47c]
00457191  and        ecx, 0xf87fffff
00457197  or         ecx, 0x8000000
0045719d  mov        dword ptr [edi + 0x47c], ecx
004571a3  fld        dword ptr [esi + 8]
004571a6  test       byte ptr [esi + 6], 1
004571aa  je         0x4571b7

// block 004571ac
004571ac  push       ecx
004571ad  mov        eax, edi
004571af  fstp       dword ptr [esp]
004571b2  call       0x4551b0

// block 004571b7
004571b7  fstp       dword ptr [esp + 0xc]
004571bb  fld        dword ptr [esp + 0xc]

// block 004571bf
004571bf  fstp       dword ptr [edi + 0x58]
004571c2  test       byte ptr [esi + 6], 2
004571c6  mov        eax, dword ptr [esi + 0xc]
004571c9  je         0x4571d2

// block 004571cb
004571cb  mov        ecx, edi
004571cd  call       0x4553f0

// block 004571d2
004571d2  mov        dword ptr [edi + 0x3fc], eax
004571d8  movzx      ecx, word ptr [esi + 2]
004571dc  add        ecx, esi
004571de  mov        dword ptr [edi + 0x3f0], ecx
004571e4  jmp        0x455686

// block 004571e9
004571e9  test       byte ptr [esi + 6], 2
004571ed  je         0x4571fd

// block 004571ef
004571ef  mov        eax, dword ptr [esi + 0xc]
004571f2  mov        ecx, edi
004571f4  call       0x4553f0

// block 004571f9
004571f9  mov        ebx, eax
004571fb  jmp        0x457200

// block 004571fd
004571fd  mov        ebx, dword ptr [esi + 0xc]

// block 00457200
00457200  test       byte ptr [esi + 6], 1
00457204  lea        eax, [esi + 8]
00457207  je         0x457969

// block 0045720d
0045720d  mov        edx, edi
0045720f  call       0x455580

// block 00457214
00457214  mov        dword ptr [eax], ebx
00457216  movzx      ecx, word ptr [esi + 2]
0045721a  add        ecx, esi
0045721c  mov        dword ptr [edi + 0x3f0], ecx
00457222  jmp        0x455686

// block 00457227
00457227  test       byte ptr [esi + 6], 2
0045722b  fld        dword ptr [esi + 0xc]
0045722e  je         0x45723b

// block 00457230
00457230  push       ecx
00457231  mov        eax, edi
00457233  fstp       dword ptr [esp]
00457236  call       0x4551b0

// block 0045723b
0045723b  test       byte ptr [esi + 6], 1
0045723f  fstp       dword ptr [esp + 0xc]
00457243  lea        eax, [esi + 8]
00457246  je         0x457259

// block 00457248
00457248  mov        esi, dword ptr [ebp + 8]
0045724b  mov        edi, eax
0045724d  call       0x4554d0

// block 00457252
00457252  mov        esi, dword ptr [esp + 0x14]
00457256  mov        edi, dword ptr [ebp + 8]

// block 00457259
00457259  fld        dword ptr [esp + 0xc]
0045725d  fstp       dword ptr [eax]
0045725f  movzx      ecx, word ptr [esi + 2]
00457263  add        ecx, esi
00457265  mov        dword ptr [edi + 0x3f0], ecx
0045726b  jmp        0x455686

// block 00457270
00457270  test       byte ptr [esi + 6], 2
00457274  je         0x457286

// block 00457276
00457276  mov        eax, dword ptr [esi + 0xc]
00457279  mov        ecx, edi
0045727b  call       0x4553f0

// block 00457280
00457280  mov        dword ptr [esp + 0xc], eax
00457284  jmp        0x45728d

// block 00457286
00457286  mov        edx, dword ptr [esi + 0xc]
00457289  mov        dword ptr [esp + 0xc], edx

// block 0045728d
0045728d  test       byte ptr [esi + 6], 4
00457291  je         0x4572a1

// block 00457293
00457293  mov        eax, dword ptr [esi + 0x10]
00457296  mov        ecx, edi
00457298  call       0x4553f0

// block 0045729d
0045729d  mov        ebx, eax
0045729f  jmp        0x4572a4

// block 004572a1
004572a1  mov        ebx, dword ptr [esi + 0x10]

// block 004572a4
004572a4  test       byte ptr [esi + 6], 1
004572a8  lea        eax, [esi + 8]
004572ab  je         0x4572b4

// block 004572ad
004572ad  mov        edx, edi
004572af  call       0x455580

// block 004572b4
004572b4  mov        ecx, dword ptr [esp + 0xc]
004572b8  add        ebx, ecx
004572ba  mov        dword ptr [eax], ebx
004572bc  movzx      ecx, word ptr [esi + 2]
004572c0  add        ecx, esi
004572c2  mov        dword ptr [edi + 0x3f0], ecx
004572c8  jmp        0x455686

// block 004572cd
004572cd  test       byte ptr [esi + 6], 2
004572d1  fld        dword ptr [esi + 0xc]
004572d4  je         0x4572e1

// block 004572d6
004572d6  push       ecx
004572d7  mov        eax, edi
004572d9  fstp       dword ptr [esp]
004572dc  call       0x4551b0

// block 004572e1
004572e1  test       byte ptr [esi + 6], 4
004572e5  fstp       dword ptr [esp + 0x10]
004572e9  fld        dword ptr [esi + 0x10]
004572ec  je         0x4572f9

// block 004572ee
004572ee  push       ecx
004572ef  mov        eax, edi
004572f1  fstp       dword ptr [esp]
004572f4  call       0x4551b0

// block 004572f9
004572f9  test       byte ptr [esi + 6], 1
004572fd  fstp       dword ptr [esp + 0xc]
00457301  lea        eax, [esi + 8]
00457304  je         0x457317

// block 00457306
00457306  mov        esi, dword ptr [ebp + 8]
00457309  mov        edi, eax
0045730b  call       0x4554d0

// block 00457310
00457310  mov        esi, dword ptr [esp + 0x14]
00457314  mov        edi, dword ptr [ebp + 8]

// block 00457317
00457317  fld        dword ptr [esp + 0xc]
0045731b  fadd       dword ptr [esp + 0x10]
0045731f  fstp       dword ptr [eax]
00457321  movzx      ecx, word ptr [esi + 2]
00457325  add        ecx, esi
00457327  mov        dword ptr [edi + 0x3f0], ecx
0045732d  jmp        0x455686

// block 00457332
00457332  test       byte ptr [esi + 6], 2
00457336  je         0x457346

// block 00457338
00457338  mov        eax, dword ptr [esi + 0xc]
0045733b  mov        ecx, edi
0045733d  call       0x4553f0

// block 00457342
00457342  mov        ebx, eax
00457344  jmp        0x457349

// block 00457346
00457346  mov        ebx, dword ptr [esi + 0xc]

// block 00457349
00457349  test       byte ptr [esi + 6], 4
0045734d  je         0x45735f

// block 0045734f
0045734f  mov        eax, dword ptr [esi + 0x10]
00457352  mov        ecx, edi
00457354  call       0x4553f0

// block 00457359
00457359  mov        dword ptr [esp + 0xc], eax
0045735d  jmp        0x457366

// block 0045735f
0045735f  mov        edx, dword ptr [esi + 0x10]
00457362  mov        dword ptr [esp + 0xc], edx

// block 00457366
00457366  test       byte ptr [esi + 6], 1
0045736a  lea        eax, [esi + 8]
0045736d  je         0x457376

// block 0045736f
0045736f  mov        edx, edi
00457371  call       0x455580

// block 00457376
00457376  sub        ebx, dword ptr [esp + 0xc]
0045737a  mov        dword ptr [eax], ebx
0045737c  movzx      ecx, word ptr [esi + 2]
00457380  add        ecx, esi
00457382  mov        dword ptr [edi + 0x3f0], ecx
00457388  jmp        0x455686

// block 0045738d
0045738d  test       byte ptr [esi + 6], 2
00457391  fld        dword ptr [esi + 0xc]
00457394  je         0x4573a1

// block 00457396
00457396  push       ecx
00457397  mov        eax, edi
00457399  fstp       dword ptr [esp]
0045739c  call       0x4551b0

// block 004573a1
004573a1  test       byte ptr [esi + 6], 4
004573a5  fstp       dword ptr [esp + 0xc]
004573a9  fld        dword ptr [esi + 0x10]
004573ac  je         0x4573b9

// block 004573ae
004573ae  push       ecx
004573af  mov        eax, edi
004573b1  fstp       dword ptr [esp]
004573b4  call       0x4551b0

// block 004573b9
004573b9  test       byte ptr [esi + 6], 1
004573bd  fstp       dword ptr [esp + 0x10]
004573c1  lea        eax, [esi + 8]
004573c4  je         0x4573d7

// block 004573c6
004573c6  mov        esi, dword ptr [ebp + 8]
004573c9  mov        edi, eax
004573cb  call       0x4554d0

// block 004573d0
004573d0  mov        esi, dword ptr [esp + 0x14]
004573d4  mov        edi, dword ptr [ebp + 8]

// block 004573d7
004573d7  fld        dword ptr [esp + 0xc]
004573db  fsub       dword ptr [esp + 0x10]
004573df  fstp       dword ptr [eax]
004573e1  movzx      ecx, word ptr [esi + 2]
004573e5  add        ecx, esi
004573e7  mov        dword ptr [edi + 0x3f0], ecx
004573ed  jmp        0x455686

// block 004573f2
004573f2  test       byte ptr [esi + 6], 2
004573f6  mov        eax, dword ptr [esi + 0xc]
004573f9  je         0x457402

// block 004573fb
004573fb  mov        ecx, edi
004573fd  call       0x4553f0

// block 00457402
00457402  test       byte ptr [esi + 6], 4
00457406  mov        dword ptr [esp + 0xc], eax
0045740a  je         0x45741a

// block 0045740c
0045740c  mov        eax, dword ptr [esi + 0x10]
0045740f  mov        ecx, edi
00457411  call       0x4553f0

// block 00457416
00457416  mov        ebx, eax
00457418  jmp        0x45741d

// block 0045741a
0045741a  mov        ebx, dword ptr [esi + 0x10]

// block 0045741d
0045741d  test       byte ptr [esi + 6], 1
00457421  lea        eax, [esi + 8]
00457424  je         0x45742d

// block 00457426
00457426  mov        edx, edi
00457428  call       0x455580

// block 0045742d
0045742d  imul       ebx, dword ptr [esp + 0xc]
00457432  mov        dword ptr [eax], ebx
00457434  movzx      ecx, word ptr [esi + 2]
00457438  add        ecx, esi
0045743a  mov        dword ptr [edi + 0x3f0], ecx
00457440  jmp        0x455686

// block 00457445
00457445  test       byte ptr [esi + 6], 2
00457449  fld        dword ptr [esi + 0xc]
0045744c  je         0x457459

// block 0045744e
0045744e  push       ecx
0045744f  mov        eax, edi
00457451  fstp       dword ptr [esp]
00457454  call       0x4551b0

// block 00457459
00457459  test       byte ptr [esi + 6], 4
0045745d  fstp       dword ptr [esp + 0x10]
00457461  fld        dword ptr [esi + 0x10]
00457464  je         0x457471

// block 00457466
00457466  push       ecx
00457467  mov        eax, edi
00457469  fstp       dword ptr [esp]
0045746c  call       0x4551b0

// block 00457471
00457471  test       byte ptr [esi + 6], 1
00457475  fstp       dword ptr [esp + 0xc]
00457479  lea        eax, [esi + 8]
0045747c  je         0x45748f

// block 0045747e
0045747e  mov        esi, dword ptr [ebp + 8]
00457481  mov        edi, eax
00457483  call       0x4554d0

// block 00457488
00457488  mov        esi, dword ptr [esp + 0x14]
0045748c  mov        edi, dword ptr [ebp + 8]

// block 0045748f
0045748f  fld        dword ptr [esp + 0xc]
00457493  fmul       dword ptr [esp + 0x10]
00457497  fstp       dword ptr [eax]
00457499  movzx      ecx, word ptr [esi + 2]
0045749d  add        ecx, esi
0045749f  mov        dword ptr [edi + 0x3f0], ecx
004574a5  jmp        0x455686

// block 004574aa
004574aa  test       byte ptr [esi + 6], 2
004574ae  je         0x4574c0

// block 004574b0
004574b0  mov        eax, dword ptr [esi + 0xc]
004574b3  mov        ecx, edi
004574b5  call       0x4553f0

// block 004574ba
004574ba  mov        dword ptr [esp + 0xc], eax
004574be  jmp        0x4574c7

// block 004574c0
004574c0  mov        ecx, dword ptr [esi + 0xc]
004574c3  mov        dword ptr [esp + 0xc], ecx

// block 004574c7
004574c7  test       byte ptr [esi + 6], 4
004574cb  je         0x4574db

// block 004574cd
004574cd  mov        eax, dword ptr [esi + 0x10]
004574d0  mov        ecx, edi
004574d2  call       0x4553f0

// block 004574d7
004574d7  mov        ebx, eax
004574d9  jmp        0x4574de

// block 004574db
004574db  mov        ebx, dword ptr [esi + 0x10]

// block 004574de
004574de  test       byte ptr [esi + 6], 1
004574e2  lea        eax, [esi + 8]
004574e5  je         0x4574ee

// block 004574e7
004574e7  mov        edx, edi
004574e9  call       0x455580

// block 004574ee
004574ee  mov        ecx, eax
004574f0  mov        eax, dword ptr [esp + 0xc]
004574f4  cdq        
004574f5  idiv       ebx
004574f7  mov        dword ptr [ecx], eax
004574f9  movzx      ecx, word ptr [esi + 2]
004574fd  add        ecx, esi
004574ff  mov        dword ptr [edi + 0x3f0], ecx
00457505  jmp        0x455686

// block 0045750a
0045750a  test       byte ptr [esi + 6], 2
0045750e  fld        dword ptr [esi + 0xc]
00457511  je         0x45751e

// block 00457513
00457513  push       ecx
00457514  mov        eax, edi
00457516  fstp       dword ptr [esp]
00457519  call       0x4551b0

// block 0045751e
0045751e  test       byte ptr [esi + 6], 4
00457522  fstp       dword ptr [esp + 0xc]
00457526  fld        dword ptr [esi + 0x10]
00457529  je         0x457536

// block 0045752b
0045752b  push       ecx
0045752c  mov        eax, edi
0045752e  fstp       dword ptr [esp]
00457531  call       0x4551b0

// block 00457536
00457536  test       byte ptr [esi + 6], 1
0045753a  fstp       dword ptr [esp + 0x10]
0045753e  lea        eax, [esi + 8]
00457541  je         0x457554

// block 00457543
00457543  mov        esi, dword ptr [ebp + 8]
00457546  mov        edi, eax
00457548  call       0x4554d0

// block 0045754d
0045754d  mov        esi, dword ptr [esp + 0x14]
00457551  mov        edi, dword ptr [ebp + 8]

// block 00457554
00457554  fld        dword ptr [esp + 0xc]
00457558  fdiv       dword ptr [esp + 0x10]
0045755c  fstp       dword ptr [eax]
0045755e  movzx      ecx, word ptr [esi + 2]
00457562  add        ecx, esi
00457564  mov        dword ptr [edi + 0x3f0], ecx
0045756a  jmp        0x455686

// block 0045756f
0045756f  test       byte ptr [esi + 6], 2
00457573  je         0x457585

// block 00457575
00457575  mov        eax, dword ptr [esi + 0xc]
00457578  mov        ecx, edi
0045757a  call       0x4553f0

// block 0045757f
0045757f  mov        dword ptr [esp + 0xc], eax
00457583  jmp        0x45758c

// block 00457585
00457585  mov        edx, dword ptr [esi + 0xc]
00457588  mov        dword ptr [esp + 0xc], edx

// block 0045758c
0045758c  test       byte ptr [esi + 6], 4
00457590  je         0x4575a0

// block 00457592
00457592  mov        eax, dword ptr [esi + 0x10]
00457595  mov        ecx, edi
00457597  call       0x4553f0

// block 0045759c
0045759c  mov        ebx, eax
0045759e  jmp        0x4575a3

// block 004575a0
004575a0  mov        ebx, dword ptr [esi + 0x10]

// block 004575a3
004575a3  test       byte ptr [esi + 6], 1
004575a7  lea        eax, [esi + 8]
004575aa  je         0x4575b3

// block 004575ac
004575ac  mov        edx, edi
004575ae  call       0x455580

// block 004575b3
004575b3  mov        ecx, eax
004575b5  mov        eax, dword ptr [esp + 0xc]
004575b9  cdq        
004575ba  idiv       ebx
004575bc  mov        dword ptr [ecx], edx
004575be  movzx      ecx, word ptr [esi + 2]
004575c2  add        ecx, esi
004575c4  mov        dword ptr [edi + 0x3f0], ecx
004575ca  jmp        0x455686

// block 004575cf
004575cf  test       byte ptr [esi + 6], 4
004575d3  fld        dword ptr [esi + 0x10]
004575d6  je         0x4575e3

// block 004575d8
004575d8  push       ecx
004575d9  mov        eax, edi
004575db  fstp       dword ptr [esp]
004575de  call       0x4551b0

// block 004575e3
004575e3  test       byte ptr [esi + 6], 2
004575e7  fstp       dword ptr [esp + 0x10]
004575eb  fld        dword ptr [esi + 0xc]
004575ee  je         0x4575fb

// block 004575f0
004575f0  push       ecx
004575f1  mov        eax, edi
004575f3  fstp       dword ptr [esp]
004575f6  call       0x4551b0

// block 004575fb
004575fb  fstp       dword ptr [esp + 0xc]
004575ff  fld        dword ptr [esp + 0xc]
00457603  fld        dword ptr [esp + 0x10]
00457607  call       0x4933ca

// block 0045760c
0045760c  fstp       dword ptr [esp + 0xc]
00457610  fld        dword ptr [esp + 0xc]
00457614  jmp        0x457b57

// block 00457619
00457619  test       byte ptr [esi + 6], 2
0045761d  je         0x45762d

// block 0045761f
0045761f  mov        eax, dword ptr [esi + 0xc]
00457622  mov        ecx, edi
00457624  call       0x4553f0

// block 00457629
00457629  mov        ebx, eax
0045762b  jmp        0x457630

// block 0045762d
0045762d  mov        ebx, dword ptr [esi + 0xc]

// block 00457630
00457630  test       byte ptr [esi + 6], 1
00457634  lea        eax, [esi + 8]
00457637  je         0x457640

// block 00457639
00457639  mov        edx, edi
0045763b  call       0x455580

// block 00457640
00457640  add        dword ptr [eax], ebx
00457642  movzx      ecx, word ptr [esi + 2]
00457646  add        ecx, esi
00457648  mov        dword ptr [edi + 0x3f0], ecx
0045764e  jmp        0x455686

// block 00457653
00457653  test       byte ptr [esi + 6], 2
00457657  fld        dword ptr [esi + 0xc]
0045765a  je         0x457667

// block 0045765c
0045765c  push       ecx
0045765d  mov        eax, edi
0045765f  fstp       dword ptr [esp]
00457662  call       0x4551b0

// block 00457667
00457667  test       byte ptr [esi + 6], 1
0045766b  fstp       dword ptr [esp + 0xc]
0045766f  lea        eax, [esi + 8]
00457672  je         0x457685

// block 00457674
00457674  mov        esi, dword ptr [ebp + 8]
00457677  mov        edi, eax
00457679  call       0x4554d0

// block 0045767e
0045767e  mov        esi, dword ptr [esp + 0x14]
00457682  mov        edi, dword ptr [ebp + 8]

// block 00457685
00457685  fld        dword ptr [esp + 0xc]
00457689  fadd       dword ptr [eax]
0045768b  fstp       dword ptr [eax]
0045768d  movzx      ecx, word ptr [esi + 2]
00457691  add        ecx, esi
00457693  mov        dword ptr [edi + 0x3f0], ecx
00457699  jmp        0x455686

// block 0045769e
0045769e  test       byte ptr [esi + 6], 2
004576a2  je         0x4576b2

// block 004576a4
004576a4  mov        eax, dword ptr [esi + 0xc]
004576a7  mov        ecx, edi
004576a9  call       0x4553f0

// block 004576ae
004576ae  mov        ebx, eax
004576b0  jmp        0x4576b5

// block 004576b2
004576b2  mov        ebx, dword ptr [esi + 0xc]

// block 004576b5
004576b5  test       byte ptr [esi + 6], 1
004576b9  lea        eax, [esi + 8]
004576bc  je         0x4576c5

// block 004576be
004576be  mov        edx, edi
004576c0  call       0x455580

// block 004576c5
004576c5  sub        dword ptr [eax], ebx
004576c7  movzx      ecx, word ptr [esi + 2]
004576cb  add        ecx, esi
004576cd  mov        dword ptr [edi + 0x3f0], ecx
004576d3  jmp        0x455686

// block 004576d8
004576d8  test       byte ptr [esi + 6], 2
004576dc  fld        dword ptr [esi + 0xc]
004576df  je         0x4576ec

// block 004576e1
004576e1  push       ecx
004576e2  mov        eax, edi
004576e4  fstp       dword ptr [esp]
004576e7  call       0x4551b0

// block 004576ec
004576ec  test       byte ptr [esi + 6], 1
004576f0  fstp       dword ptr [esp + 0xc]
004576f4  lea        eax, [esi + 8]
004576f7  je         0x45770a

// block 004576f9
004576f9  mov        esi, dword ptr [ebp + 8]
004576fc  mov        edi, eax
004576fe  call       0x4554d0

// block 00457703
00457703  mov        esi, dword ptr [esp + 0x14]
00457707  mov        edi, dword ptr [ebp + 8]

// block 0045770a
0045770a  fld        dword ptr [eax]
0045770c  fsub       dword ptr [esp + 0xc]
00457710  fstp       dword ptr [eax]
00457712  movzx      ecx, word ptr [esi + 2]
00457716  add        ecx, esi
00457718  mov        dword ptr [edi + 0x3f0], ecx
0045771e  jmp        0x455686

// block 00457723
00457723  test       byte ptr [esi + 6], 2
00457727  je         0x457737

// block 00457729
00457729  mov        eax, dword ptr [esi + 0xc]
0045772c  mov        ecx, edi
0045772e  call       0x4553f0

// block 00457733
00457733  mov        ebx, eax
00457735  jmp        0x45773a

// block 00457737
00457737  mov        ebx, dword ptr [esi + 0xc]

// block 0045773a
0045773a  test       byte ptr [esi + 6], 1
0045773e  lea        eax, [esi + 8]
00457741  je         0x45774a

// block 00457743
00457743  mov        edx, edi
00457745  call       0x455580

// block 0045774a
0045774a  mov        ecx, dword ptr [eax]
0045774c  imul       ecx, ebx
0045774f  mov        dword ptr [eax], ecx
00457751  movzx      ecx, word ptr [esi + 2]
00457755  add        ecx, esi
00457757  mov        dword ptr [edi + 0x3f0], ecx
0045775d  jmp        0x455686

// block 00457762
00457762  test       byte ptr [esi + 6], 2
00457766  fld        dword ptr [esi + 0xc]
00457769  je         0x457776

// block 0045776b
0045776b  push       ecx
0045776c  mov        eax, edi
0045776e  fstp       dword ptr [esp]
00457771  call       0x4551b0

// block 00457776
00457776  test       byte ptr [esi + 6], 1
0045777a  fstp       dword ptr [esp + 0xc]
0045777e  lea        eax, [esi + 8]
00457781  je         0x457794

// block 00457783
00457783  mov        esi, dword ptr [ebp + 8]
00457786  mov        edi, eax
00457788  call       0x4554d0

// block 0045778d
0045778d  mov        esi, dword ptr [esp + 0x14]
00457791  mov        edi, dword ptr [ebp + 8]

// block 00457794
00457794  fld        dword ptr [esp + 0xc]
00457798  fmul       dword ptr [eax]
0045779a  fstp       dword ptr [eax]
0045779c  movzx      ecx, word ptr [esi + 2]
004577a0  add        ecx, esi
004577a2  mov        dword ptr [edi + 0x3f0], ecx
004577a8  jmp        0x455686

// block 004577ad
004577ad  test       byte ptr [esi + 6], 2
004577b1  je         0x4577c1

// block 004577b3
004577b3  mov        eax, dword ptr [esi + 0xc]
004577b6  mov        ecx, edi
004577b8  call       0x4553f0

// block 004577bd
004577bd  mov        ebx, eax
004577bf  jmp        0x4577c4

// block 004577c1
004577c1  mov        ebx, dword ptr [esi + 0xc]

// block 004577c4
004577c4  test       byte ptr [esi + 6], 1
004577c8  lea        eax, [esi + 8]
004577cb  je         0x4577d4

// block 004577cd
004577cd  mov        edx, edi
004577cf  call       0x455580

// block 004577d4
004577d4  mov        ecx, eax
004577d6  mov        eax, dword ptr [ecx]
004577d8  cdq        
004577d9  idiv       ebx
004577db  mov        dword ptr [ecx], eax
004577dd  movzx      ecx, word ptr [esi + 2]
004577e1  add        ecx, esi
004577e3  mov        dword ptr [edi + 0x3f0], ecx
004577e9  jmp        0x455686

// block 004577ee
004577ee  test       byte ptr [esi + 6], 2
004577f2  fld        dword ptr [esi + 0xc]
004577f5  je         0x457802

// block 004577f7
004577f7  push       ecx
004577f8  mov        eax, edi
004577fa  fstp       dword ptr [esp]
004577fd  call       0x4551b0

// block 00457802
00457802  test       byte ptr [esi + 6], 1
00457806  fstp       dword ptr [esp + 0xc]
0045780a  lea        eax, [esi + 8]
0045780d  je         0x457820

// block 0045780f
0045780f  mov        esi, dword ptr [ebp + 8]
00457812  mov        edi, eax
00457814  call       0x4554d0

// block 00457819
00457819  mov        esi, dword ptr [esp + 0x14]
0045781d  mov        edi, dword ptr [ebp + 8]

// block 00457820
00457820  fld        dword ptr [eax]
00457822  fdiv       dword ptr [esp + 0xc]
00457826  fstp       dword ptr [eax]
00457828  movzx      ecx, word ptr [esi + 2]
0045782c  add        ecx, esi
0045782e  mov        dword ptr [edi + 0x3f0], ecx
00457834  jmp        0x455686

// block 00457839
00457839  test       byte ptr [esi + 6], 2
0045783d  je         0x45784d

// block 0045783f
0045783f  mov        eax, dword ptr [esi + 0xc]
00457842  mov        ecx, edi
00457844  call       0x4553f0

// block 00457849
00457849  mov        ebx, eax
0045784b  jmp        0x457850

// block 0045784d
0045784d  mov        ebx, dword ptr [esi + 0xc]

// block 00457850
00457850  test       byte ptr [esi + 6], 1
00457854  lea        eax, [esi + 8]
00457857  je         0x457860

// block 00457859
00457859  mov        edx, edi
0045785b  call       0x455580

// block 00457860
00457860  mov        ecx, eax
00457862  mov        eax, dword ptr [ecx]
00457864  cdq        
00457865  idiv       ebx
00457867  mov        dword ptr [ecx], edx
00457869  movzx      ecx, word ptr [esi + 2]
0045786d  add        ecx, esi
0045786f  mov        dword ptr [edi + 0x3f0], ecx
00457875  jmp        0x455686

// block 0045787a
0045787a  test       byte ptr [esi + 6], 2
0045787e  fld        dword ptr [esi + 0xc]
00457881  je         0x45788e

// block 00457883
00457883  push       ecx
00457884  mov        eax, edi
00457886  fstp       dword ptr [esp]
00457889  call       0x4551b0

// block 0045788e
0045788e  test       byte ptr [esi + 6], 1
00457892  fstp       dword ptr [esp + 0x10]
00457896  fld        dword ptr [esi + 8]
00457899  lea        ebx, [esi + 8]
0045789c  je         0x4578a9

// block 0045789e
0045789e  push       ecx
0045789f  mov        eax, edi
004578a1  fstp       dword ptr [esp]
004578a4  call       0x4551b0

// block 004578a9
004578a9  fstp       dword ptr [esp + 0xc]
004578ad  fld        dword ptr [esp + 0xc]
004578b1  fld        dword ptr [esp + 0x10]
004578b5  call       0x4933ca

// block 004578ba
004578ba  fstp       dword ptr [esp + 0xc]
004578be  test       byte ptr [esi + 6], 1
004578c2  fld        dword ptr [esp + 0xc]
004578c6  fstp       dword ptr [esp + 0xc]
004578ca  jne        0x4578e5

// block 004578cc
004578cc  fld        dword ptr [esp + 0xc]
004578d0  mov        eax, ebx
004578d2  fstp       dword ptr [eax]
004578d4  movzx      ecx, word ptr [esi + 2]
004578d8  add        ecx, esi
004578da  mov        dword ptr [edi + 0x3f0], ecx
004578e0  jmp        0x455686

// block 004578e5
004578e5  mov        edi, ebx
004578e7  jmp        0x457b66

// block 004578ec
004578ec  test       byte ptr [edi + 0x480], 1
004578f3  je         0x457926

// block 004578f5
004578f5  test       byte ptr [esi + 6], 2
004578f9  je         0x457909

// block 004578fb
004578fb  mov        eax, dword ptr [esi + 0xc]
004578fe  mov        ecx, edi
00457900  call       0x4553f0

// block 00457905
00457905  mov        ebx, eax
00457907  jmp        0x45790c

// block 00457909
00457909  mov        ebx, dword ptr [esi + 0xc]

// block 0045790c
0045790c  test       ebx, ebx
0045790e  je         0x457957

// block 00457910
00457910  mov        esi, 0x4ce560
00457915  call       0x464440

// block 0045791a
0045791a  xor        edx, edx
0045791c  div        ebx
0045791e  mov        esi, dword ptr [esp + 0x14]
00457922  mov        ebx, edx
00457924  jmp        0x457959

// block 00457926
00457926  test       byte ptr [esi + 6], 2
0045792a  je         0x45793a

// block 0045792c
0045792c  mov        eax, dword ptr [esi + 0xc]
0045792f  mov        ecx, edi
00457931  call       0x4553f0

// block 00457936
00457936  mov        ebx, eax
00457938  jmp        0x45793d

// block 0045793a
0045793a  mov        ebx, dword ptr [esi + 0xc]

// block 0045793d
0045793d  test       ebx, ebx
0045793f  je         0x457957

// block 00457941
00457941  mov        esi, 0x4ce568
00457946  call       0x464440

// block 0045794b
0045794b  xor        edx, edx
0045794d  div        ebx
0045794f  mov        esi, dword ptr [esp + 0x14]
00457953  mov        ebx, edx
00457955  jmp        0x457959

// block 00457957
00457957  xor        ebx, ebx

// block 00457959
00457959  test       byte ptr [esi + 6], 1
0045795d  lea        eax, [esi + 8]
00457960  je         0x457969

// block 00457962
00457962  mov        edx, edi
00457964  call       0x455580

// block 00457969
00457969  mov        dword ptr [eax], ebx
0045796b  movzx      ecx, word ptr [esi + 2]
0045796f  add        ecx, esi
00457971  mov        dword ptr [edi + 0x3f0], ecx
00457977  jmp        0x455686

// block 0045797c
0045797c  fld        dword ptr [esi + 0xc]
0045797f  mov        bl, 1
00457981  test       byte ptr [edi + 0x480], bl
00457987  je         0x4579a9

// block 00457989
00457989  test       byte ptr [esi + 6], 2
0045798d  je         0x45799a

// block 0045798f
0045798f  push       ecx
00457990  mov        eax, edi
00457992  fstp       dword ptr [esp]
00457995  call       0x4551b0

// block 0045799a
0045799a  fstp       dword ptr [esp + 0xc]
0045799e  mov        esi, 0x4ce560
004579a3  fld        dword ptr [esp + 0xc]
004579a7  jmp        0x4579c7

// block 004579a9
004579a9  test       byte ptr [esi + 6], 2
004579ad  je         0x4579ba

// block 004579af
004579af  push       ecx
004579b0  mov        eax, edi
004579b2  fstp       dword ptr [esp]
004579b5  call       0x4551b0

// block 004579ba
004579ba  fstp       dword ptr [esp + 0xc]
004579be  mov        esi, 0x4ce568
004579c3  fld        dword ptr [esp + 0xc]

// block 004579c7
004579c7  push       ecx
004579c8  fstp       dword ptr [esp]
004579cb  call       0x40d660

// block 004579d0
004579d0  mov        ecx, dword ptr [esp + 0x14]
004579d4  fstp       dword ptr [esp + 0xc]
004579d8  lea        eax, [ecx + 8]
004579db  test       byte ptr [ecx + 6], bl
004579de  je         0x4579ec

// block 004579e0
004579e0  mov        esi, dword ptr [ebp + 8]
004579e3  mov        edi, eax
004579e5  call       0x4554d0

// block 004579ea
004579ea  mov        edi, esi

// block 004579ec
004579ec  fld        dword ptr [esp + 0xc]
004579f0  mov        esi, dword ptr [esp + 0x14]
004579f4  fstp       dword ptr [eax]
004579f6  movzx      ecx, word ptr [esi + 2]
004579fa  add        ecx, esi
004579fc  mov        dword ptr [edi + 0x3f0], ecx
00457a02  jmp        0x455686

// block 00457a07
00457a07  test       byte ptr [esi + 6], 2
00457a0b  fld        dword ptr [esi + 0xc]
00457a0e  je         0x457a1b

// block 00457a10
00457a10  push       ecx
00457a11  mov        eax, edi
00457a13  fstp       dword ptr [esp]
00457a16  call       0x4551b0

// block 00457a1b
00457a1b  test       byte ptr [esi + 6], 1
00457a1f  fstp       dword ptr [esp + 0xc]
00457a23  lea        eax, [esi + 8]
00457a26  je         0x457a39

// block 00457a28
00457a28  mov        esi, dword ptr [ebp + 8]
00457a2b  mov        edi, eax
00457a2d  call       0x4554d0

// block 00457a32
00457a32  mov        esi, dword ptr [esp + 0x14]
00457a36  mov        edi, dword ptr [ebp + 8]

// block 00457a39
00457a39  fld        dword ptr [esp + 0xc]
00457a3d  push       ecx
00457a3e  fstp       dword ptr [esp]
00457a41  mov        ebx, eax
00457a43  call       0x406680

// block 00457a48
00457a48  fstp       dword ptr [ebx]
00457a4a  movzx      ecx, word ptr [esi + 2]
00457a4e  add        ecx, esi
00457a50  mov        dword ptr [edi + 0x3f0], ecx
00457a56  jmp        0x455686

// block 00457a5b
00457a5b  test       byte ptr [esi + 6], 2
00457a5f  fld        dword ptr [esi + 0xc]
00457a62  je         0x457a6f

// block 00457a64
00457a64  push       ecx
00457a65  mov        eax, edi
00457a67  fstp       dword ptr [esp]
00457a6a  call       0x4551b0

// block 00457a6f
00457a6f  test       byte ptr [esi + 6], 1
00457a73  fstp       dword ptr [esp + 0xc]
00457a77  lea        eax, [esi + 8]
00457a7a  je         0x457a8d

// block 00457a7c
00457a7c  mov        esi, dword ptr [ebp + 8]
00457a7f  mov        edi, eax
00457a81  call       0x4554d0

// block 00457a86
00457a86  mov        esi, dword ptr [esp + 0x14]
00457a8a  mov        edi, dword ptr [ebp + 8]

// block 00457a8d
00457a8d  fld        dword ptr [esp + 0xc]
00457a91  push       ecx
00457a92  fstp       dword ptr [esp]
00457a95  mov        ebx, eax
00457a97  call       0x406170

// block 00457a9c
00457a9c  fstp       dword ptr [ebx]
00457a9e  movzx      ecx, word ptr [esi + 2]
00457aa2  add        ecx, esi
00457aa4  mov        dword ptr [edi + 0x3f0], ecx
00457aaa  jmp        0x455686

// block 00457aaf
00457aaf  test       byte ptr [esi + 6], 2
00457ab3  fld        dword ptr [esi + 0xc]
00457ab6  je         0x457ac3

// block 00457ab8
00457ab8  push       ecx
00457ab9  mov        eax, edi
00457abb  fstp       dword ptr [esp]
00457abe  call       0x4551b0

// block 00457ac3
00457ac3  test       byte ptr [esi + 6], 1
00457ac7  fstp       dword ptr [esp + 0xc]
00457acb  lea        eax, [esi + 8]
00457ace  je         0x457ae1

// block 00457ad0
00457ad0  mov        esi, dword ptr [ebp + 8]
00457ad3  mov        edi, eax
00457ad5  call       0x4554d0

// block 00457ada
00457ada  mov        esi, dword ptr [esp + 0x14]
00457ade  mov        edi, dword ptr [ebp + 8]

// block 00457ae1
00457ae1  fld        dword ptr [esp + 0xc]
00457ae5  push       ecx
00457ae6  fstp       dword ptr [esp]
00457ae9  mov        ebx, eax
00457aeb  call       0x4317d0

// block 00457af0
00457af0  fstp       dword ptr [ebx]
00457af2  movzx      ecx, word ptr [esi + 2]
00457af6  add        ecx, esi
00457af8  mov        dword ptr [edi + 0x3f0], ecx
00457afe  jmp        0x455686

// block 00457b03
00457b03  test       byte ptr [esi + 6], 2
00457b07  fld        dword ptr [esi + 0xc]
00457b0a  je         0x457b17

// block 00457b0c
00457b0c  push       ecx
00457b0d  mov        eax, edi
00457b0f  fstp       dword ptr [esp]
00457b12  call       0x4551b0

// block 00457b17
00457b17  fstp       dword ptr [esp + 0xc]
00457b1b  fld        dword ptr [esp + 0xc]
00457b1f  call       0x493440

// block 00457b24
00457b24  fstp       dword ptr [esp + 0xc]
00457b28  fld        dword ptr [esp + 0xc]
00457b2c  jmp        0x457b57

// block 00457b2e
00457b2e  test       byte ptr [esi + 6], 2
00457b32  fld        dword ptr [esi + 0xc]
00457b35  je         0x457b42

// block 00457b37
00457b37  push       ecx
00457b38  mov        eax, edi
00457b3a  fstp       dword ptr [esp]
00457b3d  call       0x4551b0

// block 00457b42
00457b42  fstp       dword ptr [esp + 0xc]
00457b46  fld        dword ptr [esp + 0xc]
00457b4a  call       0x493590

// block 00457b4f
00457b4f  fstp       dword ptr [esp + 0xc]
00457b53  fld        dword ptr [esp + 0xc]

// block 00457b57
00457b57  test       byte ptr [esi + 6], 1
00457b5b  fstp       dword ptr [esp + 0xc]
00457b5f  lea        eax, [esi + 8]
00457b62  je         0x457b75

// block 00457b64
00457b64  mov        edi, eax

// block 00457b66
00457b66  mov        esi, dword ptr [ebp + 8]
00457b69  call       0x4554d0

// block 00457b6e
00457b6e  mov        esi, dword ptr [esp + 0x14]
00457b72  mov        edi, dword ptr [ebp + 8]

// block 00457b75
00457b75  fld        dword ptr [esp + 0xc]
00457b79  fstp       dword ptr [eax]
00457b7b  movzx      ecx, word ptr [esi + 2]
00457b7f  add        ecx, esi
00457b81  mov        dword ptr [edi + 0x3f0], ecx
00457b87  jmp        0x455686

// block 00457b8c
00457b8c  test       byte ptr [esi + 6], 1
00457b90  fld        dword ptr [esi + 8]
00457b93  lea        ebx, [esi + 8]
00457b96  je         0x457ba3

// block 00457b98
00457b98  push       ecx
00457b99  mov        eax, edi
00457b9b  fstp       dword ptr [esp]
00457b9e  call       0x4551b0

// block 00457ba3
00457ba3  test       byte ptr [esi + 6], 1
00457ba7  fstp       dword ptr [esp + 0xc]
00457bab  je         0x457bc0

// block 00457bad
00457bad  mov        esi, dword ptr [ebp + 8]
00457bb0  mov        edi, ebx
00457bb2  call       0x4554d0

// block 00457bb7
00457bb7  mov        esi, dword ptr [esp + 0x14]
00457bbb  mov        edi, dword ptr [ebp + 8]
00457bbe  mov        ebx, eax

// block 00457bc0
00457bc0  fldz       
00457bc2  sub        esp, 8
00457bc5  fstp       dword ptr [esp + 4]
00457bc9  fld        dword ptr [esp + 0x14]
00457bcd  fstp       dword ptr [esp]
00457bd0  call       0x464640

// block 00457bd5
00457bd5  fstp       dword ptr [ebx]
00457bd7  movzx      ecx, word ptr [esi + 2]
00457bdb  add        ecx, esi
00457bdd  mov        dword ptr [edi + 0x3f0], ecx
00457be3  jmp        0x455686

// block 00457be8
00457be8  movzx      edx, byte ptr [esi + 8]
00457bec  shl        edx, 0x1e
00457bef  xor        edx, dword ptr [edi + 0x47c]
00457bf5  and        edx, 0x40000000
00457bfb  xor        dword ptr [edi + 0x47c], edx
00457c01  movzx      ecx, word ptr [esi + 2]
00457c05  add        ecx, esi
00457c07  mov        dword ptr [edi + 0x3f0], ecx
00457c0d  jmp        0x455686

// block 00457c12
00457c12  movzx      eax, byte ptr [esi + 8]
00457c16  mov        dword ptr [edi + 0x20], eax
00457c19  movzx      ecx, word ptr [esi + 2]
00457c1d  add        ecx, esi
00457c1f  mov        dword ptr [edi + 0x3f0], ecx
00457c25  jmp        0x455686

// block 00457c2a
00457c2a  movzx      ecx, byte ptr [esi + 8]
00457c2e  shl        ecx, 0x10
00457c31  xor        ecx, dword ptr [edi + 0x47c]
00457c37  and        ecx, 0x10000
00457c3d  xor        dword ptr [edi + 0x47c], ecx
00457c43  movzx      ecx, word ptr [esi + 2]
00457c47  add        ecx, esi
00457c49  mov        dword ptr [edi + 0x3f0], ecx
00457c4f  jmp        0x455686

// block 00457c54
00457c54  movzx      edx, byte ptr [esi + 8]
00457c58  shl        edx, 0x1d
00457c5b  xor        edx, dword ptr [edi + 0x47c]
00457c61  and        edx, 0x20000000
00457c67  xor        dword ptr [edi + 0x47c], edx
00457c6d  movzx      ecx, word ptr [esi + 2]
00457c71  add        ecx, esi
00457c73  mov        dword ptr [edi + 0x3f0], ecx
00457c79  jmp        0x455686

// block 00457c7e
00457c7e  movzx      eax, byte ptr [esi + 8]
00457c82  xor        eax, dword ptr [edi + 0x480]
00457c88  and        eax, 1

// block 00457c8b
00457c8b  xor        dword ptr [edi + 0x480], eax

// block 00457c91
00457c91  movzx      ecx, word ptr [esi + 2]
00457c95  add        ecx, esi
00457c97  mov        dword ptr [edi + 0x3f0], ecx
00457c9d  jmp        0x455686

// block 00457ca2
00457ca2  mov        esi, 4

// block 00457ca7
00457ca7  fldz       
00457ca9  fcomp      dword ptr [edi + 0x34]
00457cac  fnstsw     ax
00457cae  test       ah, 0x44
00457cb1  jnp        0x457cdf

// block 00457cb3
00457cb3  fld        dword ptr [edi + 0x34]
00457cb6  sub        esp, 8
00457cb9  fmul       dword ptr [0x4b2ed0]
00457cbf  fstp       dword ptr [esp + 0x20]
00457cc3  fld        dword ptr [esp + 0x20]
00457cc7  fstp       dword ptr [esp + 4]
00457ccb  fld        dword ptr [edi + 0x28]
00457cce  fstp       dword ptr [esp]
00457cd1  call       0x464640

// block 00457cd6
00457cd6  or         dword ptr [edi + 0x47c], esi
00457cdc  fstp       dword ptr [edi + 0x28]

// block 00457cdf
00457cdf  fldz       
00457ce1  fcomp      dword ptr [edi + 0x38]
00457ce4  fnstsw     ax
00457ce6  test       ah, 0x44
00457ce9  jnp        0x457d17

// block 00457ceb
00457ceb  fld        dword ptr [edi + 0x38]
00457cee  sub        esp, 8
00457cf1  fmul       dword ptr [0x4b2ed0]
00457cf7  fstp       dword ptr [esp + 0x20]
00457cfb  fld        dword ptr [esp + 0x20]
00457cff  fstp       dword ptr [esp + 4]
00457d03  fld        dword ptr [edi + 0x2c]
00457d06  fstp       dword ptr [esp]
00457d09  call       0x464640

// block 00457d0e
00457d0e  or         dword ptr [edi + 0x47c], esi
00457d14  fstp       dword ptr [edi + 0x2c]

// block 00457d17
00457d17  fldz       
00457d19  fcom       dword ptr [edi + 0x4c]
00457d1c  fnstsw     ax
00457d1e  test       ah, 0x44
00457d21  jnp        0x457d39

// block 00457d23
00457d23  fld        dword ptr [edi + 0x4c]
00457d26  fmul       dword ptr [0x4b2ed0]
00457d2c  or         dword ptr [edi + 0x47c], 8
00457d33  fadd       dword ptr [edi + 0x44]
00457d36  fstp       dword ptr [edi + 0x44]

// block 00457d39
00457d39  fcom       dword ptr [edi + 0x48]
00457d3c  fnstsw     ax
00457d3e  test       ah, 0x44
00457d41  jnp        0x457d59

// block 00457d43
00457d43  fld        dword ptr [edi + 0x48]
00457d46  fmul       dword ptr [0x4b2ed0]
00457d4c  or         dword ptr [edi + 0x47c], 0xc
00457d53  fadd       dword ptr [edi + 0x40]
00457d56  fstp       dword ptr [edi + 0x40]

// block 00457d59
00457d59  fld        dword ptr [edi + 0x2f4]
00457d5f  fmul       dword ptr [0x4b2ed0]
00457d65  fadd       dword ptr [edi + 0x60]
00457d68  fstp       dword ptr [esp + 0x18]
00457d6c  fld        dword ptr [esp + 0x18]
00457d70  fst        dword ptr [edi + 0x60]
00457d73  fld1       
00457d75  fcom       st(1)
00457d77  fnstsw     ax
00457d79  fld1       
00457d7b  test       ah, 0x41
00457d7e  jp         0x457d89

// block 00457d80
00457d80  fsub       st(2), st(0)
00457d82  fxch       st(2)
00457d84  fstp       dword ptr [edi + 0x60]
00457d87  jmp        0x457d9d

// block 00457d89
00457d89  fxch       st(2)
00457d8b  fcom       st(3)
00457d8d  fnstsw     ax
00457d8f  test       ah, 5
00457d92  jp         0x457d9b

// block 00457d94
00457d94  fadd       st(2)
00457d96  fstp       dword ptr [edi + 0x60]
00457d99  jmp        0x457d9d

// block 00457d9b
00457d9b  fstp       st(0)

// block 00457d9d
00457d9d  fld        dword ptr [edi + 0x2f8]
00457da3  fmul       dword ptr [0x4b2ed0]
00457da9  fadd       dword ptr [edi + 0x64]
00457dac  fstp       dword ptr [esp + 0x18]
00457db0  fld        dword ptr [esp + 0x18]
00457db4  fst        dword ptr [edi + 0x64]
00457db7  fcom       st(1)
00457db9  fnstsw     ax
00457dbb  fstp       st(1)
00457dbd  test       ah, 1
00457dc0  jne        0x457dcb

// block 00457dc2
00457dc2  fstp       st(2)
00457dc4  fsubp      st(1)
00457dc6  fstp       dword ptr [edi + 0x64]
00457dc9  jmp        0x457de1

// block 00457dcb
00457dcb  fcom       st(2)
00457dcd  fnstsw     ax
00457dcf  fstp       st(2)
00457dd1  test       ah, 5
00457dd4  jp         0x457ddd

// block 00457dd6
00457dd6  faddp      st(1)
00457dd8  fstp       dword ptr [edi + 0x64]
00457ddb  jmp        0x457de1

// block 00457ddd
00457ddd  fstp       st(1)
00457ddf  fstp       st(0)

// block 00457de1
00457de1  test       dword ptr [edi + 0x47c], 0x4000
00457deb  je         0x457e23

// block 00457ded
00457ded  fld        dword ptr [edi + 0x430]
00457df3  fadd       dword ptr [0x4cee0c]
00457df9  fstp       dword ptr [edi + 0x430]
00457dff  fld        dword ptr [edi + 0x434]
00457e05  fadd       dword ptr [0x4cee10]
00457e0b  fstp       dword ptr [edi + 0x434]
00457e11  fld        dword ptr [edi + 0x438]
00457e17  fadd       dword ptr [0x4cee14]
00457e1d  fstp       dword ptr [edi + 0x438]

// block 00457e23
00457e23  test       byte ptr [edi + 0x480], 0x10
00457e2a  je         0x457fa4

// block 00457e30
00457e30  lea        ecx, [esp + 0x9c]
00457e37  mov        eax, edi
00457e39  call       0x45cdf0

// block 00457e3e
00457e3e  fld        dword ptr [esp + 0x9c]
00457e45  fld        qword ptr [0x4a3d18]
00457e4b  fdiv       st(1), st(0)
00457e4d  fxch       st(1)
00457e4f  fstp       dword ptr [esp + 0x18]
00457e53  fld        dword ptr [esp + 0x18]
00457e57  fst        dword ptr [edi + 0x7c]
00457e5a  fld        dword ptr [esp + 0xa0]
00457e61  fld        qword ptr [0x4a3d10]
00457e67  fdiv       st(1), st(0)
00457e69  fxch       st(1)
00457e6b  fstp       dword ptr [esp + 0x18]
00457e6f  fld        dword ptr [esp + 0x18]
00457e73  fst        dword ptr [edi + 0x80]
00457e79  fldz       
00457e7b  fcom       st(3)
00457e7d  fnstsw     ax
00457e7f  fstp       st(3)
00457e81  test       ah, 0x41
00457e84  jne        0x457e8d

// block 00457e86
00457e86  fxch       st(2)
00457e88  fst        dword ptr [edi + 0x7c]
00457e8b  fxch       st(2)

// block 00457e8d
00457e8d  fcomp      st(2)
00457e8f  fnstsw     ax
00457e91  test       ah, 5
00457e94  jp         0x457ea0

// block 00457e96
00457e96  fxch       st(1)
00457e98  fst        dword ptr [edi + 0x80]
00457e9e  fxch       st(1)

// block 00457ea0
00457ea0  fld        dword ptr [esp + 0xa8]
00457ea7  fdiv       st(3)
00457ea9  fstp       dword ptr [esp + 0x18]
00457ead  fld        dword ptr [esp + 0x18]
00457eb1  fst        dword ptr [edi + 0x84]
00457eb7  fld        dword ptr [esp + 0xac]
00457ebe  fdiv       st(2)
00457ec0  fstp       dword ptr [esp + 0x18]
00457ec4  fld        dword ptr [esp + 0x18]
00457ec8  fst        dword ptr [edi + 0x88]
00457ece  fxch       st(1)
00457ed0  fcomp      st(3)
00457ed2  fnstsw     ax
00457ed4  test       ah, 5
00457ed7  jp         0x457ee3

// block 00457ed9
00457ed9  fxch       st(2)
00457edb  fst        dword ptr [edi + 0x84]
00457ee1  fxch       st(2)

// block 00457ee3
00457ee3  fcomp      st(2)
00457ee5  fnstsw     ax
00457ee7  test       ah, 5
00457eea  jp         0x457ef6

// block 00457eec
00457eec  fxch       st(1)
00457eee  fst        dword ptr [edi + 0x88]
00457ef4  fxch       st(1)

// block 00457ef6
00457ef6  fld        dword ptr [esp + 0xb4]
00457efd  fdiv       st(3)
00457eff  fstp       dword ptr [esp + 0x18]
00457f03  fld        dword ptr [esp + 0x18]
00457f07  fst        dword ptr [edi + 0x8c]
00457f0d  fld        dword ptr [esp + 0xb8]
00457f14  fdiv       st(2)
00457f16  fstp       dword ptr [esp + 0x18]
00457f1a  fld        dword ptr [esp + 0x18]
00457f1e  fst        dword ptr [edi + 0x90]
00457f24  fxch       st(1)
00457f26  fcomp      st(3)
00457f28  fnstsw     ax
00457f2a  test       ah, 5
00457f2d  jp         0x457f39

// block 00457f2f
00457f2f  fxch       st(2)
00457f31  fst        dword ptr [edi + 0x8c]
00457f37  fxch       st(2)

// block 00457f39
00457f39  fcomp      st(2)
00457f3b  fnstsw     ax
00457f3d  test       ah, 5
00457f40  jp         0x457f4c

// block 00457f42
00457f42  fxch       st(1)
00457f44  fst        dword ptr [edi + 0x90]
00457f4a  fxch       st(1)

// block 00457f4c
00457f4c  fld        dword ptr [esp + 0xc0]
00457f53  fdivrp     st(3)
00457f55  fxch       st(2)
00457f57  fstp       dword ptr [esp + 0x18]
00457f5b  fld        dword ptr [esp + 0x18]
00457f5f  fst        dword ptr [edi + 0x94]
00457f65  fld        dword ptr [esp + 0xc4]
00457f6c  fdivrp     st(3)
00457f6e  fxch       st(2)
00457f70  fstp       dword ptr [esp + 0x18]
00457f74  fld        dword ptr [esp + 0x18]
00457f78  fst        dword ptr [edi + 0x98]
00457f7e  fxch       st(2)
00457f80  fcomp      st(1)
00457f82  fnstsw     ax
00457f84  test       ah, 5
00457f87  jp         0x457f8f

// block 00457f89
00457f89  fst        dword ptr [edi + 0x94]

// block 00457f8f
00457f8f  fcom       st(1)
00457f91  fnstsw     ax
00457f93  fstp       st(1)
00457f95  test       ah, 0x41
00457f98  jne        0x457fa2

// block 00457f9a
00457f9a  fstp       dword ptr [edi + 0x98]
00457fa0  jmp        0x457fa4

// block 00457fa2
00457fa2  fstp       st(0)

// block 00457fa4
00457fa4  cmp        dword ptr [edi + 0xe0], 0
00457fab  je         0x458011

// block 00457fad
00457fad  test       dword ptr [edi + 0x47c], 0x200
00457fb7  lea        ebx, [esp + 0x20]
00457fbb  jne        0x457fe7

// block 00457fbd
00457fbd  add        edi, 0x9c
00457fc3  call       0x405900

// block 00457fc8
00457fc8  mov        edx, dword ptr [eax]
00457fca  mov        ecx, dword ptr [ebp + 8]
00457fcd  mov        dword ptr [ecx + 0x424], edx
00457fd3  mov        edx, dword ptr [eax + 4]
00457fd6  mov        dword ptr [ecx + 0x428], edx
00457fdc  mov        eax, dword ptr [eax + 8]
00457fdf  mov        dword ptr [ecx + 0x42c], eax
00457fe5  jmp        0x45800f

// block 00457fe7
00457fe7  add        edi, 0x9c
00457fed  call       0x405900

// block 00457ff2
00457ff2  mov        edx, dword ptr [eax]
00457ff4  mov        ecx, dword ptr [ebp + 8]
00457ff7  mov        dword ptr [ecx + 0x43c], edx
00457ffd  mov        edx, dword ptr [eax + 4]
00458000  mov        dword ptr [ecx + 0x440], edx
00458006  mov        eax, dword ptr [eax + 8]
00458009  mov        dword ptr [ecx + 0x444], eax

// block 0045800f
0045800f  mov        edi, ecx

// block 00458011
00458011  cmp        dword ptr [edi + 0x12c], 0
00458018  je         0x458047

// block 0045801a
0045801a  lea        eax, [edi + 0xe8]
00458020  lea        ebx, [esp + 0x20]
00458024  call       0x4589e0

// block 00458029
00458029  mov        cl, byte ptr [esp + 0x28]
0045802d  mov        dl, byte ptr [esp + 0x24]
00458031  mov        al, byte ptr [esp + 0x20]
00458035  mov        byte ptr [edi + 0x3be], cl
0045803b  mov        byte ptr [edi + 0x3bd], dl
00458041  mov        byte ptr [edi + 0x3bc], al

// block 00458047
00458047  cmp        dword ptr [edi + 0x158], 0
0045804e  je         0x458066

// block 00458050
00458050  add        edi, 0x134
00458056  call       0x458cf0

// block 0045805b
0045805b  mov        ecx, dword ptr [ebp + 8]
0045805e  mov        byte ptr [ecx + 0x3bf], al
00458064  mov        edi, ecx

// block 00458066
00458066  cmp        dword ptr [edi + 0x1e0], 0
0045806d  je         0x458095

// block 0045806f
0045806f  add        edi, 0x1ac
00458075  lea        ebx, [esp + 0x2c]
00458079  call       0x458e40

// block 0045807e
0045807e  mov        edx, dword ptr [eax]
00458080  mov        ecx, dword ptr [ebp + 8]
00458083  mov        dword ptr [ecx + 0x40], edx
00458086  mov        eax, dword ptr [eax + 4]
00458089  or         dword ptr [ecx + 0x47c], 8
00458090  mov        dword ptr [ecx + 0x44], eax
00458093  mov        edi, ecx

// block 00458095
00458095  cmp        dword ptr [edi + 0x21c], 0
0045809c  je         0x4580c4

// block 0045809e
0045809e  add        edi, 0x1e8
004580a4  lea        ebx, [esp + 0x2c]
004580a8  call       0x458e40

// block 004580ad
004580ad  mov        edx, dword ptr [eax]
004580af  mov        ecx, dword ptr [ebp + 8]
004580b2  mov        dword ptr [ecx + 0x50], edx
004580b5  mov        eax, dword ptr [eax + 4]
004580b8  or         dword ptr [ecx + 0x47c], 0x10
004580bf  mov        dword ptr [ecx + 0x54], eax
004580c2  mov        edi, ecx

// block 004580c4
004580c4  cmp        dword ptr [edi + 0x1a4], 0
004580cb  je         0x4580f9

// block 004580cd
004580cd  add        edi, 0x160
004580d3  lea        ebx, [esp + 0x20]
004580d7  call       0x405900

// block 004580dc
004580dc  mov        edx, dword ptr [eax]
004580de  mov        ecx, dword ptr [ebp + 8]
004580e1  mov        dword ptr [ecx + 0x24], edx
004580e4  mov        edx, dword ptr [eax + 4]
004580e7  mov        dword ptr [ecx + 0x28], edx
004580ea  mov        eax, dword ptr [eax + 8]
004580ed  or         dword ptr [ecx + 0x47c], 4
004580f4  mov        dword ptr [ecx + 0x2c], eax
004580f7  mov        edi, ecx

// block 004580f9
004580f9  cmp        dword ptr [edi + 0x268], 0
00458100  je         0x45812f

// block 00458102
00458102  lea        eax, [edi + 0x224]
00458108  lea        ebx, [esp + 0x20]
0045810c  call       0x4589e0

// block 00458111
00458111  mov        cl, byte ptr [esp + 0x28]
00458115  mov        dl, byte ptr [esp + 0x24]
00458119  mov        al, byte ptr [esp + 0x20]
0045811d  mov        byte ptr [edi + 0x3c2], cl
00458123  mov        byte ptr [edi + 0x3c1], dl
00458129  mov        byte ptr [edi + 0x3c0], al

// block 0045812f
0045812f  cmp        dword ptr [edi + 0x294], 0
00458136  je         0x45814e

// block 00458138
00458138  add        edi, 0x270
0045813e  call       0x458cf0

// block 00458143
00458143  mov        ecx, dword ptr [ebp + 8]
00458146  mov        byte ptr [ecx + 0x3c3], al
0045814c  mov        edi, ecx

// block 0045814e
0045814e  cmp        dword ptr [edi + 0x2c0], 0
00458155  je         0x45816d

// block 00458157
00458157  add        edi, 0x29c
0045815d  call       0x459100

// block 00458162
00458162  mov        edx, dword ptr [ebp + 8]
00458165  fstp       dword ptr [edx + 0x2f4]
0045816b  mov        edi, edx

// block 0045816d
0045816d  cmp        dword ptr [edi + 0x2ec], 0
00458174  je         0x45818c

// block 00458176
00458176  add        edi, 0x2c8
0045817c  call       0x459100

// block 00458181
00458181  mov        eax, dword ptr [ebp + 8]
00458184  fstp       dword ptr [eax + 0x2f8]
0045818a  mov        edi, eax

// block 0045818c
0045818c  mov        esi, dword ptr [edi + 0x47c]
00458192  mov        eax, esi
00458194  shr        eax, 0x17
00458197  and        eax, 0x1f
0045819a  cmp        eax, 9
0045819d  jne        0x45842c

// block 004581a3
004581a3  mov        eax, dword ptr [edi + 0x3fc]
004581a9  fld        dword ptr [edi + 0x2c]
004581ac  fstp       dword ptr [esp + 0x1c]
004581b0  lea        ecx, [eax - 1]
004581b3  mov        dword ptr [esp + 0x18], ecx
004581b7  fild       dword ptr [esp + 0x18]
004581bb  mov        eax, dword ptr [edi + 0x3c]
004581be  mov        ebx, dword ptr [edi + 0x478]
004581c4  fstp       dword ptr [esp + 0x18]
004581c8  fld        dword ptr [esp + 0x18]
004581cc  fld        st(0)
004581ce  fdivr      qword ptr [0x4a3cd0]
004581d4  fstp       dword ptr [esp + 0x38]
004581d8  fldz       
004581da  fstp       dword ptr [esp + 0x14]
004581de  fidivr     dword ptr [edi + 0x400]
004581e4  fstp       dword ptr [esp + 0x3c]
004581e8  fld        dword ptr [edi + 0x43c]
004581ee  fadd       dword ptr [edi + 0x424]
004581f4  fstp       dword ptr [esp + 0x20]
004581f8  fld        dword ptr [edi + 0x440]
004581fe  fadd       dword ptr [edi + 0x428]
00458204  fstp       dword ptr [esp + 0x24]
00458208  fld        dword ptr [edi + 0x444]
0045820e  fadd       dword ptr [edi + 0x42c]
00458214  fstp       dword ptr [esp + 0x28]
00458218  fld        dword ptr [esp + 0x20]
0045821c  fadd       dword ptr [edi + 0x430]
00458222  fstp       dword ptr [esp + 0x2c]
00458226  fld        dword ptr [edi + 0x434]
0045822c  fadd       dword ptr [esp + 0x24]
00458230  fstp       dword ptr [esp + 0x30]
00458234  fld        dword ptr [edi + 0x438]
0045823a  fadd       dword ptr [esp + 0x28]
0045823e  fstp       dword ptr [esp + 0x34]
00458242  test       eax, eax
00458244  je         0x4582c4

// block 00458246
00458246  fld        dword ptr [eax + 0x43c]
0045824c  fadd       dword ptr [eax + 0x424]
00458252  fstp       dword ptr [esp + 0x20]
00458256  fld        dword ptr [eax + 0x428]
0045825c  fadd       dword ptr [eax + 0x440]
00458262  fstp       dword ptr [esp + 0x24]
00458266  fld        dword ptr [eax + 0x42c]
0045826c  fadd       dword ptr [eax + 0x444]
00458272  fstp       dword ptr [esp + 0x28]
00458276  fld        dword ptr [esp + 0x20]
0045827a  fadd       dword ptr [eax + 0x430]
00458280  fstp       dword ptr [esp + 0x40]
00458284  fld        dword ptr [eax + 0x434]
0045828a  fadd       dword ptr [esp + 0x24]
0045828e  fstp       dword ptr [esp + 0x44]
00458292  fld        dword ptr [eax + 0x438]
00458298  fadd       dword ptr [esp + 0x28]
0045829c  fstp       dword ptr [esp + 0x48]
004582a0  fld        dword ptr [esp + 0x40]
004582a4  fadd       dword ptr [esp + 0x2c]
004582a8  fstp       dword ptr [esp + 0x2c]
004582ac  fld        dword ptr [esp + 0x44]
004582b0  fadd       dword ptr [esp + 0x30]
004582b4  fstp       dword ptr [esp + 0x30]
004582b8  fld        dword ptr [esp + 0x48]
004582bc  fadd       dword ptr [esp + 0x34]
004582c0  fstp       dword ptr [esp + 0x34]

// block 004582c4
004582c4  test       esi, 0x10000
004582ca  je         0x4582d4

// block 004582cc
004582cc  mov        esi, dword ptr [edi + 0x3c0]
004582d2  jmp        0x4582da

// block 004582d4
004582d4  mov        esi, dword ptr [edi + 0x3bc]

// block 004582da
004582da  test       ecx, ecx
004582dc  jle        0x4583ee

// block 004582e2
004582e2  fld        dword ptr [esp + 0x34]
004582e6  mov        dword ptr [esp + 0x10], ecx
004582ea  fadd       qword ptr [0x4a3d58]
004582f0  fstp       dword ptr [esp + 0xc]

// block 004582f4
004582f4  fld1       
004582f6  mov        dword ptr [ebx + 0x10], esi
004582f9  fstp       dword ptr [ebx + 0xc]
004582fc  sub        esp, 8
004582ff  fld        dword ptr [edi + 0x7c]
00458302  mov        ecx, ebx
00458304  fadd       dword ptr [edi + 0x60]
00458307  fstp       dword ptr [ebx + 0x14]
0045830a  fld        dword ptr [esp + 0x1c]
0045830e  fadd       dword ptr [edi + 0x64]
00458311  fstp       dword ptr [ebx + 0x18]
00458314  fld        dword ptr [edi + 0x40]
00458317  fmul       qword ptr [0x4a3cf8]
0045831d  fadd       dword ptr [edi + 0x44]
00458320  fstp       dword ptr [esp + 0x20]
00458324  fld        dword ptr [esp + 0x20]
00458328  fstp       dword ptr [esp + 4]
0045832c  fld        dword ptr [esp + 0x24]
00458330  fstp       dword ptr [esp]
00458333  call       0x4594b0

// block 00458338
00458338  fld        dword ptr [esp + 0x2c]
0045833c  add        ebx, 0x1c
0045833f  fadd       dword ptr [ebx - 0x1c]
00458342  sub        esp, 8
00458345  mov        ecx, ebx
00458347  fstp       dword ptr [ebx - 0x1c]
0045834a  fld        dword ptr [ebx - 0x18]
0045834d  fadd       dword ptr [esp + 0x38]
00458351  fstp       dword ptr [ebx - 0x18]
00458354  fld        dword ptr [esp + 0x14]
00458358  fstp       dword ptr [ebx - 0x14]
0045835b  mov        dword ptr [ebx + 0x10], esi
0045835e  fld1       
00458360  fstp       dword ptr [ebx + 0xc]
00458363  fld        dword ptr [edi + 0x84]
00458369  fadd       dword ptr [edi + 0x60]
0045836c  fstp       dword ptr [ebx + 0x14]
0045836f  fld        dword ptr [esp + 0x1c]
00458373  fadd       dword ptr [edi + 0x64]
00458376  fstp       dword ptr [ebx + 0x18]
00458379  fld        dword ptr [edi + 0x44]
0045837c  fld        dword ptr [edi + 0x40]
0045837f  fmul       qword ptr [0x4a3cf8]
00458385  fsubp      st(1)
00458387  fstp       dword ptr [esp + 0x20]
0045838b  fld        dword ptr [esp + 0x20]
0045838f  fstp       dword ptr [esp + 4]
00458393  fld        dword ptr [esp + 0x24]
00458397  fstp       dword ptr [esp]
0045839a  call       0x4594b0

// block 0045839f
0045839f  fld        dword ptr [esp + 0x2c]
004583a3  sub        esp, 8
004583a6  fadd       dword ptr [ebx]
004583a8  add        ebx, 0x1c
004583ab  fstp       dword ptr [ebx - 0x1c]
004583ae  fld        dword ptr [ebx - 0x18]
004583b1  fadd       dword ptr [esp + 0x38]
004583b5  fstp       dword ptr [ebx - 0x18]
004583b8  fld        dword ptr [esp + 0x14]
004583bc  fstp       dword ptr [ebx - 0x14]
004583bf  fld        dword ptr [esp + 0x44]
004583c3  fadd       dword ptr [esp + 0x1c]
004583c7  fstp       dword ptr [esp + 0x1c]
004583cb  fld        dword ptr [esp + 0x40]
004583cf  fstp       dword ptr [esp + 4]
004583d3  fld        dword ptr [esp + 0x24]
004583d7  fstp       dword ptr [esp]
004583da  call       0x464640

// block 004583df
004583df  sub        dword ptr [esp + 0x10], 1
004583e4  fstp       dword ptr [esp + 0x1c]
004583e8  jne        0x4582f4

// block 004583ee
004583ee  mov        esi, dword ptr [edi + 0x478]
004583f4  fld        dword ptr [esp + 0x14]
004583f8  mov        eax, dword ptr [ebp + 8]
004583fb  fld        st(0)
004583fd  mov        ecx, 7
00458402  mov        edi, ebx

// block 00458404
00458404  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00458406
00458406  fadd       dword ptr [eax + 0x64]
00458409  fstp       dword ptr [ebx + 0x18]
0045840c  mov        esi, dword ptr [eax + 0x478]
00458412  add        esi, 0x1c
00458415  lea        edi, [ebx + 0x1c]
00458418  mov        ecx, 7

// block 0045841d
0045841d  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045841f
0045841f  fadd       dword ptr [eax + 0x64]
00458422  fstp       dword ptr [ebx + 0x34]
00458425  mov        edi, eax
00458427  jmp        0x4586a1

// block 0045842c
0045842c  cmp        eax, 0xd
0045842f  jne        0x4586a1

// block 00458435
00458435  fld        dword ptr [edi + 0x2c]
00458438  mov        ebx, dword ptr [edi + 0x3fc]
0045843e  fld        dword ptr [edi + 0x24]
00458441  push       ecx
00458442  fmul       qword ptr [0x4a3cf8]
00458448  fsubp      st(1)
0045844a  fstp       dword ptr [esp + 0x1c]
0045844e  fld        dword ptr [esp + 0x1c]
00458452  fstp       dword ptr [esp]
00458455  call       0x4646e0

// block 0045845a
0045845a  fstp       dword ptr [esp + 0x1c]
0045845e  dec        ebx
0045845f  mov        dword ptr [esp + 0x18], ebx
00458463  fild       dword ptr [esp + 0x18]
00458467  mov        eax, dword ptr [edi + 0x3c]
0045846a  mov        ebx, dword ptr [edi + 0x478]
00458470  fstp       dword ptr [esp + 0x18]
00458474  fld        dword ptr [edi + 0x24]
00458477  fld        dword ptr [esp + 0x18]
0045847b  fld        st(0)
0045847d  fdivp      st(2)
0045847f  fxch       st(1)
00458481  fstp       dword ptr [esp + 0x38]
00458485  fldz       
00458487  fstp       dword ptr [esp + 0x14]
0045848b  fidivr     dword ptr [edi + 0x400]
00458491  fstp       dword ptr [esp + 0x3c]
00458495  fld        dword ptr [edi + 0x424]
0045849b  fadd       dword ptr [edi + 0x43c]
004584a1  fstp       dword ptr [esp + 0x20]
004584a5  fld        dword ptr [edi + 0x428]
004584ab  fadd       dword ptr [edi + 0x440]
004584b1  fstp       dword ptr [esp + 0x24]
004584b5  fld        dword ptr [edi + 0x42c]
004584bb  fadd       dword ptr [edi + 0x444]
004584c1  fstp       dword ptr [esp + 0x28]
004584c5  fld        dword ptr [esp + 0x20]
004584c9  fadd       dword ptr [edi + 0x430]
004584cf  fstp       dword ptr [esp + 0x2c]
004584d3  fld        dword ptr [edi + 0x434]
004584d9  fadd       dword ptr [esp + 0x24]
004584dd  fstp       dword ptr [esp + 0x30]
004584e1  fld        dword ptr [edi + 0x438]
004584e7  fadd       dword ptr [esp + 0x28]
004584eb  fstp       dword ptr [esp + 0x34]
004584ef  test       eax, eax
004584f1  je         0x458571

// block 004584f3
004584f3  fld        dword ptr [eax + 0x43c]
004584f9  fadd       dword ptr [eax + 0x424]
004584ff  fstp       dword ptr [esp + 0x20]
00458503  fld        dword ptr [eax + 0x428]
00458509  fadd       dword ptr [eax + 0x440]
0045850f  fstp       dword ptr [esp + 0x24]
00458513  fld        dword ptr [eax + 0x42c]
00458519  fadd       dword ptr [eax + 0x444]
0045851f  fstp       dword ptr [esp + 0x28]
00458523  fld        dword ptr [esp + 0x20]
00458527  fadd       dword ptr [eax + 0x430]
0045852d  fstp       dword ptr [esp + 0x40]
00458531  fld        dword ptr [eax + 0x434]
00458537  fadd       dword ptr [esp + 0x24]
0045853b  fstp       dword ptr [esp + 0x44]
0045853f  fld        dword ptr [eax + 0x438]
00458545  fadd       dword ptr [esp + 0x28]
00458549  fstp       dword ptr [esp + 0x48]
0045854d  fld        dword ptr [esp + 0x2c]
00458551  fadd       dword ptr [esp + 0x40]
00458555  fstp       dword ptr [esp + 0x2c]
00458559  fld        dword ptr [esp + 0x44]
0045855d  fadd       dword ptr [esp + 0x30]
00458561  fstp       dword ptr [esp + 0x30]
00458565  fld        dword ptr [esp + 0x48]
00458569  fadd       dword ptr [esp + 0x34]
0045856d  fstp       dword ptr [esp + 0x34]

// block 00458571
00458571  test       esi, 0x10000
00458577  je         0x458581

// block 00458579
00458579  mov        esi, dword ptr [edi + 0x3c0]
0045857f  jmp        0x458587

// block 00458581
00458581  mov        esi, dword ptr [edi + 0x3bc]

// block 00458587
00458587  mov        eax, dword ptr [edi + 0x3fc]
0045858d  test       eax, eax
0045858f  jle        0x4586a1

// block 00458595
00458595  fld        dword ptr [esp + 0x34]
00458599  mov        dword ptr [esp + 0x10], eax
0045859d  fadd       qword ptr [0x4a3d58]
004585a3  fstp       dword ptr [esp + 0xc]

// block 004585a7
004585a7  fld1       
004585a9  mov        dword ptr [ebx + 0x10], esi
004585ac  fstp       dword ptr [ebx + 0xc]
004585af  sub        esp, 8
004585b2  fld        dword ptr [edi + 0x7c]
004585b5  mov        ecx, ebx
004585b7  fadd       dword ptr [edi + 0x60]
004585ba  fstp       dword ptr [ebx + 0x14]
004585bd  fld        dword ptr [esp + 0x1c]
004585c1  fadd       dword ptr [edi + 0x64]
004585c4  fstp       dword ptr [ebx + 0x18]
004585c7  fld        dword ptr [edi + 0x40]
004585ca  fmul       qword ptr [0x4a3cf8]
004585d0  fadd       dword ptr [edi + 0x44]
004585d3  fstp       dword ptr [esp + 0x20]
004585d7  fld        dword ptr [esp + 0x20]
004585db  fstp       dword ptr [esp + 4]
004585df  fld        dword ptr [esp + 0x24]
004585e3  fstp       dword ptr [esp]
004585e6  call       0x4594b0

// block 004585eb
004585eb  fld        dword ptr [esp + 0x2c]
004585ef  add        ebx, 0x1c
004585f2  fadd       dword ptr [ebx - 0x1c]
004585f5  sub        esp, 8
004585f8  mov        ecx, ebx
004585fa  fstp       dword ptr [ebx - 0x1c]
004585fd  fld        dword ptr [ebx - 0x18]
00458600  fadd       dword ptr [esp + 0x38]
00458604  fstp       dword ptr [ebx - 0x18]
00458607  fld        dword ptr [esp + 0x14]
0045860b  fstp       dword ptr [ebx - 0x14]
0045860e  mov        dword ptr [ebx + 0x10], esi
00458611  fld1       
00458613  fstp       dword ptr [ebx + 0xc]
00458616  fld        dword ptr [edi + 0x84]
0045861c  fadd       dword ptr [edi + 0x60]
0045861f  fstp       dword ptr [ebx + 0x14]
00458622  fld        dword ptr [esp + 0x1c]
00458626  fadd       dword ptr [edi + 0x64]
00458629  fstp       dword ptr [ebx + 0x18]
0045862c  fld        dword ptr [edi + 0x44]
0045862f  fld        dword ptr [edi + 0x40]
00458632  fmul       qword ptr [0x4a3cf8]
00458638  fsubp      st(1)
0045863a  fstp       dword ptr [esp + 0x20]
0045863e  fld        dword ptr [esp + 0x20]
00458642  fstp       dword ptr [esp + 4]
00458646  fld        dword ptr [esp + 0x24]
0045864a  fstp       dword ptr [esp]
0045864d  call       0x4594b0

// block 00458652
00458652  fld        dword ptr [esp + 0x2c]
00458656  sub        esp, 8
00458659  fadd       dword ptr [ebx]
0045865b  add        ebx, 0x1c
0045865e  fstp       dword ptr [ebx - 0x1c]
00458661  fld        dword ptr [ebx - 0x18]
00458664  fadd       dword ptr [esp + 0x38]
00458668  fstp       dword ptr [ebx - 0x18]
0045866b  fld        dword ptr [esp + 0x14]
0045866f  fstp       dword ptr [ebx - 0x14]
00458672  fld        dword ptr [esp + 0x44]
00458676  fadd       dword ptr [esp + 0x1c]
0045867a  fstp       dword ptr [esp + 0x1c]
0045867e  fld        dword ptr [esp + 0x40]
00458682  fstp       dword ptr [esp + 4]
00458686  fld        dword ptr [esp + 0x24]
0045868a  fstp       dword ptr [esp]
0045868d  call       0x464640

// block 00458692
00458692  sub        dword ptr [esp + 0x10], 1
00458697  fstp       dword ptr [esp + 0x1c]
0045869b  jne        0x4585a7

// block 004586a1
004586a1  mov        eax, dword ptr [edi + 0x4a8]
004586a7  test       eax, eax
004586a9  je         0x4586b7

// block 004586ab
004586ab  mov        ecx, edi
004586ad  call       eax

// block 004586af
004586af  test       eax, eax
004586b1  jne        0x455b44

// block 004586b7
004586b7  mov        edx, dword ptr [edi + 0x6c]
004586ba  mov        ecx, dword ptr [edi + 0x74]
004586bd  mov        dword ptr [edi + 0x68], edx
004586c0  fld        dword ptr [ecx]
004586c2  fcomp      qword ptr [0x4a3db0]
004586c8  fnstsw     ax
004586ca  test       ah, 0x41
004586cd  jne        0x458703

// block 004586cf
004586cf  fld        dword ptr [0x4a3dac]
004586d5  fcomp      dword ptr [ecx]
004586d7  fnstsw     ax
004586d9  test       ah, 0x41
004586dc  jne        0x458703

// block 004586de
004586de  fld        dword ptr [edi + 0x70]
004586e1  inc        edx
004586e2  fadd       qword ptr [0x4a3ce0]
004586e8  mov        dword ptr [edi + 0x6c], edx
004586eb  xor        eax, eax
004586ed  fstp       dword ptr [edi + 0x70]
004586f0  fld        dword ptr [esp + 0x6c]
004586f4  fstp       dword ptr [0x4b2ed0]
004586fa  pop        edi
004586fb  pop        esi
004586fc  pop        ebx
004586fd  mov        esp, ebp
004586ff  pop        ebp
00458700  ret        4

// block 00458703
00458703  fld        dword ptr [ecx]
00458705  fadd       dword ptr [edi + 0x70]
00458708  fstp       dword ptr [esp + 0x18]
0045870c  fld        dword ptr [esp + 0x18]
00458710  fst        dword ptr [edi + 0x70]
00458713  call       0x4931e0

// block 00458718
00458718  fld        dword ptr [esp + 0x6c]
0045871c  mov        dword ptr [edi + 0x6c], eax
0045871f  fstp       dword ptr [0x4b2ed0]

// block 00458725
00458725  pop        edi
00458726  pop        esi
00458727  xor        eax, eax
00458729  pop        ebx
0045872a  mov        esp, ebp
0045872c  pop        ebp
0045872d  ret        4
