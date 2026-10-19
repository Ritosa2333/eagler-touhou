"""Convert existing Traditional Chinese dialogue with OpenCC, not translation."""
import json
import sys
from opencc import OpenCC

converter = OpenCC('t2s')
with open(sys.argv[1], encoding='utf-8') as source:
    tables = json.load(source)
converted = [{key: converter.convert(value) for key, value in table.items()} for table in tables]
with open(sys.argv[2], 'w', encoding='utf-8') as target:
    json.dump(converted, target, ensure_ascii=False)
