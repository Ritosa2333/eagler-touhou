"""Observe actual43a810 vertical-band endpoints, timer reset and launch."""
import pathlib,json,struct,hashlib
root=pathlib.Path(__file__).resolve().parents[2]
source=root/'scripts/reverse/inspect-player-targeting.py'
ns={'__file__':str(source),'__name__':'native_sanae_a_definitions'}
exec(compile(source.read_text(encoding='utf-8').split('rows=[]')[0],str(source),'exec'),ns)
mu,put,heap,enemy,stack,stop=(ns[k] for k in ['mu','put','heap','enemy','stack','stop'])
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_ESP,UC_X86_REG_EDX,UC_X86_REG_EIP,UC_X86_REG_EBX
put(enemy+0x26f8,'I',0);put(enemy+0x27b4,'I',94);put(enemy+0x1074,'fff',-42.26,160,0)
projectile=heap+0x9000;observed_interrupts=[]
mu.hook_add(UC_HOOK_CODE,lambda u,a,s,d:observed_interrupts.append(mu.reg_read(UC_X86_REG_EBX)),begin=0x461970,end=0x461970)
read=lambda p,fmt:struct.unpack('<'+fmt,mu.mem_read(p,struct.calcsize('<'+fmt)))
rows=[]
for y,age,phase in [(176.01,5,1),(176,5,1),(175.99,5,1),(128,5,1),(127.99,5,1),(128.01,5,1),(176,4,1),(176,5,2)]:
 mu.mem_write(projectile,bytes(0x78));put(projectile+4,'i',age);put(projectile+0x14,'fff',30,y,0)
 put(projectile+0x20,'fff',0,-20,0);put(projectile+0x2c,'ff',20,-1.5707963705062866);put(projectile+0x48,'i',phase)
 observed_interrupts.clear();put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDX,projectile)
 mu.emu_start(0x43a810,stop,count=10000)
 if mu.reg_read(UC_X86_REG_EIP)!=stop:raise RuntimeError('Sanae A update did not return')
 rows.append({'y':y,'age':age,'phase':phase,'afterAge':read(projectile+4,'i')[0],'speed':read(projectile+0x2c,'f')[0],'velocity':read(projectile+0x20,'ff'),'angle':read(projectile+0x30,'f')[0],'launchAngle':read(projectile+0x70,'f')[0],'interrupts':observed_interrupts.copy()})
launch=[]
for age in [1000,1001,1002,1003,1004]:
 put(projectile+4,'i',age);put(projectile+0x48,'i',1);put(projectile+0x2c,'f',0);put(projectile+0x70,'f',3.1415927410125732)
 put(stack,'I',stop);mu.reg_write(UC_X86_REG_ESP,stack);mu.reg_write(UC_X86_REG_EDX,projectile);mu.emu_start(0x43a810,stop,count=10000)
 launch.append({'age':age,'speed':read(projectile+0x2c,'f')[0],'angle':read(projectile+0x30,'f')[0]})
out=root/'artifacts/reverse/sanae-a-targeting-native.json'
out.write_text(json.dumps({'exeSha256':hashlib.sha256(ns['exe'].read_bytes()).hexdigest(),'function':'43a810','scope':'Original CPU instructions and actual deferred IRQ call with handle0, controlled enemy and projectile; no gameplay instruction or return replacement','radius':read(0x4a3d88,'f')[0],'rows':rows,'launch':launch},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'rows':rows,'launch':launch}))
