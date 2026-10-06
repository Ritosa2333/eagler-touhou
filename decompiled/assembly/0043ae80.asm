
// block 0043ae80
0043ae80  push       ebx
0043ae81  push       ebp
0043ae82  mov        ebp, dword ptr [esp + 0xc]
0043ae86  push       esi
0043ae87  xor        esi, esi
0043ae89  push       edi
0043ae8a  mov        dword ptr [ebp + 0x10], eax
0043ae8d  cmp        eax, esi
0043ae8f  jne        0x43b1c1

// block 0043ae95
0043ae95  mov        eax, dword ptr [0x4b0cb0]
0043ae9a  mov        ecx, ebp
0043ae9c  mov        dword ptr [0x4b4518], ebp
0043aea2  call       0x43ccd0

// block 0043aea7
0043aea7  mov        edi, dword ptr [0x4b0cb0]
0043aead  mov        ebx, ebp
0043aeaf  call       0x43cbb0

// block 0043aeb4
0043aeb4  push       0x24
0043aeb6  mov        dword ptr [ebp + 0xa0], eax
0043aebc  call       0x46c9ea

// block 0043aec1
0043aec1  add        esp, 4
0043aec4  cmp        eax, esi
0043aec6  je         0x43aef7

// block 0043aec8
0043aec8  xor        ecx, ecx
0043aeca  mov        dword ptr [eax + 4], ecx
0043aecd  mov        dword ptr [eax + 8], ecx
0043aed0  mov        dword ptr [eax + 0xc], ecx
0043aed3  mov        dword ptr [eax + 0x14], ecx
0043aed6  mov        dword ptr [eax + 0x18], ecx
0043aed9  mov        dword ptr [eax + 0x1c], ecx
0043aedc  mov        dword ptr [eax + 0x20], ecx
0043aedf  mov        ecx, 4
0043aee4  mov        dword ptr [eax], 0x72323174
0043aeea  mov        word ptr [eax + 4], cx
0043aeee  mov        dword ptr [eax + 0x10], 0x100
0043aef5  jmp        0x43aef9

// block 0043aef7
0043aef7  xor        eax, eax

// block 0043aef9
0043aef9  push       0x70
0043aefb  mov        dword ptr [ebp + 0x18], eax
0043aefe  call       0x46c9ea

// block 0043af03
0043af03  mov        edi, eax
0043af05  add        esp, 4
0043af08  cmp        edi, esi
0043af0a  je         0x43af25

// block 0043af0c
0043af0c  lea        esi, [edi + 0x18]
0043af0f  call       0x423250

// block 0043af14
0043af14  push       0x70
0043af16  push       0
0043af18  push       edi
0043af19  call       0x477420

// block 0043af1e
0043af1e  add        esp, 0xc
0043af21  xor        esi, esi
0043af23  jmp        0x43af27

// block 0043af25
0043af25  xor        edi, edi

// block 0043af27
0043af27  push       0xa0
0043af2c  mov        dword ptr [ebp + 0x1c], edi
0043af2f  call       0x46c9ea

// block 0043af34
0043af34  mov        edi, eax
0043af36  add        esp, 4
0043af39  cmp        edi, esi
0043af3b  je         0x43af4e

// block 0043af3d
0043af3d  push       0xa0
0043af42  push       esi
0043af43  push       edi
0043af44  call       0x477420

// block 0043af49
0043af49  add        esp, 0xc
0043af4c  jmp        0x43af50

// block 0043af4e
0043af4e  xor        edi, edi

