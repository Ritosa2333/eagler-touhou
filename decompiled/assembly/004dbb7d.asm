
// block 004dbb3a
004dbb3a  mov        dword ptr [ebp - 0x7b], esi
004dbb3d  xchg       ebp, eax
004dbb3e  sub        al, 0xf1
004dbb40  pop        esi
004dbb41  xchg       edi, eax

// block 004dbb7d
004dbb7d  je         0x4dbbfa

// block 004dbb7f
004dbb7f  jecxz      0x4dbb7d

// block 004dbb81
004dbb81  dec        edi
004dbb82  xchg       esi, eax
004dbb83  inc        esi
004dbb84  movsd      dword ptr es:[edi], dword ptr [esi]
004dbb85  sbb        eax, dword ptr [ebp + edi + 0x241e4947]
004dbb8c  pop        ebp
004dbb8d  mov        byte ptr [0x3fd41efd], al
004dbb92  cmpsb      byte ptr [esi], byte ptr es:[edi]
004dbb93  cmp        dword ptr [edx], edi
004dbb95  test       bl, cl
004dbb97  cmpsb      byte ptr [esi], byte ptr es:[edi]
004dbb98  lea        esi, [0xa54bb529]
004dbb9e  push       0x65aa4255
004dbba3  jae        0x4dbb3a

// block 004dbbfa
004dbbfa  inc        ecx
004dbbfb  loopne     0x4dbc0c

// block 004dbbfd
004dbbfd  or         al, 0x6c
004dbbff  adc        al, byte ptr [eax]
004dbc01  lea        edi, [esi]
004dbc03  sub        ch, byte ptr [edi + ecx*4 + 9]
004dbc07  inc        esp
004dbc08  dec        esi

// block 004dbc0c
004dbc0c  mov        byte ptr [edx + edx*4 + 0x66], cl
004dbc10  pop        edx
004dbc11  fnsave     dword ptr [0xab59d738]
