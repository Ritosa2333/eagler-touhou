
// block 0045ee00
0045ee00  sub        esp, 0x44
0045ee03  mov        eax, dword ptr [eax]
0045ee05  push       ebx
0045ee06  push       ebp
0045ee07  push       esi
0045ee08  push       edi
0045ee09  lea        edx, [esp + 0x10]
0045ee0d  push       edx
0045ee0e  xor        ebx, ebx
0045ee10  mov        dword ptr [esp + 0x14], ebx
0045ee14  mov        ecx, dword ptr [eax]
0045ee16  push       ebx
0045ee17  push       eax
0045ee18  mov        eax, dword ptr [ecx + 0x48]
0045ee1b  call       eax

// block 0045ee1d
0045ee1d  mov        eax, dword ptr [esp + 0x10]
0045ee21  mov        ecx, dword ptr [eax]
0045ee23  lea        edx, [esp + 0x30]
0045ee27  push       edx
0045ee28  push       eax
0045ee29  mov        eax, dword ptr [ecx + 0x30]
0045ee2c  call       eax

// block 0045ee2e
0045ee2e  mov        eax, dword ptr [esp + 0x10]
0045ee32  mov        ecx, dword ptr [eax]
0045ee34  push       ebx
0045ee35  push       ebx
0045ee36  lea        edx, [esp + 0x24]
0045ee3a  push       edx
0045ee3b  push       eax
0045ee3c  mov        eax, dword ptr [ecx + 0x34]
0045ee3f  call       eax

// block 0045ee41
0045ee41  mov        eax, dword ptr [esp + 0x30]
0045ee45  cmp        eax, 0x1d
0045ee48  ja         0x45f4f5

// block 0045ee4e
0045ee4e  movzx      ecx, byte ptr [eax + 0x45f52c]
0045ee55  jmp        dword ptr [ecx*4 + 0x45f518]

// block 0045ee5c
0045ee5c  xor        ecx, ecx
0045ee5e  mov        dword ptr [esp + 0x14], ecx
0045ee62  cmp        dword ptr [esp + 0x4c], ebx
0045ee66  jbe        0x45f4f5

// block 0045ee6c
0045ee6c  mov        eax, dword ptr [esp + 0x48]

// block 0045ee70
0045ee70  mov        edi, dword ptr [esp + 0x1c]
0045ee74  imul       edi, ecx
0045ee77  add        edi, dword ptr [esp + 0x20]
0045ee7b  xor        ebp, ebp
0045ee7d  mov        dword ptr [esp + 0x18], ebp
0045ee81  cmp        eax, ebx
0045ee83  jbe        0x45efb4

// block 0045ee89
0045ee89  lea        ecx, [edi - 2]
0045ee8c  lea        esp, [esp]

// block 0045ee90
0045ee90  cmp        byte ptr [ecx + 5], 0
0045ee94  jne        0x45ef9d

// block 0045ee9a
0045ee9a  xor        esi, esi
0045ee9c  mov        dword ptr [esp + 0x2c], ebx
0045eea0  mov        dword ptr [esp + 0x28], ebx
0045eea4  test       ebp, ebp
0045eea6  jbe        0x45eec6

// block 0045eea8
0045eea8  cmp        byte ptr [ecx + 1], 0
0045eeac  je         0x45eec6

// block 0045eeae
0045eeae  movzx      edx, byte ptr [ecx - 1]
0045eeb2  movzx      ebx, byte ptr [ecx]
0045eeb5  mov        dword ptr [esp + 0x28], edx
0045eeb9  movzx      edx, byte ptr [ecx - 2]
0045eebd  mov        dword ptr [esp + 0x2c], edx
0045eec1  mov        esi, 1

// block 0045eec6
0045eec6  dec        eax
0045eec7  cmp        ebp, eax
0045eec9  jae        0x45eee8

// block 0045eecb
0045eecb  cmp        byte ptr [ecx + 9], 0
0045eecf  je         0x45eee8

// block 0045eed1
0045eed1  movzx      eax, byte ptr [ecx + 8]
0045eed5  movzx      edx, byte ptr [ecx + 7]
0045eed9  add        dword ptr [esp + 0x28], edx
0045eedd  add        ebx, eax
0045eedf  movzx      eax, byte ptr [ecx + 6]
0045eee3  add        dword ptr [esp + 0x2c], eax
0045eee7  inc        esi

