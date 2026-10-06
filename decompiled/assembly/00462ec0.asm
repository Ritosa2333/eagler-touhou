
// block 00462ec0
00462ec0  sub        esp, 0x104
00462ec6  mov        eax, dword ptr [0x4ad138]
00462ecb  xor        eax, esp
00462ecd  mov        dword ptr [esp + 0x100], eax
00462ed4  push       esi
00462ed5  xor        esi, esi
00462ed7  cmp        dword ptr [0x4cf3fc], esi
00462edd  je         0x4631fb

// block 00462ee3
00462ee3  test       dword ptr [0x4cee78], 0x400
00462eed  jne        0x46305d

// block 00462ef3
00462ef3  lea        eax, [esp + 4]
00462ef7  push       eax
00462ef8  call       dword ptr [0x498244]

// block 00462efe
00462efe  movzx      esi, byte ptr [esp + 0x7d]
00462f03  movzx      ecx, byte ptr [esp + 0x56]
00462f08  movzx      edx, byte ptr [esp + 0x48]
00462f0d  movzx      eax, byte ptr [esp + 0x11]
00462f12  and        esi, 0x80
00462f18  and        ecx, 0x80
00462f1e  add        esi, esi
00462f20  add        esi, esi
00462f22  or         esi, ecx
00462f24  movzx      ecx, byte ptr [esp + 0x54]
00462f29  add        esi, esi
00462f2b  and        edx, 0x80
00462f31  or         esi, edx
00462f33  movzx      edx, byte ptr [esp + 0x28]
00462f38  or         ecx, edx
00462f3a  movzx      edx, byte ptr [esp + 0x15]
00462f3f  and        ecx, 0x80
00462f45  add        esi, esi
00462f47  and        eax, 0x80
00462f4c  or         esi, eax
00462f4e  movzx      eax, byte ptr [esp + 0x57]
00462f53  add        esi, esi
00462f55  or         esi, ecx
00462f57  movzx      ecx, byte ptr [esp + 0x55]
00462f5c  and        ecx, 0x80
00462f62  add        esi, esi
00462f64  and        eax, 0x80
00462f69  or         esi, eax
00462f6b  movzx      eax, byte ptr [esp + 0x1f]
00462f70  and        edx, 0x80
00462f76  add        esi, esi
00462f78  or         esi, ecx
00462f7a  movzx      ecx, byte ptr [esp + 0x5c]
00462f7f  and        ecx, 0x80
00462f85  shl        esi, 7
00462f88  or         esi, edx
00462f8a  movzx      edx, byte ptr [esp + 0x5e]
00462f8f  and        eax, 0x80
00462f94  shr        edx, 1
00462f96  or         ecx, edx
00462f98  movzx      edx, byte ptr [esp + 0x6c]
00462f9d  shr        ecx, 2
00462fa0  add        esi, esi
00462fa2  or         esi, eax
00462fa4  movzx      eax, byte ptr [esp + 0x14]
00462fa9  and        eax, 0x80
00462fae  or         ecx, eax
00462fb0  movzx      eax, byte ptr [esp + 0x2a]
00462fb5  or         edx, eax
00462fb7  movzx      eax, byte ptr [esp + 0x2c]
00462fbc  shr        ecx, 1
00462fbe  and        edx, 0x80
00462fc4  or         ecx, edx
00462fc6  movzx      edx, byte ptr [esp + 0x66]
00462fcb  or         edx, eax
00462fcd  movzx      eax, byte ptr [esp + 0x29]
00462fd2  shr        ecx, 1
00462fd4  and        edx, 0x80
00462fda  or         ecx, edx
00462fdc  movzx      edx, byte ptr [esp + 0x68]
00462fe1  shr        ecx, 1
00462fe3  or         edx, eax
00462fe5  movzx      eax, byte ptr [esp + 0x67]
00462fea  and        edx, 0x80
00462ff0  or         ecx, edx
00462ff2  movzx      edx, byte ptr [esp + 0x2b]
00462ff7  shr        ecx, 1
00462ff9  add        esi, esi
00462ffb  or         esi, ecx
00462ffd  movzx      ecx, byte ptr [esp + 0x6a]
00463002  or         ecx, edx
00463004  and        ecx, 0x80
0046300a  or         esi, ecx
0046300c  and        al, 0x80
0046300e  movzx      ecx, al
00463011  neg        ecx
00463013  sbb        ecx, ecx
00463015  and        ecx, 0xa0
0046301b  mov        dl, byte ptr [esp + 0x6d]
0046301f  and        dl, 0x80
00463022  or         esi, ecx
00463024  mov        cl, byte ptr [esp + 0x65]
00463028  movzx      eax, dl
0046302b  neg        eax
0046302d  sbb        eax, eax
0046302f  and        cl, 0x80
00463032  and        eax, 0x90
00463037  movzx      edx, cl
0046303a  or         esi, eax
0046303c  movzx      eax, byte ptr [esp + 0x6b]
00463041  neg        edx
00463043  sbb        edx, edx
00463045  and        al, 0x80
00463047  and        edx, 0x60
0046304a  movzx      ecx, al
0046304d  or         esi, edx
0046304f  neg        ecx
00463051  sbb        ecx, ecx
00463053  and        ecx, 0x50

// block 00463056
00463056  or         esi, ecx
00463058  jmp        0x4631fb

// block 0046305d
0046305d  mov        eax, dword ptr [0x4ce908]
00463062  mov        edx, dword ptr [eax]
00463064  mov        edx, dword ptr [edx + 0x24]
00463067  lea        ecx, [esp + 4]
0046306b  push       ecx
0046306c  push       0x100
00463071  push       eax
00463072  call       edx

