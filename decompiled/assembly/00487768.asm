
// block 00487768
00487768  mov        edi, edi
0048776a  push       ebp
0048776b  mov        ebp, esp
0048776d  push       esi
0048776e  push       edi
0048776f  mov        edi, dword ptr [ebp + 0x10]
00487772  mov        eax, edi
00487774  sub        eax, 0
00487777  je         0x488d62

// block 0048777d
0048777d  dec        eax
0048777e  je         0x488d51

// block 00487784
00487784  dec        eax
00487785  je         0x488d23

// block 0048778b
0048778b  dec        eax
0048778c  je         0x488cdb

// block 00487792
00487792  dec        eax
00487793  je         0x488c52

// block 00487799
00487799  mov        ecx, dword ptr [ebp + 0xc]
0048779c  mov        eax, dword ptr [ebp + 8]
0048779f  push       ebx
004877a0  push       0x20
004877a2  pop        edx
004877a3  jmp        0x487c1a

// block 004877a8
004877a8  mov        esi, dword ptr [eax]
004877aa  cmp        esi, dword ptr [ecx]
004877ac  je         0x48782a

// block 004877ae
004877ae  movzx      esi, byte ptr [eax]
004877b1  movzx      ebx, byte ptr [ecx]
004877b4  sub        esi, ebx
004877b6  je         0x4877cd

// block 004877b8
004877b8  xor        ebx, ebx
004877ba  test       esi, esi
004877bc  setg       bl
004877bf  lea        ebx, [ebx + ebx - 1]
004877c3  mov        esi, ebx
004877c5  test       esi, esi
004877c7  jne        0x487c36

// block 004877cd
004877cd  movzx      esi, byte ptr [eax + 1]
004877d1  movzx      ebx, byte ptr [ecx + 1]
004877d5  sub        esi, ebx
004877d7  je         0x4877ee

// block 004877d9
004877d9  xor        ebx, ebx
004877db  test       esi, esi
004877dd  setg       bl
004877e0  lea        ebx, [ebx + ebx - 1]
004877e4  mov        esi, ebx
004877e6  test       esi, esi
004877e8  jne        0x487c36

// block 004877ee
004877ee  movzx      esi, byte ptr [eax + 2]
004877f2  movzx      ebx, byte ptr [ecx + 2]
004877f6  sub        esi, ebx
004877f8  je         0x48780f

// block 004877fa
004877fa  xor        ebx, ebx
004877fc  test       esi, esi
004877fe  setg       bl
00487801  lea        ebx, [ebx + ebx - 1]
00487805  mov        esi, ebx
00487807  test       esi, esi
00487809  jne        0x487c36

// block 0048780f
0048780f  movzx      esi, byte ptr [eax + 3]
00487813  movzx      ebx, byte ptr [ecx + 3]
00487817  sub        esi, ebx
00487819  je         0x48782c

// block 0048781b
0048781b  xor        ebx, ebx
0048781d  test       esi, esi
0048781f  setg       bl
00487822  lea        ebx, [ebx + ebx - 1]
00487826  mov        esi, ebx
00487828  jmp        0x48782c

// block 0048782a
0048782a  xor        esi, esi

// block 0048782c
0048782c  test       esi, esi
0048782e  jne        0x487c36

// block 00487834
00487834  mov        esi, dword ptr [eax + 4]
00487837  cmp        esi, dword ptr [ecx + 4]
0048783a  je         0x4878ba

// block 0048783c
0048783c  movzx      esi, byte ptr [eax + 4]
00487840  movzx      ebx, byte ptr [ecx + 4]
00487844  sub        esi, ebx
00487846  je         0x48785d

// block 00487848
00487848  xor        ebx, ebx
0048784a  test       esi, esi
0048784c  setg       bl
0048784f  lea        ebx, [ebx + ebx - 1]
00487853  mov        esi, ebx
00487855  test       esi, esi
00487857  jne        0x487c36

// block 0048785d
0048785d  movzx      esi, byte ptr [eax + 5]
00487861  movzx      ebx, byte ptr [ecx + 5]
00487865  sub        esi, ebx
00487867  je         0x48787e

// block 00487869
00487869  xor        ebx, ebx
0048786b  test       esi, esi
0048786d  setg       bl
00487870  lea        ebx, [ebx + ebx - 1]
00487874  mov        esi, ebx
00487876  test       esi, esi
00487878  jne        0x487c36

// block 0048787e
0048787e  movzx      esi, byte ptr [eax + 6]
00487882  movzx      ebx, byte ptr [ecx + 6]
00487886  sub        esi, ebx
00487888  je         0x48789f

// block 0048788a
0048788a  xor        ebx, ebx
0048788c  test       esi, esi
0048788e  setg       bl
00487891  lea        ebx, [ebx + ebx - 1]
00487895  mov        esi, ebx
00487897  test       esi, esi
00487899  jne        0x487c36

// block 0048789f
0048789f  movzx      esi, byte ptr [eax + 7]
004878a3  movzx      ebx, byte ptr [ecx + 7]
004878a7  sub        esi, ebx
004878a9  je         0x4878bc

// block 004878ab
004878ab  xor        ebx, ebx
004878ad  test       esi, esi
004878af  setg       bl
004878b2  lea        ebx, [ebx + ebx - 1]
004878b6  mov        esi, ebx
004878b8  jmp        0x4878bc

// block 004878ba
004878ba  xor        esi, esi

// block 004878bc
004878bc  test       esi, esi
004878be  jne        0x487c36

// block 004878c4
004878c4  mov        esi, dword ptr [eax + 8]
004878c7  cmp        esi, dword ptr [ecx + 8]
004878ca  je         0x48794a

// block 004878cc
004878cc  movzx      esi, byte ptr [eax + 8]
004878d0  movzx      ebx, byte ptr [ecx + 8]
004878d4  sub        esi, ebx
004878d6  je         0x4878ed

// block 004878d8
004878d8  xor        ebx, ebx
004878da  test       esi, esi
004878dc  setg       bl
004878df  lea        ebx, [ebx + ebx - 1]
004878e3  mov        esi, ebx
004878e5  test       esi, esi
004878e7  jne        0x487c36

// block 004878ed
004878ed  movzx      esi, byte ptr [eax + 9]
004878f1  movzx      ebx, byte ptr [ecx + 9]
004878f5  sub        esi, ebx
004878f7  je         0x48790e

// block 004878f9
004878f9  xor        ebx, ebx
004878fb  test       esi, esi
004878fd  setg       bl
00487900  lea        ebx, [ebx + ebx - 1]
00487904  mov        esi, ebx
00487906  test       esi, esi
00487908  jne        0x487c36

// block 0048790e
0048790e  movzx      esi, byte ptr [eax + 0xa]
00487912  movzx      ebx, byte ptr [ecx + 0xa]
00487916  sub        esi, ebx
00487918  je         0x48792f

// block 0048791a
0048791a  xor        ebx, ebx
0048791c  test       esi, esi
0048791e  setg       bl
00487921  lea        ebx, [ebx + ebx - 1]
00487925  mov        esi, ebx
00487927  test       esi, esi
00487929  jne        0x487c36

// block 0048792f
0048792f  movzx      esi, byte ptr [eax + 0xb]
00487933  movzx      ebx, byte ptr [ecx + 0xb]
00487937  sub        esi, ebx
00487939  je         0x48794c

// block 0048793b
0048793b  xor        ebx, ebx
0048793d  test       esi, esi
0048793f  setg       bl
00487942  lea        ebx, [ebx + ebx - 1]
00487946  mov        esi, ebx
00487948  jmp        0x48794c

// block 0048794a
0048794a  xor        esi, esi

// block 0048794c
0048794c  test       esi, esi
0048794e  jne        0x487c36

// block 00487954
00487954  mov        esi, dword ptr [eax + 0xc]
00487957  cmp        esi, dword ptr [ecx + 0xc]
0048795a  je         0x4879da

// block 0048795c
0048795c  movzx      esi, byte ptr [eax + 0xc]
00487960  movzx      ebx, byte ptr [ecx + 0xc]
00487964  sub        esi, ebx
00487966  je         0x48797d

// block 00487968
00487968  xor        ebx, ebx
0048796a  test       esi, esi
0048796c  setg       bl
0048796f  lea        ebx, [ebx + ebx - 1]
00487973  mov        esi, ebx
00487975  test       esi, esi
00487977  jne        0x487c36

// block 0048797d
0048797d  movzx      esi, byte ptr [eax + 0xd]
00487981  movzx      ebx, byte ptr [ecx + 0xd]
00487985  sub        esi, ebx
00487987  je         0x48799e

// block 00487989
00487989  xor        ebx, ebx
0048798b  test       esi, esi
0048798d  setg       bl
00487990  lea        ebx, [ebx + ebx - 1]
00487994  mov        esi, ebx
00487996  test       esi, esi
00487998  jne        0x487c36

// block 0048799e
0048799e  movzx      esi, byte ptr [eax + 0xe]
004879a2  movzx      ebx, byte ptr [ecx + 0xe]
004879a6  sub        esi, ebx
004879a8  je         0x4879bf

// block 004879aa
004879aa  xor        ebx, ebx
004879ac  test       esi, esi
004879ae  setg       bl
004879b1  lea        ebx, [ebx + ebx - 1]
004879b5  mov        esi, ebx
004879b7  test       esi, esi
004879b9  jne        0x487c36

// block 004879bf
004879bf  movzx      esi, byte ptr [eax + 0xf]
004879c3  movzx      ebx, byte ptr [ecx + 0xf]
004879c7  sub        esi, ebx
004879c9  je         0x4879dc

// block 004879cb
004879cb  xor        ebx, ebx
004879cd  test       esi, esi
004879cf  setg       bl
004879d2  lea        ebx, [ebx + ebx - 1]
004879d6  mov        esi, ebx
004879d8  jmp        0x4879dc

// block 004879da
004879da  xor        esi, esi

// block 004879dc
004879dc  test       esi, esi
004879de  jne        0x487c36

// block 004879e4
004879e4  mov        esi, dword ptr [eax + 0x10]
004879e7  cmp        esi, dword ptr [ecx + 0x10]
004879ea  je         0x487a6a

// block 004879ec
004879ec  movzx      ebx, byte ptr [ecx + 0x10]
004879f0  movzx      esi, byte ptr [eax + 0x10]
004879f4  sub        esi, ebx
004879f6  je         0x487a0d

// block 004879f8
004879f8  xor        ebx, ebx
004879fa  test       esi, esi
004879fc  setg       bl
004879ff  lea        ebx, [ebx + ebx - 1]
00487a03  mov        esi, ebx
00487a05  test       esi, esi
00487a07  jne        0x487c36

// block 00487a0d
00487a0d  movzx      esi, byte ptr [eax + 0x11]
00487a11  movzx      ebx, byte ptr [ecx + 0x11]
00487a15  sub        esi, ebx
00487a17  je         0x487a2e

// block 00487a19
00487a19  xor        ebx, ebx
00487a1b  test       esi, esi
00487a1d  setg       bl
00487a20  lea        ebx, [ebx + ebx - 1]
00487a24  mov        esi, ebx
00487a26  test       esi, esi
00487a28  jne        0x487c36