// block 0045eee8
0045eee8  cmp        dword ptr [esp + 0x14], 0
0045eeed  jbe        0x45ef26

// block 0045eeef
0045eeef  mov        eax, dword ptr [esp + 0x1c]
0045eef3  cdq        
0045eef4  and        edx, 3
0045eef7  add        eax, edx
0045eef9  sar        eax, 2
0045eefc  add        eax, eax
0045eefe  add        eax, eax
0045ef00  mov        edx, eax
0045ef02  mov        eax, edi
0045ef04  sub        eax, edx
0045ef06  cmp        byte ptr [eax + 3], 0
0045ef0a  je         0x45ef26

// block 0045ef0c
0045ef0c  movzx      edx, byte ptr [eax + 2]
0045ef10  mov        ebp, dword ptr [esp + 0x18]
0045ef14  add        ebx, edx
0045ef16  movzx      edx, byte ptr [eax + 1]
0045ef1a  movzx      eax, byte ptr [eax]
0045ef1d  add        dword ptr [esp + 0x28], edx
0045ef21  add        dword ptr [esp + 0x2c], eax
0045ef25  inc        esi

// block 0045ef26
0045ef26  mov        edx, dword ptr [esp + 0x4c]
0045ef2a  dec        edx
0045ef2b  cmp        dword ptr [esp + 0x14], edx
0045ef2f  jae        0x45ef62

// block 0045ef31
0045ef31  mov        eax, dword ptr [esp + 0x1c]
0045ef35  cdq        
0045ef36  and        edx, 3
0045ef39  add        eax, edx
0045ef3b  sar        eax, 2
0045ef3e  cmp        byte ptr [edi + eax*4 + 3], 0
0045ef43  lea        eax, [edi + eax*4]
0045ef46  je         0x45ef62

// block 0045ef48
0045ef48  movzx      edx, byte ptr [eax + 2]
0045ef4c  mov        ebp, dword ptr [esp + 0x18]
0045ef50  add        ebx, edx
0045ef52  movzx      edx, byte ptr [eax + 1]
0045ef56  movzx      eax, byte ptr [eax]
0045ef59  add        dword ptr [esp + 0x28], edx
0045ef5d  add        dword ptr [esp + 0x2c], eax
0045ef61  inc        esi

// block 0045ef62
0045ef62  cmp        esi, 1
0045ef65  jbe        0x45ef87

// block 0045ef67
0045ef67  xor        edx, edx
0045ef69  mov        eax, ebx
0045ef6b  div        esi
0045ef6d  xor        edx, edx
0045ef6f  mov        ebx, eax
0045ef71  mov        eax, dword ptr [esp + 0x28]
0045ef75  div        esi
0045ef77  xor        edx, edx
0045ef79  mov        dword ptr [esp + 0x28], eax
0045ef7d  mov        eax, dword ptr [esp + 0x2c]
0045ef81  div        esi
0045ef83  mov        dword ptr [esp + 0x2c], eax

// block 0045ef87
0045ef87  mov        al, byte ptr [esp + 0x2c]
0045ef8b  mov        dl, byte ptr [esp + 0x28]
0045ef8f  mov        byte ptr [ecx + 4], bl
0045ef92  mov        byte ptr [ecx + 3], dl
0045ef95  mov        byte ptr [edi], al
0045ef97  mov        eax, dword ptr [esp + 0x48]
0045ef9b  xor        ebx, ebx

// block 0045ef9d
0045ef9d  inc        ebp
0045ef9e  add        edi, 4
0045efa1  add        ecx, 4
0045efa4  mov        dword ptr [esp + 0x18], ebp
0045efa8  cmp        ebp, eax
0045efaa  jb         0x45ee90

// block 0045efb0
0045efb0  mov        ecx, dword ptr [esp + 0x14]

// block 0045efb4
0045efb4  inc        ecx
0045efb5  mov        dword ptr [esp + 0x14], ecx
0045efb9  cmp        ecx, dword ptr [esp + 0x4c]
0045efbd  jb         0x45ee70

// block 0045efc3
0045efc3  jmp        0x45f4f5

// block 0045efc8
0045efc8  xor        eax, eax
0045efca  mov        dword ptr [esp + 0x14], eax
0045efce  cmp        dword ptr [esp + 0x4c], ebx
0045efd2  jbe        0x45f4f5

