#!/usr/bin/env python3
"""Simple DATUM assembler: assembly string -> LaTeX array output
This is an abstract machine assembler for a hypothetical machine called DATUM.
It translates a simple assembly language into a flat list of complex floats,
Usage:
  - as a module: call `assemble_to_latex(assembly_str)`
  - CLI: `python scripts/assembler.py input.asm` or pipe assembly on stdin
"""
from __future__ import annotations
import sys
from typing import List

OPCODES = {
    "NOP": 1,
    "SET": 2,
    "HALT": 3,
    "ADD": 4,
    "SUB": 5,
    "MULT": 6,
    "DIV": 7,
    "MOD": 8,
    "POW": 9,
    "LOG": 10,
    "COPY": 11,
    "JMP": 12,
    "ALOC": 13
}

largest_index=100  # Ensure minimum output length
def parse_arg(token: str) -> float:
    """Parse a single argument token into an integer value.

    Supported forms:
    - "=5"  immediate value 5
    - "&12" address 12
    - "5"   numeric literal
    These are really just cosmetic, as the argument types are determined by the opcode.
    """
    t = token.strip()
    if not t:
        return 0
    if t.startswith("=") or t.startswith("&"):
        try:
            return float(t[1:])
        except ValueError:
            raise ValueError(f"Invalid numeric argument: {token}")
    try:
        return float(t)
    except ValueError:
        raise ValueError(f"Unsupported argument format: {token}")


def assemble(assembly: str) -> List[float]:
    """Assemble a multi-line assembly string into a flat list of integers.

    Each instruction becomes 4 integers: [opcode, a, b, c].
    If fewer than 3 args are provided for an instruction, remaining slots are 0.
    Blank lines and lines starting with '#' are ignored.
    """
    words: List[float] = [4,0,0]  # Initial state of A, with PC starting at 4
    for raw_line in assembly.splitlines():
        line = raw_line.split("#", 1)[0].strip()
        if not line:
            continue
        parts = [p for p in line.replace(',', ' ').split() if p]
        if not parts:
            continue
        mnemonic = parts[0].upper()
        if mnemonic not in OPCODES:
            raise ValueError(f"Unknown mnemonic: {mnemonic}")
        opcode = OPCODES[mnemonic]
        args = parts[1:]
        nums = [opcode]
        for i in range(3):
            if i < len(args):
                nums.append(parse_arg(args[i]))
            else:
                nums.append(0)
        words.extend(nums)
    while len(words) < largest_index:
        words.append(0)
    return words


def assemble_to_latex(assembly: str) -> str:
    """Return LaTeX-formatted array string for the assembled words.

    Example output: "\\[A\\to\\left[4,0,0,4,20,21\\right]\\]"
    """
    words = assemble(assembly)
    inner = ",".join(str(x) for x in words)
    # Escape backslashes for literal LaTeX in a Python string output
    return f"A\\to\\left[{inner}\\right]"


def _main(argv: List[str]):
    if len(argv) >= 2:
        path = argv[1]
        with open(path, "r", encoding="utf-8") as f:
            assembly = f.read()
    else:
        assembly = sys.stdin.read()
    try:
        print(assemble_to_latex(assembly))
    except Exception as e:
        print("Error:", e, file=sys.stderr)
        sys.exit(2)


if __name__ == "__main__":
    _main(sys.argv)
