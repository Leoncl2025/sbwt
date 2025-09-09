import json
import os
import subprocess
import sys
from typing import List
from mock_reads import generate_reads_ref

def run(cmd: List[str], *, expect_rc: int = 0) -> subprocess.CompletedProcess:
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != expect_rc:
        print(f"Command {' '.join(cmd)} failed: rc={proc.returncode}\nstdout=\n{proc.stdout}\nstderr=\n{proc.stderr}")
        raise SystemExit(1)
    return proc


def assert_equal(actual, expected, msg: str = ""):
    if actual != expected:
        print(f"Assertion failed: {actual!r} != {expected!r} {msg}")
        raise SystemExit(2)


def main() -> int:
    if len(sys.argv) < 2:
        print("Usage: test_build_index_e2e.py <path-to-my_app>")
        return 99
    exe = sys.argv[1]
    if not os.path.isfile(exe):
        print(f"Executable not found: {exe}")
        return 98
    
    ref_fa = "test_ref.fa"
    reads_fa = "test_reads.fa"
    generate_reads_ref(ref_fa, reads_fa, ref_size=10000, kmer=150, reads_size=100)
    run([exe, ref_fa, '3', '50'])
    print("All e2e checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())