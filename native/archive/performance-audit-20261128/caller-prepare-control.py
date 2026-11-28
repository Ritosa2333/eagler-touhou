from pathlib import Path
p=Path(__file__).resolve().parent
source=(p/'caller-window.cpp').read_text().replace('caller-events.jsonl','caller-control-events.jsonl').replace('sample_count==6','sample_count==12')
source=source.replace('RECT frame = GetClientArea();','SetWindowText(GetHandle(), L"TouhouOnline | caller control");\n  RECT frame = GetClientArea();')
(p/'caller-control-window.cpp').write_text(source)
for kind in ['compile','link']:
    text=(p/f'caller-{kind}.rsp').read_text().replace('caller-window','caller-control-window').replace('caller-host.exe','caller-control-host.exe')
    (p/f'caller-control-{kind}.rsp').write_text(text)
(p/'build-caller-control.cmd').write_text((p/'build-caller.cmd').read_text().replace('caller-compile.rsp','caller-control-compile.rsp').replace('caller-link.rsp','caller-control-link.rsp'))
run=(p/'run-caller.py').read_text().replace('caller-events','caller-control-events').replace('caller-stdout','caller-control-stdout').replace('caller-result','caller-control-result').replace('caller-host.exe','caller-control-host.exe').replace('<48','<78').replace('signaledWithin48s','signaledWithin78s')
(p/'run-caller-control.py').write_text(run)