// block 00487a2e
00487a2e  movzx      esi, byte ptr [eax + 0x12]
00487a32  movzx      ebx, byte ptr [ecx + 0x12]
00487a36  sub        esi, ebx
00487a38  je         0x487a4f

// block 00487a3a
00487a3a  xor        ebx, ebx
00487a3c  test       esi, esi
00487a3e  setg       bl
00487a41  lea        ebx, [ebx + ebx - 1]
00487a45  mov        esi, ebx
00487a47  test       esi, esi
00487a49  jne        0x487c36

// block 00487a4f
00487a4f  movzx      esi, byte ptr [eax + 0x13]
00487a53  movzx      ebx, byte ptr [ecx + 0x13]
00487a57  sub        esi, ebx
00487a59  je         0x487a6c

// block 00487a5b
00487a5b  xor        ebx, ebx
00487a5d  test       esi, esi
00487a5f  setg       bl
00487a62  lea        ebx, [ebx + ebx - 1]
00487a66  mov        esi, ebx
00487a68  jmp        0x487a6c

// block 00487a6a
00487a6a  xor        esi, esi

// block 00487a6c
00487a6c  test       esi, esi
00487a6e  jne        0x487c36

// block 00487a74
00487a74  mov        esi, dword ptr [eax + 0x14]
00487a77  cmp        esi, dword ptr [ecx + 0x14]
00487a7a  je         0x487afa

// block 00487a7c
00487a7c  movzx      esi, byte ptr [eax + 0x14]
00487a80  movzx      ebx, byte ptr [ecx + 0x14]
00487a84  sub        esi, ebx
00487a86  je         0x487a9d

// block 00487a88
00487a88  xor        ebx, ebx
00487a8a  test       esi, esi
00487a8c  setg       bl
00487a8f  lea        ebx, [ebx + ebx - 1]
00487a93  mov        esi, ebx
00487a95  test       esi, esi
00487a97  jne        0x487c36

// block 00487a9d
00487a9d  movzx      esi, byte ptr [eax + 0x15]
00487aa1  movzx      ebx, byte ptr [ecx + 0x15]
00487aa5  sub        esi, ebx
00487aa7  je         0x487abe

// block 00487aa9
00487aa9  xor        ebx, ebx
00487aab  test       esi, esi
00487aad  setg       bl
00487ab0  lea        ebx, [ebx + ebx - 1]
00487ab4  mov        esi, ebx
00487ab6  test       esi, esi
00487ab8  jne        0x487c36

// block 00487abe
00487abe  movzx      esi, byte ptr [eax + 0x16]
00487ac2  movzx      ebx, byte ptr [ecx + 0x16]
00487ac6  sub        esi, ebx
00487ac8  je         0x487adf

// block 00487aca
00487aca  xor        ebx, ebx
00487acc  test       esi, esi
00487ace  setg       bl
00487ad1  lea        ebx, [ebx + ebx - 1]
00487ad5  mov        esi, ebx
00487ad7  test       esi, esi
00487ad9  jne        0x487c36

// block 00487adf
00487adf  movzx      esi, byte ptr [eax + 0x17]
00487ae3  movzx      ebx, byte ptr [ecx + 0x17]
00487ae7  sub        esi, ebx
00487ae9  je         0x487afc

// block 00487aeb
00487aeb  xor        ebx, ebx
00487aed  test       esi, esi
00487aef  setg       bl
00487af2  lea        ebx, [ebx + ebx - 1]
00487af6  mov        esi, ebx
00487af8  jmp        0x487afc

// block 00487afa
00487afa  xor        esi, esi

// block 00487afc
00487afc  test       esi, esi
00487afe  jne        0x487c36

// block 00487b04
00487b04  mov        esi, dword ptr [eax + 0x18]
00487b07  cmp        esi, dword ptr [ecx + 0x18]
00487b0a  je         0x487b8a

// block 00487b0c
00487b0c  movzx      esi, byte ptr [eax + 0x18]
00487b10  movzx      ebx, byte ptr [ecx + 0x18]
00487b14  sub        esi, ebx
00487b16  je         0x487b2d

// block 00487b18
00487b18  xor        ebx, ebx
00487b1a  test       esi, esi
00487b1c  setg       bl
00487b1f  lea        ebx, [ebx + ebx - 1]
00487b23  mov        esi, ebx
00487b25  test       esi, esi
00487b27  jne        0x487c36

// block 00487b2d
00487b2d  movzx      esi, byte ptr [eax + 0x19]
00487b31  movzx      ebx, byte ptr [ecx + 0x19]
00487b35  sub        esi, ebx
00487b37  je         0x487b4e

// block 00487b39
00487b39  xor        ebx, ebx
00487b3b  test       esi, esi
00487b3d  setg       bl
00487b40  lea        ebx, [ebx + ebx - 1]
00487b44  mov        esi, ebx
00487b46  test       esi, esi
00487b48  jne        0x487c36

// block 00487b4e
00487b4e  movzx      esi, byte ptr [eax + 0x1a]
00487b52  movzx      ebx, byte ptr [ecx + 0x1a]
00487b56  sub        esi, ebx
00487b58  je         0x487b6f

// block 00487b5a
00487b5a  xor        ebx, ebx
00487b5c  test       esi, esi
00487b5e  setg       bl
00487b61  lea        ebx, [ebx + ebx - 1]
00487b65  mov        esi, ebx
00487b67  test       esi, esi
00487b69  jne        0x487c36

// block 00487b6f
00487b6f  movzx      esi, byte ptr [eax + 0x1b]
00487b73  movzx      ebx, byte ptr [ecx + 0x1b]
00487b77  sub        esi, ebx
00487b79  je         0x487b8c

// block 00487b7b
00487b7b  xor        ebx, ebx
00487b7d  test       esi, esi
00487b7f  setg       bl
00487b82  lea        ebx, [ebx + ebx - 1]
00487b86  mov        esi, ebx
00487b88  jmp        0x487b8c

// block 00487b8a
00487b8a  xor        esi, esi

// block 00487b8c
00487b8c  test       esi, esi
00487b8e  jne        0x487c36

// block 00487b94
00487b94  mov        esi, dword ptr [eax + 0x1c]
00487b97  cmp        esi, dword ptr [ecx + 0x1c]
00487b9a  je         0x487c0e

// block 00487b9c
00487b9c  movzx      esi, byte ptr [eax + 0x1c]
00487ba0  movzx      ebx, byte ptr [ecx + 0x1c]
00487ba4  sub        esi, ebx
00487ba6  je         0x487bb9

// block 00487ba8
00487ba8  xor        ebx, ebx
00487baa  test       esi, esi
00487bac  setg       bl
00487baf  lea        ebx, [ebx + ebx - 1]
00487bb3  mov        esi, ebx
00487bb5  test       esi, esi
00487bb7  jne        0x487c36

// block 00487bb9
00487bb9  movzx      esi, byte ptr [eax + 0x1d]
00487bbd  movzx      ebx, byte ptr [ecx + 0x1d]
00487bc1  sub        esi, ebx
00487bc3  je         0x487bd6

// block 00487bc5
00487bc5  xor        ebx, ebx
00487bc7  test       esi, esi
00487bc9  setg       bl
00487bcc  lea        ebx, [ebx + ebx - 1]
00487bd0  mov        esi, ebx
00487bd2  test       esi, esi
00487bd4  jne        0x487c36

// block 00487bd6
00487bd6  movzx      esi, byte ptr [eax + 0x1e]
00487bda  movzx      ebx, byte ptr [ecx + 0x1e]
00487bde  sub        esi, ebx
00487be0  je         0x487bf3

// block 00487be2
00487be2  xor        ebx, ebx
00487be4  test       esi, esi
00487be6  setg       bl
00487be9  lea        ebx, [ebx + ebx - 1]
00487bed  mov        esi, ebx
00487bef  test       esi, esi
00487bf1  jne        0x487c36

// block 00487bf3
00487bf3  movzx      esi, byte ptr [eax + 0x1f]
00487bf7  movzx      ebx, byte ptr [ecx + 0x1f]
00487bfb  sub        esi, ebx
00487bfd  je         0x487c10

// block 00487bff
00487bff  xor        ebx, ebx
00487c01  test       esi, esi
00487c03  setg       bl
00487c06  lea        ebx, [ebx + ebx - 1]
00487c0a  mov        esi, ebx
00487c0c  jmp        0x487c10

// block 00487c0e
00487c0e  xor        esi, esi

// block 00487c10
00487c10  test       esi, esi
00487c12  jne        0x487c36

// block 00487c14
00487c14  add        eax, edx
00487c16  add        ecx, edx
00487c18  sub        edi, edx

// block 00487c1a
00487c1a  cmp        edi, edx
00487c1c  jae        0x4877a8

// block 00487c22
00487c22  add        eax, edi
00487c24  add        ecx, edi
00487c26  cmp        edi, 0x1f
00487c29  ja         0x488009

// block 00487c2f
00487c2f  jmp        dword ptr [edi*4 + 0x488d68]

// block 00487c36
00487c36  mov        eax, esi
00487c38  jmp        0x48800b

// block 00487c3d
00487c3d  mov        edx, dword ptr [eax - 0x1c]
00487c40  cmp        edx, dword ptr [ecx - 0x1c]
00487c43  je         0x487cb6

// block 00487c45
00487c45  movzx      esi, dl
00487c48  movzx      edx, byte ptr [ecx - 0x1c]
00487c4c  sub        esi, edx
00487c4e  je         0x487c61

// block 00487c50
00487c50  xor        edx, edx
00487c52  test       esi, esi
00487c54  setg       dl
00487c57  lea        edx, [edx + edx - 1]
00487c5b  mov        esi, edx
00487c5d  test       esi, esi
00487c5f  jne        0x487c36

// block 00487c61
00487c61  movzx      esi, byte ptr [eax - 0x1b]
00487c65  movzx      edx, byte ptr [ecx - 0x1b]
00487c69  sub        esi, edx
00487c6b  je         0x487c7e

// block 00487c6d
00487c6d  xor        edx, edx
00487c6f  test       esi, esi
00487c71  setg       dl
00487c74  lea        edx, [edx + edx - 1]
00487c78  mov        esi, edx
00487c7a  test       esi, esi
00487c7c  jne        0x487c36

// block 00487c7e
00487c7e  movzx      esi, byte ptr [eax - 0x1a]
00487c82  movzx      edx, byte ptr [ecx - 0x1a]
00487c86  sub        esi, edx
00487c88  je         0x487c9b

// block 00487c8a
00487c8a  xor        edx, edx
00487c8c  test       esi, esi
00487c8e  setg       dl
00487c91  lea        edx, [edx + edx - 1]
00487c95  mov        esi, edx
00487c97  test       esi, esi
00487c99  jne        0x487c36

// block 00487c9b
00487c9b  movzx      esi, byte ptr [eax - 0x19]
00487c9f  movzx      edx, byte ptr [ecx - 0x19]
00487ca3  sub        esi, edx
00487ca5  je         0x487cb8

