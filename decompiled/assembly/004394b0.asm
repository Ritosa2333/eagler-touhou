
// block 004394b0
004394b0  cmp        dword ptr [eax], 0
004394b3  jne        0x4394be

// block 004394b5
004394b5  cmp        dword ptr [eax + 4], 0
004394b9  jne        0x4394be

// block 004394bb
004394bb  xor        eax, eax
004394bd  ret        

// block 004394be
004394be  mov        eax, 1
004394c3  ret        
