#!/usr/bin/env python3
from pathlib import Path
import re
import sys
import os

root = Path(os.environ.get("ROOT", Path(__file__).resolve().parents[2]))
broken = []
for md in root.rglob("*.md"):
    if "build" in md.parts:
        continue
    text = md.read_text(errors="ignore")
    for m in re.finditer(r"\[([^\]]+)\]\(([^)]+)\)", text):
        url = m.group(2)
        if url.startswith(("http://", "https://", "mailto:")):
            continue
        url = url.split("#")[0]
        if not url:
            continue
        target = (md.parent / url).resolve()
        if not target.exists():
            broken.append(f"{md.relative_to(root)}: {url}")
if broken:
    print("Broken links:")
    print("\n".join(broken))
    sys.exit(1)
print("Doc links OK")