// block 0043af50
0043af50  mov        edx, dword ptr [0x4b0cb0]
0043af56  mov        dword ptr [ebp + edx*4 + 0x20], edi
0043af5a  mov        eax, dword ptr [0x4b0cb0]
0043af5f  mov        ecx, dword ptr [ebp + 0x1c]
0043af62  mov        edx, dword ptr [0x4b0c90]
0043af68  mov        eax, dword ptr [ebp + eax*4 + 0x20]
0043af6c  mov        dword ptr [ecx + 0x5c], edx
0043af6f  mov        ecx, dword ptr [ebp + 0x1c]
0043af72  mov        edx, dword ptr [0x4b0c94]
0043af78  mov        dword ptr [ecx + 0x60], edx
0043af7b  mov        ecx, dword ptr [ebp + 0x1c]
0043af7e  mov        edx, dword ptr [0x4b0ca8]
0043af84  mov        dword ptr [ecx + 0x64], edx
0043af87  mov        edx, dword ptr [0x4b0ce0]
0043af8d  mov        ecx, dword ptr [ebp + 0x1c]
0043af90  shr        edx, 4
0043af93  xor        dl, byte ptr [ecx + 0xa]
0043af96  and        dl, 1
0043af99  xor        byte ptr [ecx + 0xa], dl
0043af9c  mov        ecx, dword ptr [0x4b44e8]
0043afa2  cmp        ecx, esi
0043afa4  je         0x43afb8

// block 0043afa6
0043afa6  mov        edi, dword ptr [ebp + 0x1c]
0043afa9  lea        esi, [ecx + 0x24]
0043afac  add        edi, 0x18
0043afaf  mov        ecx, 0xf

// block 0043afb4
0043afb4  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043afb6
0043afb6  xor        esi, esi

// block 0043afb8
0043afb8  mov        cx, word ptr [0x4b0cb0]
0043afbf  mov        word ptr [eax], cx
0043afc2  mov        dx, word ptr [0x4ce568]
0043afc9  mov        word ptr [eax + 2], dx
0043afcd  mov        dword ptr [0x4ce56c], esi
0043afd3  mov        ecx, dword ptr [eax + 0x9c]
0043afd9  xor        ecx, dword ptr [0x4cee4c]
0043afdf  and        ecx, 1
0043afe2  xor        dword ptr [eax + 0x9c], ecx
0043afe8  cmp        dword ptr [0x4cee4c], esi
0043afee  je         0x43aff6

// block 0043aff0
0043aff0  mov        dword ptr [eax + 0x30], esi
0043aff3  mov        dword ptr [eax + 0x34], esi

// block 0043aff6
0043aff6  mov        edx, dword ptr [0x4b0c44]
0043affc  mov        dword ptr [eax + 0xc], edx
0043afff  movzx      ecx, word ptr [0x4b0c48]
0043b006  mov        word ptr [eax + 0x10], cx
0043b00a  mov        edx, dword ptr [0x4b0c78]
0043b010  mov        dword ptr [eax + 0x14], edx
0043b013  movzx      ecx, word ptr [0x4b0c98]
0043b01a  mov        word ptr [eax + 0x18], cx
0043b01e  movzx      edx, word ptr [0x4b0c9c]
0043b025  mov        word ptr [eax + 0x1a], dx
0043b029  movzx      ecx, word ptr [0x4b0ca0]
0043b030  mov        word ptr [eax + 0x1c], cx
0043b034  movzx      edx, word ptr [0x4b0ca4]
0043b03b  mov        word ptr [eax + 0x1e], dx
0043b03f  mov        ecx, dword ptr [0x4b0c4c]
0043b045  mov        dword ptr [eax + 0x20], ecx
0043b048  mov        edx, dword ptr [0x4b0c50]
0043b04e  mov        dword ptr [eax + 0x24], edx
0043b051  mov        ecx, dword ptr [0x4b0c54]
0043b057  mov        dword ptr [eax + 0x28], ecx
0043b05a  mov        edx, dword ptr [0x4b0ccc]
0043b060  mov        dword ptr [eax + 0x2c], edx
0043b063  mov        ecx, dword ptr [0x4b0cc4]
0043b069  mov        dword ptr [eax + 0x38], ecx
0043b06c  mov        edx, dword ptr [0x4b0cdc]
0043b072  mov        dword ptr [eax + 0x44], edx
0043b075  xor        ecx, ecx
0043b077  add        eax, 0x4c
0043b07a  mov        edx, 0x14
0043b07f  nop        

// block 0043b080
0043b080  mov        dword ptr [eax], ecx
0043b082  add        eax, 4
0043b085  sub        ecx, 0x21522153
0043b08b  sub        edx, 1
0043b08e  jne        0x43b080

