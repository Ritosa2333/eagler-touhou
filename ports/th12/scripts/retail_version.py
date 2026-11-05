"""Exact executable identities for static TH12 1.00b content preparation."""
import hashlib
import json
from pathlib import Path

VERSIONS = json.loads(Path(__file__).with_name('retail-versions.json').read_text(encoding='utf-8'))


def validate_executable(path):
    digest = hashlib.sha256(Path(path).read_bytes()).hexdigest()
    if digest not in VERSIONS['executables']:
        raise ValueError('TH12 static tables require a supported 1.00b executable; SHA-256: ' + digest)
    return digest