// block 0045efd8
0045efd8  mov        edx, dword ptr [esp + 0x48]
0045efdc  lea        esp, [esp]

// block 0045efe0
0045efe0  mov        ebp, eax
0045efe2  imul       ebp, dword ptr [esp + 0x1c]
0045efe7  add        ebp, dword ptr [esp + 0x20]
0045efeb  xor        esi, esi
0045efed  mov        dword ptr [esp + 0x18], esi
0045eff1  cmp        edx, ebx
0045eff3  jbe        0x45f176

// block 0045eff9
0045eff9  lea        esp, [esp]

// block 0045f000
0045f000  mov        ecx, 0x8000
0045f005  test       word ptr [ebp], cx
0045f009  jne        0x45f162

// block 0045f00f
0045f00f  xor        ecx, ecx
0045f011  xor        edi, edi
0045f013  mov        dword ptr [esp + 0x2c], ebx
0045f017  test       esi, esi
0045f019  jbe        0x45f045

// block 0045f01b
0045f01b  movzx      eax, word ptr [ebp - 2]
0045f01f  test       eax, 0x8000
0045f024  je         0x45f045

// block 0045f026
0045f026  movzx      eax, ax
0045f029  mov        ecx, eax
0045f02b  mov        ebx, eax
0045f02d  shr        ecx, 0xa
0045f030  shr        ebx, 5
0045f033  and        ecx, 0x1f
0045f036  and        ebx, 0x1f
0045f039  and        eax, 0x1f
0045f03c  mov        dword ptr [esp + 0x2c], eax
0045f040  mov        edi, 1

// block 0045f045
0045f045  dec        edx
0045f046  cmp        esi, edx
0045f048  jae        0x45f074

// block 0045f04a
0045f04a  movzx      eax, word ptr [ebp + 2]
0045f04e  test       eax, 0x8000
0045f053  je         0x45f074

// block 0045f055
0045f055  movzx      esi, ax
0045f058  mov        edx, esi
0045f05a  mov        eax, esi
0045f05c  shr        edx, 0xa
0045f05f  shr        eax, 5
0045f062  and        edx, 0x1f
0045f065  and        eax, 0x1f
0045f068  and        esi, 0x1f
0045f06b  add        dword ptr [esp + 0x2c], esi
0045f06f  add        ecx, edx
0045f071  add        ebx, eax
0045f073  inc        edi

// block 0045f074
0045f074  cmp        dword ptr [esp + 0x14], 0
0045f079  jbe        0x45f0b5

// block 0045f07b
0045f07b  mov        eax, dword ptr [esp + 0x1c]
0045f07f  cdq        
0045f080  sub        eax, edx
0045f082  sar        eax, 1
0045f084  add        eax, eax
0045f086  mov        edx, eax
0045f088  mov        eax, ebp
0045f08a  sub        eax, edx
0045f08c  movzx      eax, word ptr [eax]
0045f08f  test       eax, 0x8000
0045f094  je         0x45f0b5

// block 0045f096
0045f096  movzx      eax, ax
0045f099  mov        edx, eax
0045f09b  shr        edx, 0xa
0045f09e  and        edx, 0x1f
0045f0a1  add        ecx, edx
0045f0a3  mov        edx, eax
0045f0a5  shr        edx, 5
0045f0a8  and        edx, 0x1f
0045f0ab  and        eax, 0x1f
0045f0ae  add        dword ptr [esp + 0x2c], eax
0045f0b2  add        ebx, edx
0045f0b4  inc        edi

// block 0045f0b5
0045f0b5  mov        eax, dword ptr [esp + 0x4c]
0045f0b9  dec        eax
0045f0ba  cmp        dword ptr [esp + 0x14], eax
0045f0be  jae        0x45f0f6

// block 0045f0c0
0045f0c0  mov        eax, dword ptr [esp + 0x1c]
0045f0c4  cdq        
0045f0c5  sub        eax, edx
0045f0c7  sar        eax, 1
0045f0c9  lea        eax, [ebp + eax*2]
0045f0cd  movzx      eax, word ptr [eax]
0045f0d0  test       eax, 0x8000
0045f0d5  je         0x45f0f6