// block 0043b090
0043b090  mov        eax, dword ptr [ebp + 0x1c]
0043b093  mov        ecx, dword ptr [0x4b0cc4]
0043b099  push       0x24
0043b09b  mov        dword ptr [eax + 0x6c], ecx
0043b09e  call       0x46c9ea

// block 0043b0a3
0043b0a3  add        esp, 4
0043b0a6  cmp        eax, esi
0043b0a8  je         0x43b0c6

// block 0043b0aa
0043b0aa  and        dword ptr [eax + 4], 0xfffffffe
0043b0ae  mov        dword ptr [eax + 8], esi
0043b0b1  mov        dword ptr [eax + 0xc], esi
0043b0b4  mov        dword ptr [eax + 0x10], esi
0043b0b7  mov        dword ptr [eax], esi
0043b0b9  mov        dword ptr [eax + 0x14], eax
0043b0bc  mov        dword ptr [eax + 0x18], esi
0043b0bf  mov        dword ptr [eax + 0x1c], esi
0043b0c2  mov        edi, eax
0043b0c4  jmp        0x43b0c8

// block 0043b0c6
0043b0c6  xor        edi, edi

// block 0043b0c8
0043b0c8  mov        edx, dword ptr [edi + 4]
0043b0cb  and        edx, 0xfffffffd
0043b0ce  mov        dword ptr [edi + 0xc], esi
0043b0d1  mov        dword ptr [edi + 0x10], esi
0043b0d4  or         edx, 1
0043b0d7  mov        ebx, 0xb
0043b0dc  mov        esi, edi
0043b0de  mov        dword ptr [edi + 8], 0x43c510
0043b0e5  mov        dword ptr [edi + 0x20], ebp
0043b0e8  mov        dword ptr [edi + 4], edx
0043b0eb  call       0x462380

// block 0043b0f0
0043b0f0  push       0x24
0043b0f2  mov        dword ptr [ebp + 8], edi
0043b0f5  call       0x46c9ea

// block 0043b0fa
0043b0fa  xor        edi, edi
0043b0fc  add        esp, 4
0043b0ff  cmp        eax, edi
0043b101  je         0x43b11f

// block 0043b103
0043b103  and        dword ptr [eax + 4], 0xfffffffe
0043b107  mov        dword ptr [eax + 8], edi
0043b10a  mov        dword ptr [eax + 0xc], edi
0043b10d  mov        dword ptr [eax + 0x10], edi
0043b110  mov        dword ptr [eax], edi
0043b112  mov        dword ptr [eax + 0x14], eax
0043b115  mov        dword ptr [eax + 0x18], edi
0043b118  mov        dword ptr [eax + 0x1c], edi
0043b11b  mov        esi, eax
0043b11d  jmp        0x43b121

// block 0043b11f
0043b11f  xor        esi, esi

// block 0043b121
0043b121  mov        eax, dword ptr [esi + 4]
0043b124  and        eax, 0xfffffffd
0043b127  or         eax, 1
0043b12a  mov        ebx, 0x1c
0043b12f  mov        dword ptr [esi + 8], 0x43c530
0043b136  mov        dword ptr [esi + 0xc], edi
0043b139  mov        dword ptr [esi + 0x10], edi
0043b13c  mov        dword ptr [esi + 0x20], ebp
0043b13f  mov        dword ptr [esi + 4], eax
0043b142  call       0x462380

// block 0043b147
0043b147  push       0x24
0043b149  mov        dword ptr [ebp + 0x1d4], esi
0043b14f  call       0x46c9ea

// block 0043b154
0043b154  add        esp, 4
0043b157  cmp        eax, edi
0043b159  je         0x43b177

// block 0043b15b
0043b15b  and        dword ptr [eax + 4], 0xfffffffe
0043b15f  mov        dword ptr [eax + 8], edi
0043b162  mov        dword ptr [eax + 0xc], edi
0043b165  mov        dword ptr [eax + 0x10], edi
0043b168  mov        dword ptr [eax], edi
0043b16a  mov        dword ptr [eax + 0x14], eax
0043b16d  mov        dword ptr [eax + 0x18], edi
0043b170  mov        dword ptr [eax + 0x1c], edi
0043b173  mov        esi, eax
0043b175  jmp        0x43b179

