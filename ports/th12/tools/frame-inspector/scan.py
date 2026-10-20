"""Bounded exact comparison at original completed-tick boundaries."""
from collections import deque

FIELDS=('scoreStored','rng','piv','graze','power','playerFixed','bossLife','bossPhaseAge')

def validate_row(row):
    if not isinstance(row,dict):raise ValueError('检查数据无效')
    for key in ('stage','replayCursor',*FIELDS):
        value=row.get(key)
        if key in ('rng','playerFixed'):
            if not isinstance(value,list) or len(value)!=2 or any(type(v)!=int for v in value):raise ValueError('检查字段无效：'+key)
        elif type(value)!=int:raise ValueError('检查字段无效：'+key)
    spell=row.get('spell')
    if not isinstance(spell,dict) or any(type(spell.get(k))!=int for k in ('card','flags','age')):raise ValueError('符卡检查字段无效')
    return row

def compare(original,candidate):
    validate_row(original);validate_row(candidate)
    if original['stage']!=candidate['stage'] or original['replayCursor']!=candidate['replayCursor']:raise ValueError('录像帧未对齐')
    differences={key:{'original':original[key],'candidate':candidate[key]} for key in FIELDS if original[key]!=candidate[key]}
    # Native's bookkeeping bit40 has no gameplay meaning in the portable
    # SpellState. Compare active/card/age, not unrelated internal flags.
    def spell(row):
        s=row['spell'];active=bool(s['flags']&1)
        return {'active':active,'card':s['card'] if active else None,'age':s['age'] if active else None}
    a,b=spell(original),spell(candidate)
    if a!=b:differences['spell']={'original':a,'candidate':b}
    return differences

class ScanBatch:
    def __init__(self,base,rows):
        validate_row(base)
        if not isinstance(rows,list) or not 1<=len(rows)<=240:raise ValueError('每批检查需1～240帧')
        for i,row in enumerate(rows):
            validate_row(row)
            if row['stage']!=base['stage'] or row['replayCursor']!=base['replayCursor']+i+1:raise ValueError('逐帧检查数据不连续')
        self.rows=deque(rows);self.stage=base['stage'];self.cursor=base['replayCursor']
        self.checked=0;self.result={'status':'running','startFrame':self.cursor,'checkedFrame':self.cursor}
    def observe(self,original):
        if not original.get('replayActive') or original.get('stage')!=self.stage:raise ValueError('原版退出录像或切换关卡，检查已停止')
        if original['replayCursor']==self.cursor:return False  # Esc menu / redraw without a logical tick.
        if original['replayCursor']!=self.cursor+1:raise ValueError('原版录像帧不连续，不能判为通过')
        expected=self.rows[0];differences=compare(original,expected)
        self.cursor=original['replayCursor'];self.checked+=1
        self.result.update(checkedFrame=self.cursor)
        if differences:
            self.result.update(status='diverged',firstDifference={'frame':self.cursor,'fields':differences,'original':original,'candidate':expected})
            return True
        self.rows.popleft()
        if not self.rows:self.result['status']='batch-complete';return True
        return False
