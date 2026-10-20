
// block 004942e6
004942e6  fstp       st(0)

// block 004942e8
004942e8  fstp       st(0)
004942ea  fld        xword ptr [0x4b35c0]
004942f0  cmp        byte ptr [ebp - 0x90], 0
004942f7  jg         0x494300

// block 00494300
00494300  or         cl, cl
00494302  ret        