// block 00487ca7
00487ca7  xor        edx, edx
00487ca9  test       esi, esi
00487cab  setg       dl
00487cae  lea        edx, [edx + edx - 1]
00487cb2  mov        esi, edx
00487cb4  jmp        0x487cb8

// block 00487cb6
00487cb6  xor        esi, esi

// block 00487cb8
00487cb8  test       esi, esi
00487cba  jne        0x487c36

// block 00487cc0
00487cc0  mov        edx, dword ptr [eax - 0x18]
00487cc3  cmp        edx, dword ptr [ecx - 0x18]
00487cc6  je         0x487d45

// block 00487cc8
00487cc8  movzx      esi, dl
00487ccb  movzx      edx, byte ptr [ecx - 0x18]
00487ccf  sub        esi, edx
00487cd1  je         0x487ce8

// block 00487cd3
00487cd3  xor        edx, edx
00487cd5  test       esi, esi
00487cd7  setg       dl
00487cda  lea        edx, [edx + edx - 1]
00487cde  mov        esi, edx
00487ce0  test       esi, esi
00487ce2  jne        0x487c36

// block 00487ce8
00487ce8  movzx      esi, byte ptr [eax - 0x17]
00487cec  movzx      edx, byte ptr [ecx - 0x17]
00487cf0  sub        esi, edx
00487cf2  je         0x487d09

// block 00487cf4
00487cf4  xor        edx, edx
00487cf6  test       esi, esi
00487cf8  setg       dl
00487cfb  lea        edx, [edx + edx - 1]
00487cff  mov        esi, edx
00487d01  test       esi, esi
00487d03  jne        0x487c36

// block 00487d09
00487d09  movzx      esi, byte ptr [eax - 0x16]
00487d0d  movzx      edx, byte ptr [ecx - 0x16]
00487d11  sub        esi, edx
00487d13  je         0x487d2a

// block 00487d15
00487d15  xor        edx, edx
00487d17  test       esi, esi
00487d19  setg       dl
00487d1c  lea        edx, [edx + edx - 1]
00487d20  mov        esi, edx
00487d22  test       esi, esi
00487d24  jne        0x487c36

// block 00487d2a
00487d2a  movzx      esi, byte ptr [eax - 0x15]
00487d2e  movzx      edx, byte ptr [ecx - 0x15]
00487d32  sub        esi, edx
00487d34  je         0x487d47

// block 00487d36
00487d36  xor        edx, edx
00487d38  test       esi, esi
00487d3a  setg       dl
00487d3d  lea        edx, [edx + edx - 1]
00487d41  mov        esi, edx
00487d43  jmp        0x487d47

// block 00487d45
00487d45  xor        esi, esi

// block 00487d47
00487d47  test       esi, esi
00487d49  jne        0x487c36

// block 00487d4f
00487d4f  mov        edx, dword ptr [eax - 0x14]
00487d52  cmp        edx, dword ptr [ecx - 0x14]
00487d55  je         0x487dd4

// block 00487d57
00487d57  movzx      esi, dl
00487d5a  movzx      edx, byte ptr [ecx - 0x14]
00487d5e  sub        esi, edx
00487d60  je         0x487d77

// block 00487d62
00487d62  xor        edx, edx
00487d64  test       esi, esi
00487d66  setg       dl
00487d69  lea        edx, [edx + edx - 1]
00487d6d  mov        esi, edx
00487d6f  test       esi, esi
00487d71  jne        0x487c36

// block 00487d77
00487d77  movzx      esi, byte ptr [eax - 0x13]
00487d7b  movzx      edx, byte ptr [ecx - 0x13]
00487d7f  sub        esi, edx
00487d81  je         0x487d98

// block 00487d83
00487d83  xor        edx, edx
00487d85  test       esi, esi
00487d87  setg       dl
00487d8a  lea        edx, [edx + edx - 1]
00487d8e  mov        esi, edx
00487d90  test       esi, esi
00487d92  jne        0x487c36

// block 00487d98
00487d98  movzx      esi, byte ptr [eax - 0x12]
00487d9c  movzx      edx, byte ptr [ecx - 0x12]
00487da0  sub        esi, edx
00487da2  je         0x487db9

// block 00487da4
00487da4  xor        edx, edx
00487da6  test       esi, esi
00487da8  setg       dl
00487dab  lea        edx, [edx + edx - 1]
00487daf  mov        esi, edx
00487db1  test       esi, esi
00487db3  jne        0x487c36

// block 00487db9
00487db9  movzx      esi, byte ptr [eax - 0x11]
00487dbd  movzx      edx, byte ptr [ecx - 0x11]
00487dc1  sub        esi, edx
00487dc3  je         0x487dd6

// block 00487dc5
00487dc5  xor        edx, edx
00487dc7  test       esi, esi
00487dc9  setg       dl
00487dcc  lea        edx, [edx + edx - 1]
00487dd0  mov        esi, edx
00487dd2  jmp        0x487dd6

// block 00487dd4
00487dd4  xor        esi, esi

// block 00487dd6
00487dd6  test       esi, esi
00487dd8  jne        0x487c36

// block 00487dde
00487dde  mov        edx, dword ptr [eax - 0x10]
00487de1  cmp        edx, dword ptr [ecx - 0x10]
00487de4  je         0x487e63

// block 00487de6
00487de6  movzx      esi, dl
00487de9  movzx      edx, byte ptr [ecx - 0x10]
00487ded  sub        esi, edx
00487def  je         0x487e06

// block 00487df1
00487df1  xor        edx, edx
00487df3  test       esi, esi
00487df5  setg       dl
00487df8  lea        edx, [edx + edx - 1]
00487dfc  mov        esi, edx
00487dfe  test       esi, esi
00487e00  jne        0x487c36

// block 00487e06
00487e06  movzx      esi, byte ptr [eax - 0xf]
00487e0a  movzx      edx, byte ptr [ecx - 0xf]
00487e0e  sub        esi, edx
00487e10  je         0x487e27

// block 00487e12
00487e12  xor        edx, edx
00487e14  test       esi, esi
00487e16  setg       dl
00487e19  lea        edx, [edx + edx - 1]
00487e1d  mov        esi, edx
00487e1f  test       esi, esi
00487e21  jne        0x487c36

// block 00487e27
00487e27  movzx      esi, byte ptr [eax - 0xe]
00487e2b  movzx      edx, byte ptr [ecx - 0xe]
00487e2f  sub        esi, edx
00487e31  je         0x487e48

// block 00487e33
00487e33  xor        edx, edx
00487e35  test       esi, esi
00487e37  setg       dl
00487e3a  lea        edx, [edx + edx - 1]
00487e3e  mov        esi, edx
00487e40  test       esi, esi
00487e42  jne        0x487c36

// block 00487e48
00487e48  movzx      esi, byte ptr [eax - 0xd]
00487e4c  movzx      edx, byte ptr [ecx - 0xd]
00487e50  sub        esi, edx
00487e52  je         0x487e65

// block 00487e54
00487e54  xor        edx, edx
00487e56  test       esi, esi
00487e58  setg       dl
00487e5b  lea        edx, [edx + edx - 1]
00487e5f  mov        esi, edx
00487e61  jmp        0x487e65

// block 00487e63
00487e63  xor        esi, esi

// block 00487e65
00487e65  test       esi, esi
00487e67  jne        0x487c36

// block 00487e6d
00487e6d  mov        edx, dword ptr [eax - 0xc]
00487e70  cmp        edx, dword ptr [ecx - 0xc]
00487e73  je         0x487ef3

// block 00487e75
00487e75  movzx      edx, byte ptr [ecx - 0xc]
00487e79  movzx      esi, byte ptr [eax - 0xc]
00487e7d  sub        esi, edx
00487e7f  je         0x487e96

// block 00487e81
00487e81  xor        edx, edx
00487e83  test       esi, esi
00487e85  setg       dl
00487e88  lea        edx, [edx + edx - 1]
00487e8c  mov        esi, edx
00487e8e  test       esi, esi
00487e90  jne        0x487c36

// block 00487e96
00487e96  movzx      esi, byte ptr [eax - 0xb]
00487e9a  movzx      edx, byte ptr [ecx - 0xb]
00487e9e  sub        esi, edx
00487ea0  je         0x487eb7

// block 00487ea2
00487ea2  xor        edx, edx
00487ea4  test       esi, esi
00487ea6  setg       dl
00487ea9  lea        edx, [edx + edx - 1]
00487ead  mov        esi, edx
00487eaf  test       esi, esi
00487eb1  jne        0x487c36

// block 00487eb7
00487eb7  movzx      esi, byte ptr [eax - 0xa]
00487ebb  movzx      edx, byte ptr [ecx - 0xa]
00487ebf  sub        esi, edx
00487ec1  je         0x487ed8

// block 00487ec3
00487ec3  xor        edx, edx
00487ec5  test       esi, esi
00487ec7  setg       dl
00487eca  lea        edx, [edx + edx - 1]
00487ece  mov        esi, edx
00487ed0  test       esi, esi
00487ed2  jne        0x487c36

// block 00487ed8
00487ed8  movzx      esi, byte ptr [eax - 9]
00487edc  movzx      edx, byte ptr [ecx - 9]
00487ee0  sub        esi, edx
00487ee2  je         0x487ef5

// block 00487ee4
00487ee4  xor        edx, edx
00487ee6  test       esi, esi
00487ee8  setg       dl
00487eeb  lea        edx, [edx + edx - 1]
00487eef  mov        esi, edx
00487ef1  jmp        0x487ef5

// block 00487ef3
00487ef3  xor        esi, esi

// block 00487ef5
00487ef5  test       esi, esi
00487ef7  jne        0x487c36

// block 00487efd
00487efd  mov        edx, dword ptr [eax - 8]
00487f00  cmp        edx, dword ptr [ecx - 8]
00487f03  je         0x487f82

// block 00487f05
00487f05  movzx      esi, dl
00487f08  movzx      edx, byte ptr [ecx - 8]
00487f0c  sub        esi, edx
00487f0e  je         0x487f25

// block 00487f10
00487f10  xor        edx, edx
00487f12  test       esi, esi
00487f14  setg       dl
00487f17  lea        edx, [edx + edx - 1]
00487f1b  mov        esi, edx
00487f1d  test       esi, esi
00487f1f  jne        0x487c36

// block 00487f25
00487f25  movzx      esi, byte ptr [eax - 7]
00487f29  movzx      edx, byte ptr [ecx - 7]
00487f2d  sub        esi, edx
00487f2f  je         0x487f46

// block 00487f31
00487f31  xor        edx, edx
00487f33  test       esi, esi
00487f35  setg       dl
00487f38  lea        edx, [edx + edx - 1]
00487f3c  mov        esi, edx
00487f3e  test       esi, esi
00487f40  jne        0x487c36

// block 00487f46
00487f46  movzx      esi, byte ptr [eax - 6]
00487f4a  movzx      edx, byte ptr [ecx - 6]
00487f4e  sub        esi, edx
00487f50  je         0x487f67

// block 00487f52
00487f52  xor        edx, edx
00487f54  test       esi, esi
00487f56  setg       dl
00487f59  lea        edx, [edx + edx - 1]
00487f5d  mov        esi, edx
00487f5f  test       esi, esi
00487f61  jne        0x487c36

