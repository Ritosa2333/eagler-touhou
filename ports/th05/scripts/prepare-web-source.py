"""Stage legacy Windows sources for clang without changing Shift-JIS bytes.

Source strings are byte strings (the DAT files use CP932). Preserve those
bytes using octal escapes; normalize only comments and include spelling.
"""
from pathlib import Path
import re, argparse

def stage(root, out):
    files = [p for owner in ('Game','gameLib','third_party/imgui','portable','tests')
             for p in (root/owner).rglob('*') if p.suffix in ('.cpp','.h','.hpp','.inc')]
    spelling = {str(p.relative_to(root)).replace('\\','/').lower():p.relative_to(root).as_posix() for p in files}
    tokens = re.compile(rb'//[^\r\n]*|/\*[\s\S]*?\*/|(?:u8|[LuU])?"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\'')
    def convert(match):
        b=match.group()
        if b.startswith((b'//',b'/*')):
            try:return b.decode('utf-8').encode()
            except UnicodeDecodeError:return b.decode('cp932',errors='replace').encode()
        return b''.join(bytes([v]) if v<128 else ('\\%03o'%v).encode() for v in b)
    for path in files:
        rel=path.relative_to(root); data=path.read_bytes()
        def include(m):
            name=m[2].decode().replace('\\','/')
            candidate=(path.parent/name).resolve()
            try:key=candidate.relative_to(root).as_posix().lower()
            except ValueError:return m[0]
            if key in spelling:
                import os
                name=os.path.relpath(root/spelling[key],path.parent).replace('\\','/')
            return m[1]+name.encode()+m[3]
        data=re.sub(rb'(^\s*#\s*include\s*")([^"\r\n]+)(")',include,data,flags=re.M)
        data=tokens.sub(convert,data)
        target=out/rel;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(data)
    print('Staged',len(files),'source files')
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--out',type=Path,required=True);args=parser.parse_args()
    stage(Path(__file__).resolve().parents[1],args.out.resolve())
