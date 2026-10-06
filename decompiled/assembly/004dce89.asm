
// block 004dce89
004dce89  mov        eax, 0x6f
004dce8e  add        byte ptr [eax], al
004dce90  add        byte ptr [eax], al
004dce92  add        byte ptr [eax], al
004dce94  add        byte ptr [eax], al
004dce96  add        byte ptr [eax], al
004dce98  add        byte ptr [eax - 0x11ffff90], ch
004dce9e  jo         0x4dcea0

// block 004dcea0
004dcea0  add        byte ptr [eax], al
004dcea2  add        byte ptr [eax], al
004dcea4  add        byte ptr [eax], al
004dcea6  add        byte ptr [eax], al
004dcea8  add        byte ptr [eax], al
004dceaa  add        byte ptr [eax], al
004dceac  add        byte ptr [ebx - 0x9ffff90], dh
004dceb2  jo         0x4dceb4

// block 004dceb4
004dceb4  add        byte ptr [eax], al
004dceb6  add        byte ptr [eax], al
004dceb8  add        byte ptr [eax], al
004dceba  add        byte ptr [eax], al
004dcebc  add        byte ptr [eax], al
004dcebe  add        byte ptr [eax], al
004dcec0  add        byte ptr [edi - 0x1ffff90], bh
004dcec6  jo         0x4dcec8

// block 004dcec8
004dcec8  add        byte ptr [eax], al
004dceca  add        byte ptr [eax], al
004dcecc  add        byte ptr [eax], al
004dcece  add        byte ptr [eax], al
004dced0  add        byte ptr [eax], al
004dced2  add        byte ptr [eax], al
004dced4  add        dl, cl
004dced6  jo         0x4dced8

// block 004dced8
004dced8  add        byte ptr [esi], al
004dceda  jno        0x4dcedc

// block 004dcedc
004dcedc  add        byte ptr [eax], al
004dcede  add        byte ptr [eax], al
004dcee0  add        byte ptr [eax], al
004dcee2  add        byte ptr [eax], al
004dcee4  add        byte ptr [eax], al
004dcee6  add        byte ptr [eax], al
004dcee8  add        ch, dl
004dceea  jo         0x4dceec

// block 004dceec
004dceec  add        byte ptr [esi], cl
004dceee  jno        0x4dcef0

// block 004dcef0
004dcef0  add        byte ptr [eax], al
004dcef2  add        byte ptr [eax], al
004dcef4  add        byte ptr [eax], al
004dcef6  add        byte ptr [eax], al
004dcef8  add        byte ptr [eax], al
004dcefa  add        byte ptr [eax], al
004dcefc  add        dl, ah
004dcefe  jo         0x4dcf00

// block 004dcf00
004dcf00  add        byte ptr [esi], dl
004dcf02  jno        0x4dcf04

// block 004dcf04
004dcf04  add        byte ptr [eax], al
004dcf06  add        byte ptr [eax], al
004dcf08  add        byte ptr [eax], al
004dcf0a  add        byte ptr [eax], al
004dcf0c  add        byte ptr [eax], al
004dcf0e  add        byte ptr [eax], al
004dcf10  add        byte ptr [eax], al
004dcf12  add        byte ptr [eax], al
004dcf14  add        byte ptr [eax], al
004dcf16  add        byte ptr [eax], al
004dcf18  add        byte ptr [ebp + 0x73], ch
004dcf1b  jbe        0x4dcf80

// block 004dcf1d
004dcf1d  jb         0x4dcf93

// block 004dcf93
004dcf93  xor        al, byte ptr [eax + 0x59]
004dcf96  inc        ecx
004dcf97  push       eax
004dcf98  inc        ecx
004dcf99  pop        eax
004dcf9a  dec        ecx
004dcf9b  inc        eax
004dcf9c  pop        edx
004dcf9d  add        byte ptr [eax], al
004dcf9f  add        byte ptr [eax + 0x61], dl
004dcfa2  je         0x4dd00c

// block 004dcfa4
004dcfa4  inc        esi
004dcfa5  imul       ebp, dword ptr [ebp + 0x45], 0x74736978
004dcfad  jae        0x4dcff0

// block 004dcfaf
004dcfaf  add        byte ptr [eax], al
004dcfb1  add        byte ptr [ebp + 0x52], dl
004dcfb4  dec        esp
004dcfb5  inc        esp

// block 004dcff0
004dcff0  popal      