// block 00487f67
00487f67  movzx      esi, byte ptr [eax - 5]
00487f6b  movzx      edx, byte ptr [ecx - 5]
00487f6f  sub        esi, edx
00487f71  je         0x487f84

// block 00487f73
00487f73  xor        edx, edx
00487f75  test       esi, esi
00487f77  setg       dl
00487f7a  lea        edx, [edx + edx - 1]
00487f7e  mov        esi, edx
00487f80  jmp        0x487f84

// block 00487f82
00487f82  xor        esi, esi

// block 00487f84
00487f84  test       esi, esi
00487f86  jne        0x487c36

// block 00487f8c
00487f8c  mov        edx, dword ptr [eax - 4]
00487f8f  cmp        edx, dword ptr [ecx - 4]
00487f92  je         0x488003

// block 00487f94
00487f94  movzx      esi, dl
00487f97  movzx      edx, byte ptr [ecx - 4]
00487f9b  sub        esi, edx
00487f9d  je         0x487fae

// block 00487f9f
00487f9f  xor        edx, edx
00487fa1  test       esi, esi
00487fa3  setg       dl
00487fa6  lea        edx, [edx + edx - 1]
00487faa  test       edx, edx
00487fac  jne        0x487fe4

// block 00487fae
00487fae  movzx      esi, byte ptr [eax - 3]
00487fb2  movzx      edx, byte ptr [ecx - 3]
00487fb6  sub        esi, edx
00487fb8  je         0x487fc9

// block 00487fba
00487fba  xor        edx, edx
00487fbc  test       esi, esi
00487fbe  setg       dl
00487fc1  lea        edx, [edx + edx - 1]
00487fc5  test       edx, edx
00487fc7  jne        0x487fe4

// block 00487fc9
00487fc9  movzx      esi, byte ptr [eax - 2]
00487fcd  movzx      edx, byte ptr [ecx - 2]
00487fd1  sub        esi, edx
00487fd3  je         0x487fe8

// block 00487fd5
00487fd5  xor        edx, edx
00487fd7  test       esi, esi
00487fd9  setg       dl
00487fdc  lea        edx, [edx + edx - 1]
00487fe0  test       edx, edx
00487fe2  je         0x487fe8

// block 00487fe4
00487fe4  mov        eax, edx
00487fe6  jmp        0x488005

// block 00487fe8
00487fe8  movzx      eax, byte ptr [eax - 1]
00487fec  movzx      ecx, byte ptr [ecx - 1]
00487ff0  sub        eax, ecx
00487ff2  je         0x488005

// block 00487ff4
00487ff4  xor        ecx, ecx
00487ff6  test       eax, eax
00487ff8  setg       cl
00487ffb  lea        ecx, [ecx + ecx - 1]
00487fff  mov        eax, ecx
00488001  jmp        0x488005

// block 00488003
00488003  xor        eax, eax

// block 00488005
00488005  test       eax, eax
00488007  jne        0x48800b

// block 00488009
00488009  xor        eax, eax

// block 0048800b
0048800b  pop        ebx
0048800c  jmp        0x488d64

// block 00488011
00488011  mov        edx, dword ptr [eax - 0x1d]
00488014  cmp        edx, dword ptr [ecx - 0x1d]
00488017  je         0x488096

// block 00488019
00488019  movzx      esi, dl
0048801c  movzx      edx, byte ptr [ecx - 0x1d]
00488020  sub        esi, edx
00488022  je         0x488039

// block 00488024
00488024  xor        edx, edx
00488026  test       esi, esi
00488028  setg       dl
0048802b  lea        edx, [edx + edx - 1]
0048802f  mov        esi, edx
00488031  test       esi, esi
00488033  jne        0x487c36

// block 00488039
00488039  movzx      esi, byte ptr [eax - 0x1c]
0048803d  movzx      edx, byte ptr [ecx - 0x1c]
00488041  sub        esi, edx
00488043  je         0x48805a

// block 00488045
00488045  xor        edx, edx
00488047  test       esi, esi
00488049  setg       dl
0048804c  lea        edx, [edx + edx - 1]
00488050  mov        esi, edx
00488052  test       esi, esi
00488054  jne        0x487c36

// block 0048805a
0048805a  movzx      esi, byte ptr [eax - 0x1b]
0048805e  movzx      edx, byte ptr [ecx - 0x1b]
00488062  sub        esi, edx
00488064  je         0x48807b

// block 00488066
00488066  xor        edx, edx
00488068  test       esi, esi
0048806a  setg       dl
0048806d  lea        edx, [edx + edx - 1]
00488071  mov        esi, edx
00488073  test       esi, esi
00488075  jne        0x487c36

// block 0048807b
0048807b  movzx      esi, byte ptr [eax - 0x1a]
0048807f  movzx      edx, byte ptr [ecx - 0x1a]
00488083  sub        esi, edx
00488085  je         0x488098

// block 00488087
00488087  xor        edx, edx
00488089  test       esi, esi
0048808b  setg       dl
0048808e  lea        edx, [edx + edx - 1]
00488092  mov        esi, edx
00488094  jmp        0x488098

// block 00488096
00488096  xor        esi, esi

// block 00488098
00488098  test       esi, esi
0048809a  jne        0x487c36

// block 004880a0
004880a0  mov        edx, dword ptr [eax - 0x19]
004880a3  cmp        edx, dword ptr [ecx - 0x19]
004880a6  je         0x488125

// block 004880a8
004880a8  movzx      esi, dl
004880ab  movzx      edx, byte ptr [ecx - 0x19]
004880af  sub        esi, edx
004880b1  je         0x4880c8

// block 004880b3
004880b3  xor        edx, edx
004880b5  test       esi, esi
004880b7  setg       dl
004880ba  lea        edx, [edx + edx - 1]
004880be  mov        esi, edx
004880c0  test       esi, esi
004880c2  jne        0x487c36

// block 004880c8
004880c8  movzx      esi, byte ptr [eax - 0x18]
004880cc  movzx      edx, byte ptr [ecx - 0x18]
004880d0  sub        esi, edx
004880d2  je         0x4880e9

// block 004880d4
004880d4  xor        edx, edx
004880d6  test       esi, esi
004880d8  setg       dl
004880db  lea        edx, [edx + edx - 1]
004880df  mov        esi, edx
004880e1  test       esi, esi
004880e3  jne        0x487c36

// block 004880e9
004880e9  movzx      esi, byte ptr [eax - 0x17]
004880ed  movzx      edx, byte ptr [ecx - 0x17]
004880f1  sub        esi, edx
004880f3  je         0x48810a

// block 004880f5
004880f5  xor        edx, edx
004880f7  test       esi, esi
004880f9  setg       dl
004880fc  lea        edx, [edx + edx - 1]
00488100  mov        esi, edx
00488102  test       esi, esi
00488104  jne        0x487c36

// block 0048810a
0048810a  movzx      esi, byte ptr [eax - 0x16]
0048810e  movzx      edx, byte ptr [ecx - 0x16]
00488112  sub        esi, edx
00488114  je         0x488127

// block 00488116
00488116  xor        edx, edx
00488118  test       esi, esi
0048811a  setg       dl
0048811d  lea        edx, [edx + edx - 1]
00488121  mov        esi, edx
00488123  jmp        0x488127

// block 00488125
00488125  xor        esi, esi

// block 00488127
00488127  test       esi, esi
00488129  jne        0x487c36

// block 0048812f
0048812f  mov        edx, dword ptr [eax - 0x15]
00488132  cmp        edx, dword ptr [ecx - 0x15]
00488135  je         0x4881b4

// block 00488137
00488137  movzx      esi, dl
0048813a  movzx      edx, byte ptr [ecx - 0x15]
0048813e  sub        esi, edx
00488140  je         0x488157

// block 00488142
00488142  xor        edx, edx
00488144  test       esi, esi
00488146  setg       dl
00488149  lea        edx, [edx + edx - 1]
0048814d  mov        esi, edx
0048814f  test       esi, esi
00488151  jne        0x487c36

// block 00488157
00488157  movzx      esi, byte ptr [eax - 0x14]
0048815b  movzx      edx, byte ptr [ecx - 0x14]
0048815f  sub        esi, edx
00488161  je         0x488178

// block 00488163
00488163  xor        edx, edx
00488165  test       esi, esi
00488167  setg       dl
0048816a  lea        edx, [edx + edx - 1]
0048816e  mov        esi, edx
00488170  test       esi, esi
00488172  jne        0x487c36

// block 00488178
00488178  movzx      esi, byte ptr [eax - 0x13]
0048817c  movzx      edx, byte ptr [ecx - 0x13]
00488180  sub        esi, edx
00488182  je         0x488199

// block 00488184
00488184  xor        edx, edx
00488186  test       esi, esi
00488188  setg       dl
0048818b  lea        edx, [edx + edx - 1]
0048818f  mov        esi, edx
00488191  test       esi, esi
00488193  jne        0x487c36

// block 00488199
00488199  movzx      esi, byte ptr [eax - 0x12]
0048819d  movzx      edx, byte ptr [ecx - 0x12]
004881a1  sub        esi, edx
004881a3  je         0x4881b6

// block 004881a5
004881a5  xor        edx, edx
004881a7  test       esi, esi
004881a9  setg       dl
004881ac  lea        edx, [edx + edx - 1]
004881b0  mov        esi, edx
004881b2  jmp        0x4881b6

// block 004881b4
004881b4  xor        esi, esi

// block 004881b6
004881b6  test       esi, esi
004881b8  jne        0x487c36

// block 004881be
004881be  mov        edx, dword ptr [eax - 0x11]
004881c1  cmp        edx, dword ptr [ecx - 0x11]
004881c4  je         0x488243

// block 004881c6
004881c6  movzx      esi, dl
004881c9  movzx      edx, byte ptr [ecx - 0x11]
004881cd  sub        esi, edx
004881cf  je         0x4881e6

// block 004881d1
004881d1  xor        edx, edx
004881d3  test       esi, esi
004881d5  setg       dl
004881d8  lea        edx, [edx + edx - 1]
004881dc  mov        esi, edx
004881de  test       esi, esi
004881e0  jne        0x487c36

// block 004881e6
004881e6  movzx      esi, byte ptr [eax - 0x10]
004881ea  movzx      edx, byte ptr [ecx - 0x10]
004881ee  sub        esi, edx
004881f0  je         0x488207

// block 004881f2
004881f2  xor        edx, edx
004881f4  test       esi, esi
004881f6  setg       dl
004881f9  lea        edx, [edx + edx - 1]
004881fd  mov        esi, edx
004881ff  test       esi, esi
00488201  jne        0x487c36

// block 00488207
00488207  movzx      esi, byte ptr [eax - 0xf]
0048820b  movzx      edx, byte ptr [ecx - 0xf]
0048820f  sub        esi, edx
00488211  je         0x488228

// block 00488213
00488213  xor        edx, edx
00488215  test       esi, esi
00488217  setg       dl
0048821a  lea        edx, [edx + edx - 1]
0048821e  mov        esi, edx
00488220  test       esi, esi
00488222  jne        0x487c36

