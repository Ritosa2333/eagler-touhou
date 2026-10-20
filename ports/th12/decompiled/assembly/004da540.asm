
// block 004da540
004da540  jae        0x4da5a8

// block 004da542
004da542  popal      
004da543  push       0x543db05f
004da548  sbb        eax, 0x8a9aff42
004da54d  sbb        ch, byte ptr [esp + esi*4 + 0x2b9d1984]
004da554  mov        bl, 0xa9
004da557  scasd      eax, dword ptr es:[edi]
004da558  jo         0x4da584

// block 004da55a
004da55a  push       ebx
004da55b  pop        ebx
004da55c  pop        ss
004da55d  xor        dword ptr [esi], 0xffffffd4
004da560  mov        edi, edx
004da562  adc        dword ptr [ebp - 0x4d], edi
004da565  lodsb      al, byte ptr [esi]
004da566  out        0x2d, eax

// block 004da573
004da573  leave      
004da574  mov        ecx, 0x6d7efb53

// block 004da584
004da584  fidiv      dword ptr [ebp + ebx - 0x38]
004da588  fisttp     word ptr [esi + 0x41990df7]
004da58e  pop        esp
004da58f  jl         0x4da573

// block 004da591
004da591  wait       

// block 004da592
004da592  int3       

// block 004da5a8
004da5a8  cmp        al, 0xd9
004da5aa  iretd      
