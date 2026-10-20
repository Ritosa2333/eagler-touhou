
// block 004daeff
004daeff  mov        byte ptr [0x48aa8e35], al
004daf04  jg         0x4daeb4

// block 004daf06
004daf06  daa        
004daf07  pushfd     
004daf08  xchg       ebx, eax
004daf09  test       dh, ch
004daf0b  das        
004daf0c  cmp        eax, dword ptr [ebx - 2]
004daf0f  mov        ebx, 0x4e8ba43a
004daf14  mov        ah, 0x1b
004daf16  lahf       
004daf17  call       dword ptr [esi + ebp*8]

// block 004daf1a
004daf1a  das        
004daf1b  push       edi
004daf1c  xor        byte ptr [0xa01b6825], al
