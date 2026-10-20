
// block 004daef1
004daef1  lodsd      eax, dword ptr [esi]
004daef2  jne        0x4daf26

// block 004daef4
004daef4  mov        ch, 0xaa
004daef6  daa        
004daef7  cmp        byte ptr [esp + edi*4 + 0x772ad7c], cl