// block 0045f0d7
0045f0d7  movzx      eax, ax
0045f0da  mov        edx, eax
0045f0dc  shr        edx, 0xa
0045f0df  and        edx, 0x1f
0045f0e2  add        ecx, edx
0045f0e4  mov        edx, eax
0045f0e6  shr        edx, 5
0045f0e9  and        edx, 0x1f
0045f0ec  and        eax, 0x1f
0045f0ef  add        dword ptr [esp + 0x2c], eax
0045f0f3  add        ebx, edx
0045f0f5  inc        edi

// block 0045f0f6
0045f0f6  cmp        edi, 1
0045f0f9  jbe        0x45f117

// block 0045f0fb
0045f0fb  xor        edx, edx
0045f0fd  mov        eax, ecx
0045f0ff  div        edi
0045f101  xor        edx, edx
0045f103  mov        ecx, eax
0045f105  mov        eax, ebx
0045f107  div        edi
0045f109  xor        edx, edx
0045f10b  mov        ebx, eax
0045f10d  mov        eax, dword ptr [esp + 0x2c]
0045f111  div        edi
0045f113  mov        dword ptr [esp + 0x2c], eax

// block 0045f117
0045f117  mov        ax, word ptr [ebp]
0045f11b  and        cl, 0x1f
0045f11e  mov        esi, dword ptr [esp + 0x18]
0045f122  movzx      cx, cl
0045f126  shl        cx, 5
0045f12a  and        bl, 0x1f
0045f12d  movzx      dx, bl
0045f131  or         cx, dx
0045f134  mov        edx, 0x801f
0045f139  and        ax, dx
0045f13c  shl        cx, 5
0045f140  or         cx, ax
0045f143  mov        eax, 0xffe0
0045f148  and        cx, ax
0045f14b  mov        al, byte ptr [esp + 0x2c]
0045f14f  and        al, 0x1f
0045f151  movzx      dx, al
0045f155  or         cx, dx
0045f158  mov        word ptr [ebp], cx
0045f15c  mov        edx, dword ptr [esp + 0x48]
0045f160  xor        ebx, ebx

// block 0045f162
0045f162  inc        esi
0045f163  add        ebp, 2
0045f166  mov        dword ptr [esp + 0x18], esi
0045f16a  cmp        esi, edx
0045f16c  jb         0x45f000

// block 0045f172
0045f172  mov        eax, dword ptr [esp + 0x14]

// block 0045f176
0045f176  inc        eax
0045f177  mov        dword ptr [esp + 0x14], eax
0045f17b  cmp        eax, dword ptr [esp + 0x4c]
0045f17f  jb         0x45efe0

// block 0045f185
0045f185  jmp        0x45f4f5

// block 0045f18a
0045f18a  xor        eax, eax
0045f18c  mov        dword ptr [esp + 0x14], eax
0045f190  cmp        dword ptr [esp + 0x4c], ebx
0045f194  jbe        0x45f4f5

// block 0045f19a
0045f19a  mov        edx, dword ptr [esp + 0x48]
0045f19e  mov        edi, edi

// block 0045f1a0
0045f1a0  mov        ebp, eax
0045f1a2  imul       ebp, dword ptr [esp + 0x1c]
0045f1a7  add        ebp, dword ptr [esp + 0x20]
0045f1ab  xor        esi, esi
0045f1ad  mov        dword ptr [esp + 0x18], esi
0045f1b1  cmp        edx, ebx
0045f1b3  jbe        0x45f336

// block 0045f1b9
0045f1b9  lea        esp, [esp]

// block 0045f1c0
0045f1c0  mov        eax, 0xf000
0045f1c5  test       word ptr [ebp], ax
0045f1c9  jne        0x45f322

// block 0045f1cf
0045f1cf  xor        ecx, ecx
0045f1d1  xor        edi, edi
0045f1d3  mov        dword ptr [esp + 0x2c], ebx
0045f1d7  test       esi, esi
0045f1d9  jbe        0x45f205

// block 0045f1db
0045f1db  movzx      eax, word ptr [ebp - 2]
0045f1df  test       eax, 0xf000
0045f1e4  je         0x45f205

// block 0045f1e6
0045f1e6  movzx      eax, ax
0045f1e9  mov        ecx, eax
0045f1eb  mov        ebx, eax
0045f1ed  shr        ecx, 8
0045f1f0  shr        ebx, 4
0045f1f3  and        ecx, 0xf
0045f1f6  and        ebx, 0xf
0045f1f9  and        eax, 0xf
0045f1fc  mov        dword ptr [esp + 0x2c], eax
0045f200  mov        edi, 1