// block 00488228
00488228  movzx      esi, byte ptr [eax - 0xe]
0048822c  movzx      edx, byte ptr [ecx - 0xe]
00488230  sub        esi, edx
00488232  je         0x488245

// block 00488234
00488234  xor        edx, edx
00488236  test       esi, esi
00488238  setg       dl
0048823b  lea        edx, [edx + edx - 1]
0048823f  mov        esi, edx
00488241  jmp        0x488245

// block 00488243
00488243  xor        esi, esi

// block 00488245
00488245  test       esi, esi
00488247  jne        0x487c36

// block 0048824d
0048824d  mov        edx, dword ptr [eax - 0xd]
00488250  cmp        edx, dword ptr [ecx - 0xd]
00488253  je         0x4882d2

// block 00488255
00488255  movzx      esi, dl
00488258  movzx      edx, byte ptr [ecx - 0xd]
0048825c  sub        esi, edx
0048825e  je         0x488275

// block 00488260
00488260  xor        edx, edx
00488262  test       esi, esi
00488264  setg       dl
00488267  lea        edx, [edx + edx - 1]
0048826b  mov        esi, edx
0048826d  test       esi, esi
0048826f  jne        0x487c36

// block 00488275
00488275  movzx      esi, byte ptr [eax - 0xc]
00488279  movzx      edx, byte ptr [ecx - 0xc]
0048827d  sub        esi, edx
0048827f  je         0x488296

// block 00488281
00488281  xor        edx, edx
00488283  test       esi, esi
00488285  setg       dl
00488288  lea        edx, [edx + edx - 1]
0048828c  mov        esi, edx
0048828e  test       esi, esi
00488290  jne        0x487c36

// block 00488296
00488296  movzx      esi, byte ptr [eax - 0xb]
0048829a  movzx      edx, byte ptr [ecx - 0xb]
0048829e  sub        esi, edx
004882a0  je         0x4882b7

// block 004882a2
004882a2  xor        edx, edx
004882a4  test       esi, esi
004882a6  setg       dl
004882a9  lea        edx, [edx + edx - 1]
004882ad  mov        esi, edx
004882af  test       esi, esi
004882b1  jne        0x487c36

// block 004882b7
004882b7  movzx      esi, byte ptr [eax - 0xa]
004882bb  movzx      edx, byte ptr [ecx - 0xa]
004882bf  sub        esi, edx
004882c1  je         0x4882d4

// block 004882c3
004882c3  xor        edx, edx
004882c5  test       esi, esi
004882c7  setg       dl
004882ca  lea        edx, [edx + edx - 1]
004882ce  mov        esi, edx
004882d0  jmp        0x4882d4

// block 004882d2
004882d2  xor        esi, esi

// block 004882d4
004882d4  test       esi, esi
004882d6  jne        0x487c36

// block 004882dc
004882dc  mov        edx, dword ptr [eax - 9]
004882df  cmp        edx, dword ptr [ecx - 9]
004882e2  je         0x488362

// block 004882e4
004882e4  movzx      edx, byte ptr [ecx - 9]
004882e8  movzx      esi, byte ptr [eax - 9]
004882ec  sub        esi, edx
004882ee  je         0x488305

// block 004882f0
004882f0  xor        edx, edx
004882f2  test       esi, esi
004882f4  setg       dl
004882f7  lea        edx, [edx + edx - 1]
004882fb  mov        esi, edx
004882fd  test       esi, esi
004882ff  jne        0x487c36

// block 00488305
00488305  movzx      esi, byte ptr [eax - 8]
00488309  movzx      edx, byte ptr [ecx - 8]
0048830d  sub        esi, edx
0048830f  je         0x488326

// block 00488311
00488311  xor        edx, edx
00488313  test       esi, esi
00488315  setg       dl
00488318  lea        edx, [edx + edx - 1]
0048831c  mov        esi, edx
0048831e  test       esi, esi
00488320  jne        0x487c36

// block 00488326
00488326  movzx      esi, byte ptr [eax - 7]
0048832a  movzx      edx, byte ptr [ecx - 7]
0048832e  sub        esi, edx
00488330  je         0x488347

// block 00488332
00488332  xor        edx, edx
00488334  test       esi, esi
00488336  setg       dl
00488339  lea        edx, [edx + edx - 1]
0048833d  mov        esi, edx
0048833f  test       esi, esi
00488341  jne        0x487c36

// block 00488347
00488347  movzx      esi, byte ptr [eax - 6]
0048834b  movzx      edx, byte ptr [ecx - 6]
0048834f  sub        esi, edx
00488351  je         0x488364

// block 00488353
00488353  xor        edx, edx
00488355  test       esi, esi
00488357  setg       dl
0048835a  lea        edx, [edx + edx - 1]
0048835e  mov        esi, edx
00488360  jmp        0x488364

// block 00488362
00488362  xor        esi, esi

// block 00488364
00488364  test       esi, esi
00488366  jne        0x487c36

// block 0048836c
0048836c  mov        edx, dword ptr [eax - 5]
0048836f  cmp        edx, dword ptr [ecx - 5]
00488372  je         0x4883f1

// block 00488374
00488374  movzx      esi, dl
00488377  movzx      edx, byte ptr [ecx - 5]
0048837b  sub        esi, edx
0048837d  je         0x488394

// block 0048837f
0048837f  xor        edx, edx
00488381  test       esi, esi
00488383  setg       dl
00488386  lea        edx, [edx + edx - 1]
0048838a  mov        esi, edx
0048838c  test       esi, esi
0048838e  jne        0x487c36

// block 00488394
00488394  movzx      esi, byte ptr [eax - 4]
00488398  movzx      edx, byte ptr [ecx - 4]
0048839c  sub        esi, edx
0048839e  je         0x4883b5

// block 004883a0
004883a0  xor        edx, edx
004883a2  test       esi, esi
004883a4  setg       dl
004883a7  lea        edx, [edx + edx - 1]
004883ab  mov        esi, edx
004883ad  test       esi, esi
004883af  jne        0x487c36

// block 004883b5
004883b5  movzx      esi, byte ptr [eax - 3]
004883b9  movzx      edx, byte ptr [ecx - 3]
004883bd  sub        esi, edx
004883bf  je         0x4883d6

// block 004883c1
004883c1  xor        edx, edx
004883c3  test       esi, esi
004883c5  setg       dl
004883c8  lea        edx, [edx + edx - 1]
004883cc  mov        esi, edx
004883ce  test       esi, esi
004883d0  jne        0x487c36

// block 004883d6
004883d6  movzx      esi, byte ptr [eax - 2]
004883da  movzx      edx, byte ptr [ecx - 2]
004883de  sub        esi, edx
004883e0  je         0x4883f3

// block 004883e2
004883e2  xor        edx, edx
004883e4  test       esi, esi
004883e6  setg       dl
004883e9  lea        edx, [edx + edx - 1]
004883ed  mov        esi, edx
004883ef  jmp        0x4883f3

// block 004883f1
004883f1  xor        esi, esi

// block 004883f3
004883f3  test       esi, esi
004883f5  jne        0x487c36

// block 004883fb
004883fb  movzx      ecx, byte ptr [ecx - 1]
004883ff  movzx      eax, byte ptr [eax - 1]
00488403  sub        eax, ecx
00488405  je         0x48800b

// block 0048840b
0048840b  xor        ecx, ecx
0048840d  test       eax, eax
0048840f  setg       cl
00488412  lea        ecx, [ecx + ecx - 1]
00488416  mov        eax, ecx
00488418  jmp        0x48800b

// block 0048841d
0048841d  mov        edx, dword ptr [eax - 0x1e]
00488420  cmp        edx, dword ptr [ecx - 0x1e]
00488423  je         0x4884a2

// block 00488425
00488425  movzx      esi, dl
00488428  movzx      edx, byte ptr [ecx - 0x1e]
0048842c  sub        esi, edx
0048842e  je         0x488445

// block 00488430
00488430  xor        edx, edx
00488432  test       esi, esi
00488434  setg       dl
00488437  lea        edx, [edx + edx - 1]
0048843b  mov        esi, edx
0048843d  test       esi, esi
0048843f  jne        0x487c36

// block 00488445
00488445  movzx      esi, byte ptr [eax - 0x1d]
00488449  movzx      edx, byte ptr [ecx - 0x1d]
0048844d  sub        esi, edx
0048844f  je         0x488466

// block 00488451
00488451  xor        edx, edx
00488453  test       esi, esi
00488455  setg       dl
00488458  lea        edx, [edx + edx - 1]
0048845c  mov        esi, edx
0048845e  test       esi, esi
00488460  jne        0x487c36

// block 00488466
00488466  movzx      esi, byte ptr [eax - 0x1c]
0048846a  movzx      edx, byte ptr [ecx - 0x1c]
0048846e  sub        esi, edx
00488470  je         0x488487

// block 00488472
00488472  xor        edx, edx
00488474  test       esi, esi
00488476  setg       dl
00488479  lea        edx, [edx + edx - 1]
0048847d  mov        esi, edx
0048847f  test       esi, esi
00488481  jne        0x487c36

// block 00488487
00488487  movzx      esi, byte ptr [eax - 0x1b]
0048848b  movzx      edx, byte ptr [ecx - 0x1b]
0048848f  sub        esi, edx
00488491  je         0x4884a4

// block 00488493
00488493  xor        edx, edx
00488495  test       esi, esi
00488497  setg       dl
0048849a  lea        edx, [edx + edx - 1]
0048849e  mov        esi, edx
004884a0  jmp        0x4884a4

// block 004884a2
004884a2  xor        esi, esi

// block 004884a4
004884a4  test       esi, esi
004884a6  jne        0x487c36

// block 004884ac
004884ac  mov        edx, dword ptr [eax - 0x1a]
004884af  cmp        edx, dword ptr [ecx - 0x1a]
004884b2  je         0x488531

// block 004884b4
004884b4  movzx      esi, dl
004884b7  movzx      edx, byte ptr [ecx - 0x1a]
004884bb  sub        esi, edx
004884bd  je         0x4884d4

// block 004884bf
004884bf  xor        edx, edx
004884c1  test       esi, esi
004884c3  setg       dl
004884c6  lea        edx, [edx + edx - 1]
004884ca  mov        esi, edx
004884cc  test       esi, esi
004884ce  jne        0x487c36

// block 004884d4
004884d4  movzx      esi, byte ptr [eax - 0x19]
004884d8  movzx      edx, byte ptr [ecx - 0x19]
004884dc  sub        esi, edx
004884de  je         0x4884f5

// block 004884e0
004884e0  xor        edx, edx
004884e2  test       esi, esi
004884e4  setg       dl
004884e7  lea        edx, [edx + edx - 1]
004884eb  mov        esi, edx
004884ed  test       esi, esi
004884ef  jne        0x487c36

// block 004884f5
004884f5  movzx      esi, byte ptr [eax - 0x18]
004884f9  movzx      edx, byte ptr [ecx - 0x18]
004884fd  sub        esi, edx
004884ff  je         0x488516

