
// block 0040f720
0040f720  test       dword ptr [eax + 0x590], 0x2000
0040f72a  jne        0x40f733

// block 0040f72c
0040f72c  mov        dword ptr [eax + 0x558], ecx
0040f732  ret        

// block 0040f733
0040f733  mov        dword ptr [eax + 0x558], 2
0040f73d  ret        