// block 0045f205
0045f205  dec        edx
0045f206  cmp        esi, edx
0045f208  jae        0x45f234

// block 0045f20a
0045f20a  movzx      eax, word ptr [ebp + 2]
0045f20e  test       eax, 0xf000
0045f213  je         0x45f234

// block 0045f215
0045f215  movzx      esi, ax
0045f218  mov        edx, esi
0045f21a  mov        eax, esi
0045f21c  shr        edx, 8
0045f21f  shr        eax, 4
0045f222  and        edx, 0xf
0045f225  and        eax, 0xf
0045f228  and        esi, 0xf
0045f22b  add        dword ptr [esp + 0x2c], esi
0045f22f  add        ecx, edx
0045f231  add        ebx, eax
0045f233  inc        edi

// block 0045f234
0045f234  cmp        dword ptr [esp + 0x14], 0
0045f239  jbe        0x45f275

// block 0045f23b
0045f23b  mov        eax, dword ptr [esp + 0x1c]
0045f23f  cdq        
0045f240  sub        eax, edx
0045f242  sar        eax, 1
0045f244  add        eax, eax
0045f246  mov        edx, eax
0045f248  mov        eax, ebp
0045f24a  sub        eax, edx
0045f24c  movzx      eax, word ptr [eax]
0045f24f  test       eax, 0xf000
0045f254  je         0x45f275

// block 0045f256
0045f256  movzx      eax, ax
0045f259  mov        edx, eax
0045f25b  shr        edx, 8
0045f25e  and        edx, 0xf
0045f261  add        ecx, edx
0045f263  mov        edx, eax
0045f265  shr        edx, 4
0045f268  and        edx, 0xf
0045f26b  and        eax, 0xf
0045f26e  add        dword ptr [esp + 0x2c], eax
0045f272  add        ebx, edx
0045f274  inc        edi

// block 0045f275
0045f275  mov        eax, dword ptr [esp + 0x4c]
0045f279  dec        eax
0045f27a  cmp        dword ptr [esp + 0x14], eax
0045f27e  jae        0x45f2b6

// block 0045f280
0045f280  mov        eax, dword ptr [esp + 0x1c]
0045f284  cdq        
0045f285  sub        eax, edx
0045f287  sar        eax, 1
0045f289  lea        eax, [ebp + eax*2]
0045f28d  movzx      eax, word ptr [eax]
0045f290  test       eax, 0xf000
0045f295  je         0x45f2b6

// block 0045f297
0045f297  movzx      eax, ax
0045f29a  mov        edx, eax
0045f29c  shr        edx, 8
0045f29f  and        edx, 0xf
0045f2a2  add        ecx, edx
0045f2a4  mov        edx, eax
0045f2a6  shr        edx, 4
0045f2a9  and        edx, 0xf
0045f2ac  and        eax, 0xf
0045f2af  add        dword ptr [esp + 0x2c], eax
0045f2b3  add        ebx, edx
0045f2b5  inc        edi

// block 0045f2b6
0045f2b6  cmp        edi, 1
0045f2b9  jbe        0x45f2d7

// block 0045f2bb
0045f2bb  xor        edx, edx
0045f2bd  mov        eax, ecx
0045f2bf  div        edi
0045f2c1  xor        edx, edx
0045f2c3  mov        ecx, eax
0045f2c5  mov        eax, ebx
0045f2c7  div        edi
0045f2c9  xor        edx, edx
0045f2cb  mov        ebx, eax
0045f2cd  mov        eax, dword ptr [esp + 0x2c]
0045f2d1  div        edi
0045f2d3  mov        dword ptr [esp + 0x2c], eax

// block 0045f2d7
0045f2d7  mov        ax, word ptr [ebp]
0045f2db  and        cl, 0xf
0045f2de  mov        esi, dword ptr [esp + 0x18]
0045f2e2  movzx      cx, cl
0045f2e6  shl        cx, 4
0045f2ea  and        bl, 0xf
0045f2ed  movzx      dx, bl
0045f2f1  or         cx, dx
0045f2f4  mov        edx, 0xf00f
0045f2f9  and        ax, dx
0045f2fc  shl        cx, 4
0045f300  or         cx, ax
0045f303  mov        eax, 0xfff0
0045f308  and        cx, ax
0045f30b  mov        al, byte ptr [esp + 0x2c]
0045f30f  and        al, 0xf
0045f311  movzx      dx, al
0045f315  or         cx, dx
0045f318  mov        word ptr [ebp], cx
0045f31c  mov        edx, dword ptr [esp + 0x48]
0045f320  xor        ebx, ebx

