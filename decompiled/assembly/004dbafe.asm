
// block 004dbafe
004dbafe  xor        eax, ebx
004dbb00  test       word ptr [edx - 0x1e73844a], bx
004dbb07  xor        ecx, dword ptr [esi - 0x7c8f9f32]
004dbb0d  je         0x4dbb47

// block 004dbb0f
004dbb0f  rol        edi, 0x8f
004dbb12  dec        eax
004dbb13  cmp        eax, 0xd087e570
004dbb18  push       ds
004dbb19  sub        al, ah
004dbb1b  mov        dword ptr [eax + ecx*2], edx
004dbb1e  ror        byte ptr [eax], cl
004dbb20  add        byte ptr [esi - 0x37], 0xa5
004dbb24  and        eax, 0x72892c09
004dbb29  dec        ebx
004dbb2a  mov        dl, 0x49
004dbb2c  mov        ch, 0x8a
004dbb2e  rol        dword ptr [0x62292c80], cl
004dbb34  dec        ebx
004dbb35  mov        byte ptr [0x2c5b792c], al
004dbb3a  mov        dword ptr [ebp - 0x7b], esi
004dbb3d  xchg       ebp, eax
004dbb3e  sub        al, 0xf1
004dbb40  pop        esi
004dbb41  xchg       edi, eax

// block 004dbb47
004dbb47  push       0xcbfb925b
004dbb4c  add        dl, byte ptr [eax]
004dbb4e  mov        eax, dword ptr [0xa6e11118]
004dbb53  or         eax, 0x54426c68
004dbb58  aaa        
004dbb59  and        dword ptr [edx], esi
004dbb5b  adc        esp, dword ptr [ecx + 0x44]
004dbb5e  adc        al, 0xc1
004dbb60  push       eax
004dbb61  adc        eax, 0xf00bdd81
004dbb66  stc        
004dbb67  xchg       byte ptr [eax + 0x65], al
004dbb6a  int        0x9b
