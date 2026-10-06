
// block 004dbbbd
004dbbbd  mov        bl, 0x6e
004dbbbf  fnstsw     word ptr [ebx + 0x7f9bf0c6]
004dbbc5  jb         0x4dbbe4

// block 004dbbc7
004dbbc7  sar        dword ptr [eax - 0x2823db69], 0xb9
004dbbce  cmp        bh, ah
004dbbd0  sub        eax, 0xdc2d07cb