// block 0043b177
0043b177  xor        esi, esi

// block 0043b179
0043b179  mov        ecx, dword ptr [esi + 4]
0043b17c  and        ecx, 0xfffffffd
0043b17f  or         ecx, 1
0043b182  mov        ebx, 0x3a
0043b187  mov        dword ptr [esi + 8], 0x43c570
0043b18e  mov        dword ptr [esi + 0xc], edi
0043b191  mov        dword ptr [esi + 0x10], edi
0043b194  mov        dword ptr [esi + 0x20], ebp
0043b197  mov        dword ptr [esi + 4], ecx
0043b19a  call       0x462420

// block 0043b19f
0043b19f  mov        dword ptr [ebp + 0xc], esi
0043b1a2  mov        edx, dword ptr [0x4b0cb0]
0043b1a8  mov        dword ptr [ebp + 0x1d8], edx
0043b1ae  mov        dword ptr [ebp + 0x1d0], 0xffffffff
0043b1b8  xor        eax, eax
0043b1ba  pop        edi
0043b1bb  pop        esi
0043b1bc  pop        ebp
0043b1bd  pop        ebx
0043b1be  ret        4

// block 0043b1c1
0043b1c1  cmp        eax, 1
0043b1c4  jne        0x43b432

// block 0043b1ca
0043b1ca  push       ecx
0043b1cb  push       ebp
0043b1cc  mov        dword ptr [0x4b4518], ebp
0043b1d2  call       0x43c350

// block 0043b1d7
0043b1d7  test       eax, eax
0043b1d9  je         0x43b1e5

// block 0043b1db
0043b1db  or         eax, 0xffffffff
0043b1de  pop        edi
0043b1df  pop        esi
0043b1e0  pop        ebp
0043b1e1  pop        ebx
0043b1e2  ret        4

// block 0043b1e5
0043b1e5  mov        esi, dword ptr [ebp + 0x1c]
0043b1e8  mov        edi, dword ptr [0x4b44e8]
0043b1ee  add        esi, 0x18
0043b1f1  add        edi, 0x24
0043b1f4  mov        ecx, 0xf

// block 0043b1f9
0043b1f9  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043b1fb
0043b1fb  mov        eax, dword ptr [0x4b0cb0]
0043b200  lea        eax, [eax + eax*8]
0043b203  mov        ecx, dword ptr [ebp + eax*4 + 0xa8]
0043b20a  mov        edx, dword ptr [ebp + eax*4 + 0xb0]
0043b211  mov        ebx, dword ptr [ebp + eax*4 + 0xb8]
0043b218  mov        dword ptr [ebp + eax*4 + 0xac], ecx
0043b21f  mov        dword ptr [ebp + eax*4 + 0xb4], edx
0043b226  mov        dword ptr [ebp + eax*4 + 0xbc], 0xffffffff
0043b231  lea        eax, [ebp + eax*4 + 0xa8]
0043b238  mov        eax, dword ptr [ebp + 0x1c]
0043b23b  mov        ecx, dword ptr [eax + 0x5c]
0043b23e  mov        dword ptr [0x4b0c90], ecx
0043b244  mov        edx, dword ptr [ebp + 0x1c]
0043b247  mov        eax, dword ptr [edx + 0x60]
0043b24a  mov        dword ptr [0x4b0c94], eax
0043b24f  mov        ecx, dword ptr [ebp + 0x1c]
0043b252  mov        edx, dword ptr [ecx + 0x64]
0043b255  mov        dword ptr [0x4b0ca8], edx
0043b25b  mov        ax, word ptr [ebx + 2]
0043b25f  mov        word ptr [0x4ce568], ax
0043b265  mov        dword ptr [0x4ce56c], 0
0043b26f  mov        ecx, dword ptr [ebx + 0xc]
0043b272  mov        dword ptr [0x4b0c44], ecx
0043b278  movsx      eax, word ptr [ebx + 0x10]
0043b27c  mov        ecx, dword ptr [0x4b0cd0]
0043b282  cmp        eax, ecx
0043b284  mov        dword ptr [0x4b0c48], eax
0043b289  jg         0x43b295

