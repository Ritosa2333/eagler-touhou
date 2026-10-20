
// block 004db865
004db865  inc        esp
004db866  mov        esp, 0x203468da
004db86b  mov        esi, eax
004db86d  mov        al, byte ptr [0x2f0f1937]
004db872  jecxz      0x4db8af

// block 004db874
004db874  xor        bh, 0x5d
004db877  mov        eax, dword ptr [0x9bc6085b]
004db87c  sub        al, 0xf3
004db87e  mov        ebx, 0x6685451f
004db883  lodsd      eax, dword ptr [esi]
004db884  xor        sp, word ptr [edi - 0x59c22ba0]

// block 004db8af
004db8af  wait       
004db8b0  xchg       edx, eax
004db8b1  mov        al, byte ptr [0x99f934a]
004db8b6  loope      0x4db91b

// block 004db8b8
004db8b8  adc        al, 0xb2
004db8bb  sub        eax, 0x9152571a
004db8c0  mov        word ptr [ebp + 0x6f], es
004db8c3  pop        ebp
004db8c4  and        bl, byte ptr [edx - 0x57]
004db8c7  sub        dword ptr [esi + 0x3b], 0x1b
004db8cb  cld        
004db8cd  lodsb      al, byte ptr [esi]
004db8ce  loope      0x4db8e7

// block 004db8d0
004db8d0  lodsd      eax, dword ptr [esi]

// block 004db8e7
004db8e7  in         eax, 0xf5
004db8ea  ret        

// block 004db91b
004db91b  fnstsw     word ptr [ebp - 0x53c43de8]
004db921  sub        dword ptr [ebp - 0x664a329a], eax
004db927  inc        ecx
004db928  mov        bh, 0xe2
004db92b  or         eax, 0x3d3bc79d
004db930  in         al, 0x5d
004db932  ljmp       0x1f58:0xa5e75679
