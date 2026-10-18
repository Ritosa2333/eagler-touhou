"""Compile Python sources in memory without creating __pycache__ artifacts."""

from pathlib import Path
import sys
import tokenize


for value in sys.argv[1:]:
    path = Path(value)
    with tokenize.open(path) as source:
        compile(source.read(), str(path), "exec")
