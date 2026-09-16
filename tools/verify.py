#!/usr/bin/env python3
"""Проверка комплектности офлайн-комплекта для сборки термоциркулятора.

Сверяет каждую позицию tools/manifest.json: наличие файла, размер и sha256.
Печатает, чего не хватает и откуда это взять.

Запуск:
    python tools/verify.py            обычная проверка
    python tools/verify.py --update   пересчитать размеры и хеши имеющихся
                                      файлов и записать их в манифест

Коды возврата: 0 — всё на месте, 1 — не хватает обязательных позиций.
"""

import argparse
import hashlib
import json
import sys
from pathlib import Path

# Консоль Windows по умолчанию в cp866 — без этого кириллица превращается в мусор.
for stream in (sys.stdout, sys.stderr):
    try:
        stream.reconfigure(encoding="utf-8", errors="replace")
    except (AttributeError, OSError):
        pass

TOOLS = Path(__file__).resolve().parent
MANIFEST = TOOLS / "manifest.json"

OK, MISS, BAD, EXTRA = "OK", "НЕТ ФАЙЛА", "НЕ СОВПАЛ", "БЕЗ ЭТАЛОНА"


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def human(n):
    if n is None:
        return "—"
    for unit in ("Б", "КБ", "МБ", "ГБ"):
        if n < 1024:
            return f"{n:.0f} {unit}" if unit == "Б" else f"{n:.1f} {unit}"
        n /= 1024
    return f"{n:.1f} ТБ"


def check(item):
    """Возвращает (статус, подробность)."""
    path = TOOLS / item["path"]
    if not path.exists():
        return MISS, None
    size = path.stat().st_size
    if item.get("size") and size != item["size"]:
        return BAD, f"размер {human(size)}, ожидался {human(item['size'])}"
    if not item.get("sha256"):
        return EXTRA, f"{human(size)}, эталонный хеш не задан"
    actual = sha256(path)
    if actual != item["sha256"]:
        return BAD, f"sha256 {actual[:16]}…, ожидался {item['sha256'][:16]}…"
    return OK, human(size)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--update", action="store_true",
                    help="записать в манифест размеры и хеши найденных файлов")
    args = ap.parse_args()

    if not MANIFEST.exists():
        print(f"Не найден манифест: {MANIFEST}", file=sys.stderr)
        return 2

    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    items = data["items"]

    if args.update:
        changed = 0
        for item in items:
            path = TOOLS / item["path"]
            if path.exists() and not item.get("sha256"):
                item["size"] = path.stat().st_size
                item["sha256"] = sha256(path)
                changed += 1
                print(f"записан хеш: {item['path']}")
        if changed:
            MANIFEST.write_text(
                json.dumps(data, ensure_ascii=False, indent=2) + "\n",
                encoding="utf-8")
        print(f"обновлено позиций: {changed}")
        return 0

    print(f"Офлайн-комплект: {TOOLS}")
    print(f"Манифест от {data['updated']}, позиций: {len(items)}\n")

    width = max(len(i["path"]) for i in items)
    missing_required, problems = [], []

    for item in items:
        status, detail = check(item)
        mark = {OK: "  ", EXTRA: "? ", MISS: "! ", BAD: "X "}[status]
        req = "обяз." if item["required"] else "опц. "
        print(f"{mark}{item['path']:<{width}}  {req}  {status}"
              + (f"  ({detail})" if detail else ""))
        if status == MISS and item["required"]:
            missing_required.append(item)
        elif status == BAD:
            problems.append(item)

    print()

    if problems:
        print("НЕ СОВПАЛИ КОНТРОЛЬНЫЕ СУММЫ:\n")
        for item in problems:
            print(f"  {item['path']}")
            print(f"      Файл отличается от зафиксированного в манифесте.")
            print(f"      Источник: {item['source']}\n")

    if missing_required:
        print("НЕ ХВАТАЕТ ОБЯЗАТЕЛЬНЫХ ПОЗИЦИЙ:\n")
        for item in sorted(missing_required, key=lambda i: i["step"] or 99):
            print(f"  [{item['step']}] {item['name']} {item['version']}")
            print(f"      положить в: tools/{item['path']}")
            print(f"      источник:   {item['source']}")
            if item.get("note"):
                print(f"      примечание: {item['note']}")
            print()
        print("Порядок установки — docs/03-environment.md")
        print("После пополнения: python tools/verify.py --update")
        return 1

    if problems:
        return 1

    print("Комплект полон. Порядок установки — docs/03-environment.md")
    return 0


if __name__ == "__main__":
    sys.exit(main())