// block 00488501
00488501  xor        edx, edx
00488503  test       esi, esi
00488505  setg       dl
00488508  lea        edx, [edx + edx - 1]
0048850c  mov        esi, edx
0048850e  test       esi, esi
00488510  jne        0x487c36

// block 00488516
00488516  movzx      esi, byte ptr [eax - 0x17]
0048851a  movzx      edx, byte ptr [ecx - 0x17]
0048851e  sub        esi, edx
00488520  je         0x488533

// block 00488522
00488522  xor        edx, edx
00488524  test       esi, esi
00488526  setg       dl
00488529  lea        edx, [edx + edx - 1]
0048852d  mov        esi, edx
0048852f  jmp        0x488533

// block 00488531
00488531  xor        esi, esi

// block 00488533
00488533  test       esi, esi
00488535  jne        0x487c36

// block 0048853b
0048853b  mov        edx, dword ptr [eax - 0x16]
0048853e  cmp        edx, dword ptr [ecx - 0x16]
00488541  je         0x4885c0

// block 00488543
00488543  movzx      esi, dl
00488546  movzx      edx, byte ptr [ecx - 0x16]
0048854a  sub        esi, edx
0048854c  je         0x488563

// block 0048854e
0048854e  xor        edx, edx
00488550  test       esi, esi
00488552  setg       dl
00488555  lea        edx, [edx + edx - 1]
00488559  mov        esi, edx
0048855b  test       esi, esi
0048855d  jne        0x487c36

// block 00488563
00488563  movzx      esi, byte ptr [eax - 0x15]
00488567  movzx      edx, byte ptr [ecx - 0x15]
0048856b  sub        esi, edx
0048856d  je         0x488584

// block 0048856f
0048856f  xor        edx, edx
00488571  test       esi, esi
00488573  setg       dl
00488576  lea        edx, [edx + edx - 1]
0048857a  mov        esi, edx
0048857c  test       esi, esi
0048857e  jne        0x487c36

// block 00488584
00488584  movzx      esi, byte ptr [eax - 0x14]
00488588  movzx      edx, byte ptr [ecx - 0x14]
0048858c  sub        esi, edx
0048858e  je         0x4885a5

// block 00488590
00488590  xor        edx, edx
00488592  test       esi, esi
00488594  setg       dl
00488597  lea        edx, [edx + edx - 1]
0048859b  mov        esi, edx
0048859d  test       esi, esi
0048859f  jne        0x487c36

// block 004885a5
004885a5  movzx      esi, byte ptr [eax - 0x13]
004885a9  movzx      edx, byte ptr [ecx - 0x13]
004885ad  sub        esi, edx
004885af  je         0x4885c2

// block 004885b1
004885b1  xor        edx, edx
004885b3  test       esi, esi
004885b5  setg       dl
004885b8  lea        edx, [edx + edx - 1]
004885bc  mov        esi, edx
004885be  jmp        0x4885c2

// block 004885c0
004885c0  xor        esi, esi

// block 004885c2
004885c2  test       esi, esi
004885c4  jne        0x487c36

// block 004885ca
004885ca  mov        edx, dword ptr [eax - 0x12]
004885cd  cmp        edx, dword ptr [ecx - 0x12]
004885d0  je         0x48864f

// block 004885d2
004885d2  movzx      esi, dl
004885d5  movzx      edx, byte ptr [ecx - 0x12]
004885d9  sub        esi, edx
004885db  je         0x4885f2

// block 004885dd
004885dd  xor        edx, edx
004885df  test       esi, esi
004885e1  setg       dl
004885e4  lea        edx, [edx + edx - 1]
004885e8  mov        esi, edx
004885ea  test       esi, esi
004885ec  jne        0x487c36

// block 004885f2
004885f2  movzx      esi, byte ptr [eax - 0x11]
004885f6  movzx      edx, byte ptr [ecx - 0x11]
004885fa  sub        esi, edx
004885fc  je         0x488613

// block 004885fe
004885fe  xor        edx, edx
00488600  test       esi, esi
00488602  setg       dl
00488605  lea        edx, [edx + edx - 1]
00488609  mov        esi, edx
0048860b  test       esi, esi
0048860d  jne        0x487c36

// block 00488613
00488613  movzx      esi, byte ptr [eax - 0x10]
00488617  movzx      edx, byte ptr [ecx - 0x10]
0048861b  sub        esi, edx
0048861d  je         0x488634

// block 0048861f
0048861f  xor        edx, edx
00488621  test       esi, esi
00488623  setg       dl
00488626  lea        edx, [edx + edx - 1]
0048862a  mov        esi, edx
0048862c  test       esi, esi
0048862e  jne        0x487c36

// block 00488634
00488634  movzx      esi, byte ptr [eax - 0xf]
00488638  movzx      edx, byte ptr [ecx - 0xf]
0048863c  sub        esi, edx
0048863e  je         0x488651

// block 00488640
00488640  xor        edx, edx
00488642  test       esi, esi
00488644  setg       dl
00488647  lea        edx, [edx + edx - 1]
0048864b  mov        esi, edx
0048864d  jmp        0x488651

// block 0048864f
0048864f  xor        esi, esi

// block 00488651
00488651  test       esi, esi
00488653  jne        0x487c36

// block 00488659
00488659  mov        edx, dword ptr [eax - 0xe]
0048865c  cmp        edx, dword ptr [ecx - 0xe]
0048865f  je         0x4886de

// block 00488661
00488661  movzx      esi, dl
00488664  movzx      edx, byte ptr [ecx - 0xe]
00488668  sub        esi, edx
0048866a  je         0x488681

// block 0048866c
0048866c  xor        edx, edx
0048866e  test       esi, esi
00488670  setg       dl
00488673  lea        edx, [edx + edx - 1]
00488677  mov        esi, edx
00488679  test       esi, esi
0048867b  jne        0x487c36

// block 00488681
00488681  movzx      esi, byte ptr [eax - 0xd]
00488685  movzx      edx, byte ptr [ecx - 0xd]
00488689  sub        esi, edx
0048868b  je         0x4886a2

// block 0048868d
0048868d  xor        edx, edx
0048868f  test       esi, esi
00488691  setg       dl
00488694  lea        edx, [edx + edx - 1]
00488698  mov        esi, edx
0048869a  test       esi, esi
0048869c  jne        0x487c36

// block 004886a2
004886a2  movzx      esi, byte ptr [eax - 0xc]
004886a6  movzx      edx, byte ptr [ecx - 0xc]
004886aa  sub        esi, edx
004886ac  je         0x4886c3

// block 004886ae
004886ae  xor        edx, edx
004886b0  test       esi, esi
004886b2  setg       dl
004886b5  lea        edx, [edx + edx - 1]
004886b9  mov        esi, edx
004886bb  test       esi, esi
004886bd  jne        0x487c36

// block 004886c3
004886c3  movzx      esi, byte ptr [eax - 0xb]
004886c7  movzx      edx, byte ptr [ecx - 0xb]
004886cb  sub        esi, edx
004886cd  je         0x4886e0

// block 004886cf
004886cf  xor        edx, edx
004886d1  test       esi, esi
004886d3  setg       dl
004886d6  lea        edx, [edx + edx - 1]
004886da  mov        esi, edx
004886dc  jmp        0x4886e0

// block 004886de
004886de  xor        esi, esi

// block 004886e0
004886e0  test       esi, esi
004886e2  jne        0x487c36

// block 004886e8
004886e8  mov        edx, dword ptr [eax - 0xa]
004886eb  cmp        edx, dword ptr [ecx - 0xa]
004886ee  je         0x48876e

// block 004886f0
004886f0  movzx      edx, byte ptr [ecx - 0xa]
004886f4  movzx      esi, byte ptr [eax - 0xa]
004886f8  sub        esi, edx
004886fa  je         0x488711

// block 004886fc
004886fc  xor        edx, edx
004886fe  test       esi, esi
00488700  setg       dl
00488703  lea        edx, [edx + edx - 1]
00488707  mov        esi, edx
00488709  test       esi, esi
0048870b  jne        0x487c36

// block 00488711
00488711  movzx      edx, byte ptr [ecx - 9]
00488715  movzx      esi, byte ptr [eax - 9]
00488719  sub        esi, edx
0048871b  je         0x488732

// block 0048871d
0048871d  xor        edx, edx
0048871f  test       esi, esi
00488721  setg       dl
00488724  lea        edx, [edx + edx - 1]
00488728  mov        esi, edx
0048872a  test       esi, esi
0048872c  jne        0x487c36

// block 00488732
00488732  movzx      edx, byte ptr [ecx - 8]
00488736  movzx      esi, byte ptr [eax - 8]
0048873a  sub        esi, edx
0048873c  je         0x488753

// block 0048873e
0048873e  xor        edx, edx
00488740  test       esi, esi
00488742  setg       dl
00488745  lea        edx, [edx + edx - 1]
00488749  mov        esi, edx
0048874b  test       esi, esi
0048874d  jne        0x487c36

// block 00488753
00488753  movzx      edx, byte ptr [ecx - 7]
00488757  movzx      esi, byte ptr [eax - 7]
0048875b  sub        esi, edx
0048875d  je         0x488770

// block 0048875f
0048875f  xor        edx, edx
00488761  test       esi, esi
00488763  setg       dl
00488766  lea        edx, [edx + edx - 1]
0048876a  mov        esi, edx
0048876c  jmp        0x488770

// block 0048876e
0048876e  xor        esi, esi

// block 00488770
00488770  test       esi, esi
00488772  jne        0x487c36

// block 00488778
00488778  mov        edx, dword ptr [eax - 6]
0048877b  cmp        edx, dword ptr [ecx - 6]
0048877e  je         0x4887fd

// block 00488780
00488780  movzx      esi, dl
00488783  movzx      edx, byte ptr [ecx - 6]
00488787  sub        esi, edx
00488789  je         0x4887a0

// block 0048878b
0048878b  xor        edx, edx
0048878d  test       esi, esi
0048878f  setg       dl
00488792  lea        edx, [edx + edx - 1]
00488796  mov        esi, edx
00488798  test       esi, esi
0048879a  jne        0x487c36

// block 004887a0
004887a0  movzx      esi, byte ptr [eax - 5]
004887a4  movzx      edx, byte ptr [ecx - 5]
004887a8  sub        esi, edx
004887aa  je         0x4887c1

// block 004887ac
004887ac  xor        edx, edx
004887ae  test       esi, esi
004887b0  setg       dl
004887b3  lea        edx, [edx + edx - 1]
004887b7  mov        esi, edx
004887b9  test       esi, esi
004887bb  jne        0x487c36

// block 004887c1
004887c1  movzx      esi, byte ptr [eax - 4]
004887c5  movzx      edx, byte ptr [ecx - 4]
004887c9  sub        esi, edx
004887cb  je         0x4887e2

// block 004887cd
004887cd  xor        edx, edx
004887cf  test       esi, esi
004887d1  setg       dl
004887d4  lea        edx, [edx + edx - 1]
004887d8  mov        esi, edx
004887da  test       esi, esi
004887dc  jne        0x487c36