// block 0045f322
0045f322  inc        esi
0045f323  add        ebp, 2
0045f326  mov        dword ptr [esp + 0x18], esi
0045f32a  cmp        esi, edx
0045f32c  jb         0x45f1c0

// block 0045f332
0045f332  mov        eax, dword ptr [esp + 0x14]

// block 0045f336
0045f336  inc        eax
0045f337  mov        dword ptr [esp + 0x14], eax
0045f33b  cmp        eax, dword ptr [esp + 0x4c]
0045f33f  jb         0x45f1a0

// block 0045f345
0045f345  jmp        0x45f4f5

// block 0045f34a
0045f34a  xor        eax, eax
0045f34c  mov        dword ptr [esp + 0x14], eax
0045f350  cmp        dword ptr [esp + 0x4c], ebx
0045f354  jbe        0x45f4f5

// block 0045f35a
0045f35a  mov        edx, dword ptr [esp + 0x48]
0045f35e  mov        edi, edi

// block 0045f360
0045f360  mov        esi, eax
0045f362  imul       esi, dword ptr [esp + 0x1c]
0045f367  add        esi, dword ptr [esp + 0x20]
0045f36b  xor        edi, edi
0045f36d  mov        dword ptr [esp + 0x18], edi
0045f371  cmp        edx, ebx
0045f373  jbe        0x45f4e6

// block 0045f379
0045f379  lea        esp, [esp]

// block 0045f380
0045f380  cmp        byte ptr [esi + 1], 0
0045f384  jne        0x45f4d2

// block 0045f38a
0045f38a  xor        ecx, ecx
0045f38c  xor        ebp, ebp
0045f38e  mov        dword ptr [esp + 0x2c], ebx
0045f392  test       edi, edi
0045f394  jbe        0x45f3bc

// block 0045f396
0045f396  cmp        byte ptr [esi - 1], 0
0045f39a  je         0x45f3bc

// block 0045f39c
0045f39c  movzx      eax, word ptr [esi - 2]
0045f3a0  mov        ecx, eax
0045f3a2  mov        ebx, eax
0045f3a4  shr        ecx, 5
0045f3a7  shr        ebx, 2
0045f3aa  and        ecx, 7
0045f3ad  and        ebx, 7
0045f3b0  and        eax, 3
0045f3b3  mov        dword ptr [esp + 0x2c], eax
0045f3b7  mov        ebp, 1

// block 0045f3bc
0045f3bc  dec        edx
0045f3bd  cmp        edi, edx
0045f3bf  jae        0x45f3eb

// block 0045f3c1
0045f3c1  cmp        byte ptr [esi + 3], 0
0045f3c5  je         0x45f3eb

// block 0045f3c7
0045f3c7  movzx      edi, word ptr [esi + 2]
0045f3cb  mov        eax, edi
0045f3cd  mov        edx, edi
0045f3cf  shr        eax, 5
0045f3d2  shr        edx, 2
0045f3d5  and        edi, 3
0045f3d8  add        dword ptr [esp + 0x2c], edi
0045f3dc  mov        edi, dword ptr [esp + 0x18]
0045f3e0  and        eax, 7
0045f3e3  and        edx, 7
0045f3e6  add        ecx, eax
0045f3e8  add        ebx, edx
0045f3ea  inc        ebp

// block 0045f3eb
0045f3eb  cmp        dword ptr [esp + 0x14], 0
0045f3f0  jbe        0x45f428

// block 0045f3f2
0045f3f2  mov        eax, dword ptr [esp + 0x1c]
0045f3f6  cdq        
0045f3f7  sub        eax, edx
0045f3f9  sar        eax, 1
0045f3fb  add        eax, eax
0045f3fd  mov        edx, eax
0045f3ff  mov        eax, esi
0045f401  sub        eax, edx
0045f403  cmp        byte ptr [eax + 1], 0
0045f407  je         0x45f428

