
// block 004940d1
004940d1  push       ebp
004940d2  mov        ebp, esp
004940d4  add        esp, 0xfffffd30
004940da  push       ebx
004940db  push       dword ptr [ebp + 0xc]
004940de  push       dword ptr [ebp + 8]
004940e1  call       0x494104

// block 004940e6
004940e6  add        esp, 8
004940e9  wait       
004940ea  fnstcw     word ptr [ebp - 0xa4]
004940f0  and        byte ptr [ebp - 0x2c8], 0xfd
004940f7  call       0x494140

// block 004940fc
004940fc  call       0x493f83

// block 00494101
00494101  pop        ebx
00494102  leave      
00494103  ret        