// block 004887e2
004887e2  movzx      esi, byte ptr [eax - 3]
004887e6  movzx      edx, byte ptr [ecx - 3]
004887ea  sub        esi, edx
004887ec  je         0x4887ff

// block 004887ee
004887ee  xor        edx, edx
004887f0  test       esi, esi
004887f2  setg       dl
004887f5  lea        edx, [edx + edx - 1]
004887f9  mov        esi, edx
004887fb  jmp        0x4887ff

// block 004887fd
004887fd  xor        esi, esi

// block 004887ff
004887ff  test       esi, esi
00488801  jne        0x487c36

// block 00488807
00488807  mov        dx, word ptr [eax - 2]
0048880b  cmp        dx, word ptr [ecx - 2]
0048880f  je         0x488009

// block 00488815
00488815  movzx      edx, byte ptr [ecx - 2]
00488819  movzx      esi, byte ptr [eax - 2]
0048881d  sub        esi, edx
0048881f  je         0x4883fb

// block 00488825
00488825  xor        edx, edx
00488827  test       esi, esi
00488829  setg       dl
0048882c  lea        edx, [edx + edx - 1]
00488830  test       edx, edx
00488832  jne        0x488c4b

// block 00488838
00488838  jmp        0x4883fb

// block 0048883d
0048883d  mov        edx, dword ptr [eax - 0x1f]
00488840  cmp        edx, dword ptr [ecx - 0x1f]
00488843  je         0x4888c3

// block 00488845
00488845  movzx      edx, byte ptr [ecx - 0x1f]
00488849  movzx      esi, byte ptr [eax - 0x1f]
0048884d  sub        esi, edx
0048884f  je         0x488866

// block 00488851
00488851  xor        edx, edx
00488853  test       esi, esi
00488855  setg       dl
00488858  lea        edx, [edx + edx - 1]
0048885c  mov        esi, edx
0048885e  test       esi, esi
00488860  jne        0x487c36

// block 00488866
00488866  movzx      esi, byte ptr [eax - 0x1e]
0048886a  movzx      edx, byte ptr [ecx - 0x1e]
0048886e  sub        esi, edx
00488870  je         0x488887

// block 00488872
00488872  xor        edx, edx
00488874  test       esi, esi
00488876  setg       dl
00488879  lea        edx, [edx + edx - 1]
0048887d  mov        esi, edx
0048887f  test       esi, esi
00488881  jne        0x487c36

// block 00488887
00488887  movzx      esi, byte ptr [eax - 0x1d]
0048888b  movzx      edx, byte ptr [ecx - 0x1d]
0048888f  sub        esi, edx
00488891  je         0x4888a8

// block 00488893
00488893  xor        edx, edx
00488895  test       esi, esi
00488897  setg       dl
0048889a  lea        edx, [edx + edx - 1]
0048889e  mov        esi, edx
004888a0  test       esi, esi
004888a2  jne        0x487c36

// block 004888a8
004888a8  movzx      esi, byte ptr [eax - 0x1c]
004888ac  movzx      edx, byte ptr [ecx - 0x1c]
004888b0  sub        esi, edx
004888b2  je         0x4888c5

// block 004888b4
004888b4  xor        edx, edx
004888b6  test       esi, esi
004888b8  setg       dl
004888bb  lea        edx, [edx + edx - 1]
004888bf  mov        esi, edx
004888c1  jmp        0x4888c5

// block 004888c3
004888c3  xor        esi, esi

// block 004888c5
004888c5  test       esi, esi
004888c7  jne        0x487c36

// block 004888cd
004888cd  mov        edx, dword ptr [eax - 0x1b]
004888d0  cmp        edx, dword ptr [ecx - 0x1b]
004888d3  je         0x488952

// block 004888d5
004888d5  movzx      esi, dl
004888d8  movzx      edx, byte ptr [ecx - 0x1b]
004888dc  sub        esi, edx
004888de  je         0x4888f5

// block 004888e0
004888e0  xor        edx, edx
004888e2  test       esi, esi
004888e4  setg       dl
004888e7  lea        edx, [edx + edx - 1]
004888eb  mov        esi, edx
004888ed  test       esi, esi
004888ef  jne        0x487c36

// block 004888f5
004888f5  movzx      esi, byte ptr [eax - 0x1a]
004888f9  movzx      edx, byte ptr [ecx - 0x1a]
004888fd  sub        esi, edx
004888ff  je         0x488916

// block 00488901
00488901  xor        edx, edx
00488903  test       esi, esi
00488905  setg       dl
00488908  lea        edx, [edx + edx - 1]
0048890c  mov        esi, edx
0048890e  test       esi, esi
00488910  jne        0x487c36

// block 00488916
00488916  movzx      esi, byte ptr [eax - 0x19]
0048891a  movzx      edx, byte ptr [ecx - 0x19]
0048891e  sub        esi, edx
00488920  je         0x488937

// block 00488922
00488922  xor        edx, edx
00488924  test       esi, esi
00488926  setg       dl
00488929  lea        edx, [edx + edx - 1]
0048892d  mov        esi, edx
0048892f  test       esi, esi
00488931  jne        0x487c36

// block 00488937
00488937  movzx      esi, byte ptr [eax - 0x18]
0048893b  movzx      edx, byte ptr [ecx - 0x18]
0048893f  sub        esi, edx
00488941  je         0x488954

// block 00488943
00488943  xor        edx, edx
00488945  test       esi, esi
00488947  setg       dl
0048894a  lea        edx, [edx + edx - 1]
0048894e  mov        esi, edx
00488950  jmp        0x488954

// block 00488952
00488952  xor        esi, esi

// block 00488954
00488954  test       esi, esi
00488956  jne        0x487c36

// block 0048895c
0048895c  mov        edx, dword ptr [eax - 0x17]
0048895f  cmp        edx, dword ptr [ecx - 0x17]
00488962  je         0x4889e1

// block 00488964
00488964  movzx      esi, dl
00488967  movzx      edx, byte ptr [ecx - 0x17]
0048896b  sub        esi, edx
0048896d  je         0x488984

// block 0048896f
0048896f  xor        edx, edx
00488971  test       esi, esi
00488973  setg       dl
00488976  lea        edx, [edx + edx - 1]
0048897a  mov        esi, edx
0048897c  test       esi, esi
0048897e  jne        0x487c36

// block 00488984
00488984  movzx      esi, byte ptr [eax - 0x16]
00488988  movzx      edx, byte ptr [ecx - 0x16]
0048898c  sub        esi, edx
0048898e  je         0x4889a5

// block 00488990
00488990  xor        edx, edx
00488992  test       esi, esi
00488994  setg       dl
00488997  lea        edx, [edx + edx - 1]
0048899b  mov        esi, edx
0048899d  test       esi, esi
0048899f  jne        0x487c36

// block 004889a5
004889a5  movzx      esi, byte ptr [eax - 0x15]
004889a9  movzx      edx, byte ptr [ecx - 0x15]
004889ad  sub        esi, edx
004889af  je         0x4889c6

// block 004889b1
004889b1  xor        edx, edx
004889b3  test       esi, esi
004889b5  setg       dl
004889b8  lea        edx, [edx + edx - 1]
004889bc  mov        esi, edx
004889be  test       esi, esi
004889c0  jne        0x487c36

// block 004889c6
004889c6  movzx      esi, byte ptr [eax - 0x14]
004889ca  movzx      edx, byte ptr [ecx - 0x14]
004889ce  sub        esi, edx
004889d0  je         0x4889e3

// block 004889d2
004889d2  xor        edx, edx
004889d4  test       esi, esi
004889d6  setg       dl
004889d9  lea        edx, [edx + edx - 1]
004889dd  mov        esi, edx
004889df  jmp        0x4889e3

// block 004889e1
004889e1  xor        esi, esi

// block 004889e3
004889e3  test       esi, esi
004889e5  jne        0x487c36

// block 004889eb
004889eb  mov        edx, dword ptr [eax - 0x13]
004889ee  cmp        edx, dword ptr [ecx - 0x13]
004889f1  je         0x488a70

// block 004889f3
004889f3  movzx      esi, dl
004889f6  movzx      edx, byte ptr [ecx - 0x13]
004889fa  sub        esi, edx
004889fc  je         0x488a13

// block 004889fe
004889fe  xor        edx, edx
00488a00  test       esi, esi
00488a02  setg       dl
00488a05  lea        edx, [edx + edx - 1]
00488a09  mov        esi, edx
00488a0b  test       esi, esi
00488a0d  jne        0x487c36

// block 00488a13
00488a13  movzx      esi, byte ptr [eax - 0x12]
00488a17  movzx      edx, byte ptr [ecx - 0x12]
00488a1b  sub        esi, edx
00488a1d  je         0x488a34

// block 00488a1f
00488a1f  xor        edx, edx
00488a21  test       esi, esi
00488a23  setg       dl
00488a26  lea        edx, [edx + edx - 1]
00488a2a  mov        esi, edx
00488a2c  test       esi, esi
00488a2e  jne        0x487c36

// block 00488a34
00488a34  movzx      esi, byte ptr [eax - 0x11]
00488a38  movzx      edx, byte ptr [ecx - 0x11]
00488a3c  sub        esi, edx
00488a3e  je         0x488a55

// block 00488a40
00488a40  xor        edx, edx
00488a42  test       esi, esi
00488a44  setg       dl
00488a47  lea        edx, [edx + edx - 1]
00488a4b  mov        esi, edx
00488a4d  test       esi, esi
00488a4f  jne        0x487c36

// block 00488a55
00488a55  movzx      esi, byte ptr [eax - 0x10]
00488a59  movzx      edx, byte ptr [ecx - 0x10]
00488a5d  sub        esi, edx
00488a5f  je         0x488a72

// block 00488a61
00488a61  xor        edx, edx
00488a63  test       esi, esi
00488a65  setg       dl
00488a68  lea        edx, [edx + edx - 1]
00488a6c  mov        esi, edx
00488a6e  jmp        0x488a72

// block 00488a70
00488a70  xor        esi, esi

// block 00488a72
00488a72  test       esi, esi
00488a74  jne        0x487c36

// block 00488a7a
00488a7a  mov        edx, dword ptr [eax - 0xf]
00488a7d  cmp        edx, dword ptr [ecx - 0xf]
00488a80  je         0x488b00

// block 00488a82
00488a82  movzx      edx, byte ptr [ecx - 0xf]
00488a86  movzx      esi, byte ptr [eax - 0xf]
00488a8a  sub        esi, edx
00488a8c  je         0x488aa3

// block 00488a8e
00488a8e  xor        edx, edx
00488a90  test       esi, esi
00488a92  setg       dl
00488a95  lea        edx, [edx + edx - 1]
00488a99  mov        esi, edx
00488a9b  test       esi, esi
00488a9d  jne        0x487c36

// block 00488aa3
00488aa3  movzx      esi, byte ptr [eax - 0xe]
00488aa7  movzx      edx, byte ptr [ecx - 0xe]
00488aab  sub        esi, edx
00488aad  je         0x488ac4