// block 0045f409
0045f409  movzx      eax, word ptr [eax]
0045f40c  mov        edx, eax
0045f40e  shr        edx, 5
0045f411  and        edx, 7
0045f414  add        ecx, edx
0045f416  mov        edx, eax
0045f418  shr        edx, 2
0045f41b  and        edx, 7
0045f41e  and        eax, 3
0045f421  add        dword ptr [esp + 0x2c], eax
0045f425  add        ebx, edx
0045f427  inc        ebp

// block 0045f428
0045f428  mov        eax, dword ptr [esp + 0x4c]
0045f42c  dec        eax
0045f42d  cmp        dword ptr [esp + 0x14], eax
0045f431  jae        0x45f465

// block 0045f433
0045f433  mov        eax, dword ptr [esp + 0x1c]
0045f437  cdq        
0045f438  sub        eax, edx
0045f43a  sar        eax, 1
0045f43c  cmp        byte ptr [esi + eax*2 + 1], 0
0045f441  lea        eax, [esi + eax*2]
0045f444  je         0x45f465

// block 0045f446
0045f446  movzx      eax, word ptr [eax]
0045f449  mov        edx, eax
0045f44b  shr        edx, 5
0045f44e  and        edx, 7
0045f451  add        ecx, edx
0045f453  mov        edx, eax
0045f455  shr        edx, 2
0045f458  and        edx, 7
0045f45b  and        eax, 3
0045f45e  add        dword ptr [esp + 0x2c], eax
0045f462  add        ebx, edx
0045f464  inc        ebp

// block 0045f465
0045f465  cmp        ebp, 1
0045f468  jbe        0x45f486

// block 0045f46a
0045f46a  xor        edx, edx
0045f46c  mov        eax, ecx
0045f46e  div        ebp
0045f470  xor        edx, edx
0045f472  mov        ecx, eax
0045f474  mov        eax, ebx
0045f476  div        ebp
0045f478  xor        edx, edx
0045f47a  mov        ebx, eax
0045f47c  mov        eax, dword ptr [esp + 0x2c]
0045f480  div        ebp
0045f482  mov        dword ptr [esp + 0x2c], eax

// block 0045f486
0045f486  mov        ax, word ptr [esi]
0045f489  and        cl, 7
0045f48c  movzx      cx, cl
0045f490  add        cx, cx
0045f493  add        cx, cx
0045f496  add        cx, cx
0045f499  and        bl, 7
0045f49c  movzx      dx, bl
0045f4a0  or         cx, dx
0045f4a3  add        cx, cx
0045f4a6  mov        edx, 0xff03
0045f4ab  and        ax, dx
0045f4ae  add        cx, cx
0045f4b1  or         cx, ax
0045f4b4  mov        eax, 0xfffc
0045f4b9  and        cx, ax
0045f4bc  mov        al, byte ptr [esp + 0x2c]
0045f4c0  and        al, 3
0045f4c2  movzx      dx, al
0045f4c6  or         cx, dx
0045f4c9  mov        word ptr [esi], cx
0045f4cc  mov        edx, dword ptr [esp + 0x48]
0045f4d0  xor        ebx, ebx

// block 0045f4d2
0045f4d2  inc        edi
0045f4d3  add        esi, 2
0045f4d6  mov        dword ptr [esp + 0x18], edi
0045f4da  cmp        edi, edx
0045f4dc  jb         0x45f380

// block 0045f4e2
0045f4e2  mov        eax, dword ptr [esp + 0x14]

// block 0045f4e6
0045f4e6  inc        eax
0045f4e7  mov        dword ptr [esp + 0x14], eax
0045f4eb  cmp        eax, dword ptr [esp + 0x4c]
0045f4ef  jb         0x45f360

// block 0045f4f5
0045f4f5  mov        eax, dword ptr [esp + 0x10]
0045f4f9  mov        ecx, dword ptr [eax]
0045f4fb  mov        edx, dword ptr [ecx + 0x38]
0045f4fe  push       eax
0045f4ff  call       edx

// block 0045f501
0045f501  mov        eax, dword ptr [esp + 0x10]
0045f505  mov        ecx, dword ptr [eax]
0045f507  mov        edx, dword ptr [ecx + 8]
0045f50a  push       eax
0045f50b  call       edx

// block 0045f50d
0045f50d  pop        edi
0045f50e  pop        esi
0045f50f  pop        ebp
0045f510  pop        ebx
0045f511  add        esp, 0x44
0045f514  ret        
