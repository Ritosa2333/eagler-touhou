"""Map retained source-location diagnostic strings to static cross-references.
These names are evidence from the binary, not a reconstruction of source code.
"""
import bisect, csv, json, pathlib, re
ROOT=pathlib.Path(__file__).resolve().parents[1]
OUT=ROOT/'analysis'/'binary'
def read(name):
    with (OUT/name).open(encoding='utf-8',newline='') as f:return list(csv.DictReader(f))
def write(name,fields,rows):
    with (OUT/name).open('w',encoding='utf-8',newline='') as f:
        w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(rows)
def main():
    funcs=read('function_candidates.csv');starts=[int(x['address'],16) for x in funcs]
    markers={};rows=[]
    for s in read('strings.csv'):
        m=re.search(r'\\src\\([^:]+):(\d+)\s+(.*)',s['text'])
        if m:markers[s['va']]=dict(string_va=s['va'],source_path=m.group(1).replace('\\','/'),source_line=int(m.group(2)),type_or_context=m.group(3),raw_text=s['text'])
    for x in read('xrefs.csv'):
        if x['to_va'] not in markers:continue
        va=int(x['from_va'],16);i=bisect.bisect_right(starts,va)-1
        rows.append({**markers[x['to_va']], 'reference_va':x['from_va'],'candidate_function':funcs[i]['address'] if i>=0 else '', 'instruction':x['instruction']})
    write('source_location_xrefs.csv',['source_path','source_line','type_or_context','string_va','reference_va','candidate_function','instruction','raw_text'],rows)
    grouped={}
    for row in markers.values():
        grouped.setdefault(row['source_path'],[]).append({k:v for k,v in row.items() if k!='source_path'})
    (OUT/'retained_source_layout.json').write_text(json.dumps(grouped,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({'source_files':len(grouped),'source_location_strings':len(markers),'source_location_xrefs':len(rows)},indent=2))
if __name__=='__main__':main()