// block 00488aaf
00488aaf  xor        edx, edx
00488ab1  test       esi, esi
00488ab3  setg       dl
00488ab6  lea        edx, [edx + edx - 1]
00488aba  mov        esi, edx
00488abc  test       esi, esi
00488abe  jne        0x487c36

// block 00488ac4
00488ac4  movzx      esi, byte ptr [eax - 0xd]
00488ac8  movzx      edx, byte ptr [ecx - 0xd]
00488acc  sub        esi, edx
00488ace  je         0x488ae5

// block 00488ad0
00488ad0  xor        edx, edx
00488ad2  test       esi, esi
00488ad4  setg       dl
00488ad7  lea        edx, [edx + edx - 1]
00488adb  mov        esi, edx
00488add  test       esi, esi
00488adf  jne        0x487c36

// block 00488ae5
00488ae5  movzx      esi, byte ptr [eax - 0xc]
00488ae9  movzx      edx, byte ptr [ecx - 0xc]
00488aed  sub        esi, edx
00488aef  je         0x488b02

// block 00488af1
00488af1  xor        edx, edx
00488af3  test       esi, esi
00488af5  setg       dl
00488af8  lea        edx, [edx + edx - 1]
00488afc  mov        esi, edx
00488afe  jmp        0x488b02

// block 00488b00
00488b00  xor        esi, esi

// block 00488b02
00488b02  test       esi, esi
00488b04  jne        0x487c36

// block 00488b0a
00488b0a  mov        edx, dword ptr [eax - 0xb]
00488b0d  cmp        edx, dword ptr [ecx - 0xb]
00488b10  je         0x488b8f

// block 00488b12
00488b12  movzx      esi, dl
00488b15  movzx      edx, byte ptr [ecx - 0xb]
00488b19  sub        esi, edx
00488b1b  je         0x488b32

// block 00488b1d
00488b1d  xor        edx, edx
00488b1f  test       esi, esi
00488b21  setg       dl
00488b24  lea        edx, [edx + edx - 1]
00488b28  mov        esi, edx
00488b2a  test       esi, esi
00488b2c  jne        0x487c36

// block 00488b32
00488b32  movzx      esi, byte ptr [eax - 0xa]
00488b36  movzx      edx, byte ptr [ecx - 0xa]
00488b3a  sub        esi, edx
00488b3c  je         0x488b53

// block 00488b3e
00488b3e  xor        edx, edx
00488b40  test       esi, esi
00488b42  setg       dl
00488b45  lea        edx, [edx + edx - 1]
00488b49  mov        esi, edx
00488b4b  test       esi, esi
00488b4d  jne        0x487c36

// block 00488b53
00488b53  movzx      esi, byte ptr [eax - 9]
00488b57  movzx      edx, byte ptr [ecx - 9]
00488b5b  sub        esi, edx
00488b5d  je         0x488b74

// block 00488b5f
00488b5f  xor        edx, edx
00488b61  test       esi, esi
00488b63  setg       dl
00488b66  lea        edx, [edx + edx - 1]
00488b6a  mov        esi, edx
00488b6c  test       esi, esi
00488b6e  jne        0x487c36

// block 00488b74
00488b74  movzx      esi, byte ptr [eax - 8]
00488b78  movzx      edx, byte ptr [ecx - 8]
00488b7c  sub        esi, edx
00488b7e  je         0x488b91

// block 00488b80
00488b80  xor        edx, edx
00488b82  test       esi, esi
00488b84  setg       dl
00488b87  lea        edx, [edx + edx - 1]
00488b8b  mov        esi, edx
00488b8d  jmp        0x488b91

// block 00488b8f
00488b8f  xor        esi, esi

// block 00488b91
00488b91  test       esi, esi
00488b93  jne        0x487c36

// block 00488b99
00488b99  mov        edx, dword ptr [eax - 7]
00488b9c  cmp        edx, dword ptr [ecx - 7]
00488b9f  je         0x488c1e

// block 00488ba1
00488ba1  movzx      esi, dl
00488ba4  movzx      edx, byte ptr [ecx - 7]
00488ba8  sub        esi, edx
00488baa  je         0x488bc1

// block 00488bac
00488bac  xor        edx, edx
00488bae  test       esi, esi
00488bb0  setg       dl
00488bb3  lea        edx, [edx + edx - 1]
00488bb7  mov        esi, edx
00488bb9  test       esi, esi
00488bbb  jne        0x487c36

// block 00488bc1
00488bc1  movzx      esi, byte ptr [eax - 6]
00488bc5  movzx      edx, byte ptr [ecx - 6]
00488bc9  sub        esi, edx
00488bcb  je         0x488be2

// block 00488bcd
00488bcd  xor        edx, edx
00488bcf  test       esi, esi
00488bd1  setg       dl
00488bd4  lea        edx, [edx + edx - 1]
00488bd8  mov        esi, edx
00488bda  test       esi, esi
00488bdc  jne        0x487c36

// block 00488be2
00488be2  movzx      esi, byte ptr [eax - 5]
00488be6  movzx      edx, byte ptr [ecx - 5]
00488bea  sub        esi, edx
00488bec  je         0x488c03

// block 00488bee
00488bee  xor        edx, edx
00488bf0  test       esi, esi
00488bf2  setg       dl
00488bf5  lea        edx, [edx + edx - 1]
00488bf9  mov        esi, edx
00488bfb  test       esi, esi
00488bfd  jne        0x487c36

// block 00488c03
00488c03  movzx      esi, byte ptr [eax - 4]
00488c07  movzx      edx, byte ptr [ecx - 4]
00488c0b  sub        esi, edx
00488c0d  je         0x488c20

// block 00488c0f
00488c0f  xor        edx, edx
00488c11  test       esi, esi
00488c13  setg       dl
00488c16  lea        edx, [edx + edx - 1]
00488c1a  mov        esi, edx
00488c1c  jmp        0x488c20

// block 00488c1e
00488c1e  xor        esi, esi

// block 00488c20
00488c20  test       esi, esi
00488c22  jne        0x487c36

// block 00488c28
00488c28  movzx      esi, byte ptr [eax - 3]
00488c2c  movzx      edx, byte ptr [ecx - 3]
00488c30  sub        esi, edx
00488c32  je         0x488815

// block 00488c38
00488c38  xor        edx, edx
00488c3a  test       esi, esi
00488c3c  setg       dl
00488c3f  lea        edx, [edx + edx - 1]
00488c43  test       edx, edx
00488c45  je         0x488815

// block 00488c4b
00488c4b  mov        eax, edx
00488c4d  jmp        0x48800b

// block 00488c52
00488c52  mov        ecx, dword ptr [ebp + 8]
00488c55  mov        esi, dword ptr [ebp + 0xc]
00488c58  movzx      eax, byte ptr [ecx]
00488c5b  movzx      edx, byte ptr [esi]
00488c5e  sub        eax, edx
00488c60  je         0x488c77

// block 00488c62
00488c62  xor        edx, edx
00488c64  test       eax, eax
00488c66  setg       dl
00488c69  lea        edx, [edx + edx - 1]
00488c6d  mov        eax, edx
00488c6f  test       eax, eax
00488c71  jne        0x488d64

// block 00488c77
00488c77  movzx      eax, byte ptr [ecx + 1]
00488c7b  movzx      edx, byte ptr [esi + 1]
00488c7f  sub        eax, edx
00488c81  je         0x488c98

// block 00488c83
00488c83  xor        edx, edx
00488c85  test       eax, eax
00488c87  setg       dl
00488c8a  lea        edx, [edx + edx - 1]
00488c8e  mov        eax, edx
00488c90  test       eax, eax
00488c92  jne        0x488d64

// block 00488c98
00488c98  movzx      eax, byte ptr [ecx + 2]
00488c9c  movzx      edx, byte ptr [esi + 2]
00488ca0  sub        eax, edx
00488ca2  je         0x488cb9

// block 00488ca4
00488ca4  xor        edx, edx
00488ca6  test       eax, eax
00488ca8  setg       dl
00488cab  lea        edx, [edx + edx - 1]
00488caf  mov        eax, edx
00488cb1  test       eax, eax
00488cb3  jne        0x488d64

// block 00488cb9
00488cb9  movzx      eax, byte ptr [ecx + 3]
00488cbd  movzx      ecx, byte ptr [esi + 3]

// block 00488cc1
00488cc1  sub        eax, ecx
00488cc3  je         0x488d64

// block 00488cc9
00488cc9  xor        ecx, ecx
00488ccb  test       eax, eax
00488ccd  setg       cl
00488cd0  lea        ecx, [ecx + ecx - 1]
00488cd4  mov        eax, ecx
00488cd6  jmp        0x488d64

// block 00488cdb
00488cdb  mov        ecx, dword ptr [ebp + 8]
00488cde  mov        esi, dword ptr [ebp + 0xc]
00488ce1  movzx      eax, byte ptr [ecx]
00488ce4  movzx      edx, byte ptr [esi]
00488ce7  sub        eax, edx
00488ce9  je         0x488cfc

// block 00488ceb
00488ceb  xor        edx, edx
00488ced  test       eax, eax
00488cef  setg       dl
00488cf2  lea        edx, [edx + edx - 1]
00488cf6  mov        eax, edx
00488cf8  test       eax, eax
00488cfa  jne        0x488d64

// block 00488cfc
00488cfc  movzx      eax, byte ptr [ecx + 1]
00488d00  movzx      edx, byte ptr [esi + 1]
00488d04  sub        eax, edx
00488d06  je         0x488d19

// block 00488d08
00488d08  xor        edx, edx
00488d0a  test       eax, eax
00488d0c  setg       dl
00488d0f  lea        edx, [edx + edx - 1]
00488d13  mov        eax, edx
00488d15  test       eax, eax
00488d17  jne        0x488d64

// block 00488d19
00488d19  movzx      eax, byte ptr [ecx + 2]
00488d1d  movzx      ecx, byte ptr [esi + 2]
00488d21  jmp        0x488cc1

// block 00488d23
00488d23  mov        ecx, dword ptr [ebp + 8]
00488d26  mov        esi, dword ptr [ebp + 0xc]
00488d29  movzx      eax, byte ptr [ecx]
00488d2c  movzx      edx, byte ptr [esi]
00488d2f  sub        eax, edx
00488d31  je         0x488d44

// block 00488d33
00488d33  xor        edx, edx
00488d35  test       eax, eax
00488d37  setg       dl
00488d3a  lea        edx, [edx + edx - 1]
00488d3e  mov        eax, edx
00488d40  test       eax, eax
00488d42  jne        0x488d64

// block 00488d44
00488d44  movzx      eax, byte ptr [ecx + 1]
00488d48  movzx      ecx, byte ptr [esi + 1]
00488d4c  jmp        0x488cc1

// block 00488d51
00488d51  mov        eax, dword ptr [ebp + 8]
00488d54  mov        ecx, dword ptr [ebp + 0xc]
00488d57  movzx      eax, byte ptr [eax]
00488d5a  movzx      ecx, byte ptr [ecx]
00488d5d  jmp        0x488cc1

// block 00488d62
00488d62  xor        eax, eax

// block 00488d64
00488d64  pop        edi
00488d65  pop        esi
00488d66  pop        ebp
00488d67  ret        