// block 00463074
00463074  xor        esi, esi
00463076  cmp        eax, 0x8007001e
0046307b  je         0x463081

// block 0046307d
0046307d  test       eax, eax
0046307f  je         0x463093

// block 00463081
00463081  mov        eax, dword ptr [0x4ce908]
00463086  mov        ecx, dword ptr [eax]
00463088  mov        edx, dword ptr [ecx + 0x1c]
0046308b  push       eax
0046308c  call       edx

// block 0046308e
0046308e  jmp        0x4631fb

// block 00463093
00463093  movzx      esi, byte ptr [esp + 0x48]
00463098  movzx      eax, byte ptr [esp + 0x17]
0046309d  movzx      ecx, byte ptr [esp + 0x24]
004630a2  movzx      edx, byte ptr [esp + 0x20]
004630a7  and        esi, 0x80
004630ad  and        eax, 0x80
004630b2  add        esi, esi
004630b4  add        esi, esi
004630b6  or         esi, eax
004630b8  movzx      eax, byte ptr [esp + 0x1d]
004630bd  add        esi, esi
004630bf  and        ecx, 0x80
004630c5  or         esi, ecx
004630c7  movzx      ecx, byte ptr [esp + 0xcb]
004630cf  or         eax, ecx
004630d1  movzx      ecx, byte ptr [esp + 0x21]
004630d6  and        eax, 0x80
004630db  add        esi, esi
004630dd  and        edx, 0x80
004630e3  or         esi, edx
004630e5  movzx      edx, byte ptr [esp + 0x23]
004630ea  add        esi, esi
004630ec  or         esi, eax
004630ee  movzx      eax, byte ptr [esp + 0x14]
004630f3  and        eax, 0x80
004630f8  add        esi, esi
004630fa  and        edx, 0x80
00463100  or         esi, edx
00463102  movzx      edx, byte ptr [esp + 5]
00463107  and        ecx, 0x80
0046310d  add        esi, esi
0046310f  or         esi, eax
00463111  movzx      eax, byte ptr [esp + 0x31]
00463116  and        eax, 0x80
0046311b  shl        esi, 7
0046311e  or         esi, ecx
00463120  movzx      ecx, byte ptr [esp + 0x30]
00463125  shr        ecx, 1
00463127  or         eax, ecx
00463129  movzx      ecx, byte ptr [esp + 0x4c]
0046312e  and        edx, 0x80
00463134  shr        eax, 2
00463137  add        esi, esi
00463139  or         esi, edx
0046313b  movzx      edx, byte ptr [esp + 0x2e]
00463140  and        edx, 0x80
00463146  or         eax, edx
00463148  movzx      edx, byte ptr [esp + 0xcc]
00463150  or         ecx, edx
00463152  movzx      edx, byte ptr [esp + 0x54]
00463157  shr        eax, 1
00463159  and        ecx, 0x80
0046315f  or         eax, ecx
00463161  movzx      ecx, byte ptr [esp + 0xd4]
00463169  or         ecx, edx
0046316b  movzx      edx, byte ptr [esp + 0xcf]
00463173  shr        eax, 1
00463175  and        ecx, 0x80
0046317b  or         eax, ecx
0046317d  movzx      ecx, byte ptr [esp + 0x4f]
00463182  shr        eax, 1
00463184  or         ecx, edx
00463186  movzx      edx, byte ptr [esp + 0x4d]
0046318b  and        ecx, 0x80
00463191  or         eax, ecx
00463193  movzx      ecx, byte ptr [esp + 0xd1]
0046319b  shr        eax, 1
0046319d  add        esi, esi
0046319f  or         esi, eax
004631a1  movzx      eax, byte ptr [esp + 0x51]
004631a6  or         eax, ecx
004631a8  mov        cl, byte ptr [esp + 0x53]
004631ac  and        eax, 0x80
004631b1  or         esi, eax
004631b3  and        dl, 0x80
004631b6  movzx      eax, dl
004631b9  neg        eax
004631bb  sbb        eax, eax
004631bd  and        eax, 0x90
004631c2  or         esi, eax
004631c4  and        cl, 0x80
004631c7  mov        al, byte ptr [esp + 0x4b]
004631cb  movzx      edx, cl
004631ce  neg        edx
004631d0  sbb        edx, edx
004631d2  and        al, 0x80
004631d4  and        edx, 0x60
004631d7  movzx      ecx, al
004631da  or         esi, edx
004631dc  movzx      edx, byte ptr [esp + 0x55]
004631e1  neg        ecx
004631e3  sbb        ecx, ecx
004631e5  and        dl, 0x80
004631e8  and        ecx, 0x50
004631eb  movzx      eax, dl
004631ee  or         esi, ecx
004631f0  neg        eax
004631f2  sbb        eax, eax
004631f4  and        eax, 0xa0

// block 004631f9
004631f9  or         esi, eax

// block 004631fb
004631fb  mov        ecx, esi
004631fd  call       0x462a80

// block 00463202
00463202  mov        ecx, dword ptr [0x4d48b8]
00463208  mov        esi, eax
0046320a  mov        dword ptr [0x4d48bc], ecx
00463210  mov        ecx, 0x4d48b8
00463215  mov        dword ptr [0x4d48b8], esi
0046321b  call       0x465440

// block 00463220
00463220  mov        ecx, dword ptr [esp + 0x104]
00463227  mov        eax, esi
00463229  pop        esi
0046322a  xor        ecx, esp
0046322c  call       0x46c932

// block 00463231
00463231  add        esp, 0x104
00463237  ret        