// block 0043b28b
0043b28b  mov        ecx, dword ptr [0x4b0cd4]
0043b291  cmp        eax, ecx
0043b293  jge        0x43b29b

// block 0043b295
0043b295  mov        dword ptr [0x4b0c48], ecx

// block 0043b29b
0043b29b  mov        edx, dword ptr [ebx + 0x14]
0043b29e  mov        dword ptr [0x4b0c78], edx
0043b2a4  movsx      eax, word ptr [ebx + 0x18]
0043b2a8  mov        dword ptr [0x4b0c98], eax
0043b2ad  movsx      ecx, word ptr [ebx + 0x1a]
0043b2b1  mov        dword ptr [0x4b0c9c], ecx
0043b2b7  movsx      edx, word ptr [ebx + 0x1c]
0043b2bb  mov        dword ptr [0x4b0ca0], edx
0043b2c1  movsx      eax, word ptr [ebx + 0x1e]
0043b2c5  mov        dword ptr [0x4b0ca4], eax
0043b2ca  mov        edi, dword ptr [ebx + 0x20]
0043b2cd  mov        esi, 0x4b0c4c
0043b2d2  call       0x422e80

// block 0043b2d7
0043b2d7  mov        edi, dword ptr [ebx + 0x24]
0043b2da  call       0x422e80

// block 0043b2df
0043b2df  mov        edi, dword ptr [ebx + 0x28]
0043b2e2  call       0x422e80

// block 0043b2e7
0043b2e7  mov        ecx, dword ptr [ebx + 0x2c]
0043b2ea  mov        eax, dword ptr [0x4b0cb0]
0043b2ef  mov        dword ptr [0x4b0ccc], ecx
0043b2f5  mov        edx, dword ptr [ebx + 0x38]
0043b2f8  mov        dword ptr [0x4b0cc4], edx
0043b2fe  lea        eax, [eax + eax*8]
0043b301  mov        ecx, dword ptr [ebp + eax*4 + 0xb8]
0043b308  mov        edx, dword ptr [ecx + 0x3c]
0043b30b  mov        dword ptr [0x4b0cd8], edx
0043b311  mov        eax, dword ptr [ebx + 0x44]
0043b314  push       0x24
0043b316  mov        dword ptr [0x4b0cdc], eax
0043b31b  call       0x46c9ea

// block 0043b320
0043b320  xor        ecx, ecx
0043b322  add        esp, 4
0043b325  cmp        eax, ecx
0043b327  je         0x43b345

// block 0043b329
0043b329  and        dword ptr [eax + 4], 0xfffffffe
0043b32d  mov        dword ptr [eax + 8], ecx
0043b330  mov        dword ptr [eax + 0xc], ecx
0043b333  mov        dword ptr [eax + 0x10], ecx
0043b336  mov        dword ptr [eax], ecx
0043b338  mov        dword ptr [eax + 0x14], eax
0043b33b  mov        dword ptr [eax + 0x18], ecx
0043b33e  mov        dword ptr [eax + 0x1c], ecx
0043b341  mov        esi, eax
0043b343  jmp        0x43b347

// block 0043b345
0043b345  xor        esi, esi

// block 0043b347
0043b347  mov        dword ptr [esi + 0xc], ecx
0043b34a  mov        dword ptr [esi + 0x10], ecx
0043b34d  mov        ecx, dword ptr [esi + 4]
0043b350  and        ecx, 0xfffffffd
0043b353  or         ecx, 1
0043b356  mov        ebx, 0xb
0043b35b  mov        dword ptr [esi + 8], 0x43c520
0043b362  mov        dword ptr [esi + 0x20], ebp
0043b365  mov        dword ptr [esi + 4], ecx
0043b368  call       0x462380

