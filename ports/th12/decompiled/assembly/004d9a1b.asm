
// block 004d99ee
004d99ee  adc        cl, byte ptr [esi]
004d99f0  test       al, 0x71
004d99f2  aas        

// block 004d99f3
004d99f3  or         al, 0x2a

// block 004d9a1b
004d9a1b  xchg       esi, eax
004d9a1c  adc        byte ptr [ebp + 0x73fc0337], 0xd0
004d9a23  mov        byte ptr [edx + 0x65], 0x66
004d9a27  dec        edx
004d9a28  cmp        al, 0x5a
004d9a2a  xor        ch, cl
004d9a2c  imul       ecx, dword ptr [eax], 0x96829c49

// block 004d9a32
004d9a32  adc        bl, dl
004d9a34  sar        dword ptr [esi - 0x19], 0xe2

// block 004d9a39
004d9a39  and        eax, 0x6292408
004d9a3e  int        0x18

// block 004d9a40
004d9a40  je         0x4d9a54

// block 004d9a42
004d9a42  cmp        eax, 0x7efeda2c
004d9a47  adc        edi, esi
004d9a49  cwde       
004d9a4a  pop        ds
004d9a4b  pop        es
004d9a4c  and        al, 0x7e
004d9a4e  sub        edi, esi
004d9a50  adc        al, 0x8e
004d9a52  mov        ah, 0xe2

// block 004d9a54
004d9a54  xor        al, 0x61
004d9a56  push       edx
004d9a57  pop        esi
004d9a58  jl         0x4d99ee
