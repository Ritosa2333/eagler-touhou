"""合并已完成的录像记录，核对全部 17 份正式 Runtime 回放。"""
from pathlib import Path
import argparse,hashlib,json

def read_cases(paths):
    rows={};identities=set();browsers=set()
    for path in paths:
        report=json.loads(path.read_text('utf-8'))
        identities.add(report['runtime']['manifest']['buildId']);browsers.add(report['browser'])
        for row in report['cases']:
            if row['result'] not in {'captured','passed'} or row['error'] or row['final']['cursor']!=row['inputFrames']-1:
                raise ValueError('录像尚未完成：'+row['id'])
            if row['id'] in rows and canonical(row)!=canonical(rows[row['id']]):
                raise ValueError('重复录像记录不一致：'+row['id'])
            rows[row['id']]=row
    expected={f'corpus-{i:02}' for i in range(1,18)}
    if set(rows)!=expected:raise ValueError('录像编号差异：'+str(sorted(set(rows)^expected)))
    if len(identities)!=1:raise ValueError('同组包含不同 Runtime')
    return rows,identities.pop(),sorted(browsers)

def canonical(row):
    return {key:row[key] for key in ['inputSha256','inputFrames','checkpoints','final']}

def digest(value):
    return hashlib.sha256(json.dumps(value,sort_keys=True,separators=(',',':')).encode()).hexdigest()

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--before',type=Path,nargs='+',required=True)
    parser.add_argument('--after',type=Path,nargs='+',required=True)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();before,old,browsers_before=read_cases(args.before);after,new,browsers_after=read_cases(args.after)
    cases=[]
    for name in sorted(before):
        expected,actual=canonical(before[name]),canonical(after[name])
        if expected!=actual:
            for a,b in zip(expected['checkpoints'],actual['checkpoints']):
                if a!=b:raise ValueError(f'{name} 首个检查点差异：游标 {a["cursor"]}，原状态 {a}，当前状态 {b}')
            raise ValueError(name+' 输入身份、检查点数量或结束状态不同')
        cases.append({'id':name,'result':'passed','inputSha256':actual['inputSha256'],'inputFrames':actual['inputFrames'],
                      'checkpoints':len(actual['checkpoints']),'checkpointsSha256':digest(actual['checkpoints']),'final':actual['final']})
    result={'schema':'thkkd/production-replay-comparison/1','result':'passed','beforeRuntime':old,'afterRuntime':new,
            'browsersBefore':browsers_before,'browsersAfter':browsers_after,'replays':len(cases),'inputFrames':sum(r['inputFrames'] for r in cases),
            'checkpoints':sum(r['checkpoints'] for r in cases),'cases':cases}
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps({k:result[k] for k in ['result','replays','inputFrames','checkpoints']},ensure_ascii=False))

if __name__=='__main__':main()