// block 0043b36d
0043b36d  push       0x24
0043b36f  mov        dword ptr [ebp + 8], esi
0043b372  call       0x46c9ea

// block 0043b377
0043b377  xor        edi, edi
0043b379  add        esp, 4
0043b37c  cmp        eax, edi
0043b37e  je         0x43b39c

// block 0043b380
0043b380  and        dword ptr [eax + 4], 0xfffffffe
0043b384  mov        dword ptr [eax + 8], edi
0043b387  mov        dword ptr [eax + 0xc], edi
0043b38a  mov        dword ptr [eax + 0x10], edi
0043b38d  mov        dword ptr [eax], edi
0043b38f  mov        dword ptr [eax + 0x14], eax
0043b392  mov        dword ptr [eax + 0x18], edi
0043b395  mov        dword ptr [eax + 0x1c], edi
0043b398  mov        esi, eax
0043b39a  jmp        0x43b39e

// block 0043b39c
0043b39c  xor        esi, esi

// block 0043b39e
0043b39e  mov        edx, dword ptr [esi + 4]
0043b3a1  and        edx, 0xfffffffd
0043b3a4  or         edx, 1
0043b3a7  mov        ebx, 0x1c
0043b3ac  mov        dword ptr [esi + 8], 0x43c530
0043b3b3  mov        dword ptr [esi + 0xc], edi
0043b3b6  mov        dword ptr [esi + 0x10], edi
0043b3b9  mov        dword ptr [esi + 0x20], ebp
0043b3bc  mov        dword ptr [esi + 4], edx
0043b3bf  call       0x462380

// block 0043b3c4
0043b3c4  push       0x24
0043b3c6  mov        dword ptr [ebp + 0x1d4], esi
0043b3cc  call       0x46c9ea

// block 0043b3d1
0043b3d1  add        esp, 4
0043b3d4  cmp        eax, edi
0043b3d6  je         0x43b3f4

// block 0043b3d8
0043b3d8  and        dword ptr [eax + 4], 0xfffffffe
0043b3dc  mov        dword ptr [eax + 8], edi
0043b3df  mov        dword ptr [eax + 0xc], edi
0043b3e2  mov        dword ptr [eax + 0x10], edi
0043b3e5  mov        dword ptr [eax], edi
0043b3e7  mov        dword ptr [eax + 0x14], eax
0043b3ea  mov        dword ptr [eax + 0x18], edi
0043b3ed  mov        dword ptr [eax + 0x1c], edi
0043b3f0  mov        esi, eax
0043b3f2  jmp        0x43b3f6

// block 0043b3f4
0043b3f4  xor        esi, esi

// block 0043b3f6
0043b3f6  mov        eax, dword ptr [esi + 4]
0043b3f9  and        eax, 0xfffffffd
0043b3fc  or         eax, 1
0043b3ff  mov        ebx, 0x3a
0043b404  mov        dword ptr [esi + 8], 0x43c570
0043b40b  mov        dword ptr [esi + 0xc], edi
0043b40e  mov        dword ptr [esi + 0x10], edi
0043b411  mov        dword ptr [esi + 0x20], ebp
0043b414  mov        dword ptr [esi + 4], eax
0043b417  call       0x462420

// block 0043b41c
0043b41c  mov        dword ptr [ebp + 0xc], esi
0043b41f  mov        dword ptr [ebp + 0x1d8], 0xffffffff
0043b429  xor        eax, eax
0043b42b  pop        edi
0043b42c  pop        esi
0043b42d  pop        ebp
0043b42e  pop        ebx
0043b42f  ret        4

// block 0043b432
0043b432  cmp        eax, 2
0043b435  jne        0x43b446

// block 0043b437
0043b437  push       ecx
0043b438  push       ebp
0043b439  call       0x43c350

// block 0043b43e
0043b43e  test       eax, eax
0043b440  jne        0x43b1db

// block 0043b446
0043b446  pop        edi
0043b447  pop        esi
0043b448  pop        ebp
0043b449  xor        eax, eax
0043b44b  pop        ebx
0043b44c  ret        4
