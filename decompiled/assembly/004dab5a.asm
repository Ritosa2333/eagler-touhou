
// block 004dab5a
004dab5a  jp         0x4dab9d

// block 004dab5c
004dab5c  cmc        
004dab5d  ret        

// block 004dab75
004dab75  cmc        
004dab76  add        edx, edx

// block 004dab9d
004dab9d  cmp        al, 0x7a
004dab9f  cmp        dword ptr [edi - 0x56c89572], ecx
004daba5  cmp        dword ptr [ebx], edi
004daba7  jmp        0x4dab75
