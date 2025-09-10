import os
import sys
from test_build_index_e2e import run
from mock_reads import generate_reads_ref

def run_e2e(exe_build_index, exe_sbwt, max_mismatches=2) -> str:
    ref_fa = "test_ref.fa"
    reads_fa = "test_reads.fa"
    generate_reads_ref(ref_fa, reads_fa, ref_size=10000, kmer=150, reads_size=100, max_mismatches=max_mismatches)
    try:
        run([exe_build_index, ref_fa, '3', '50'])
        process = run([exe_sbwt, reads_fa, ref_fa+'.3'])
        return process.stderr
    except SystemExit as e:
        return e.code
    finally:
        if os.path.isfile(ref_fa):
            os.remove(ref_fa)
        if os.path.isfile(reads_fa):
            os.remove(reads_fa)
        if os.path.isfile(ref_fa+'.3.array.sbwt'):
            os.remove(ref_fa+'.3.array.sbwt')
        if os.path.isfile(ref_fa+'.3.meta.sbwt'):
            os.remove(ref_fa+'.3.meta.sbwt')
        if os.path.isfile(ref_fa+'.3.second.sbwt'):
            os.remove(ref_fa+'.3.second.sbwt')


def sbwt_e2e() -> int:
    if len(sys.argv) < 3:
        print("Usage: test_sbwt_e2e.py <path-to-build_index> <path-to-sbwt>")
        return 99
    exe_build_index = sys.argv[1]
    exe_sbwt = sys.argv[2]
    if not os.path.isfile(exe_build_index):
        print(f"Executable not found: {exe_build_index}")
        return 98
    if not os.path.isfile(exe_sbwt):
        print(f"Executable not found: {exe_sbwt}")
        return 97

    try:
        ret = run_e2e(exe_build_index, exe_sbwt, max_mismatches=2)
        if "Reads with alignment:	100 (100%)" not in ret:
            print(f"all reads should be aligned with max 2 mismatches, got:\n{ret}")
            return 2
        
        ret = run_e2e(exe_build_index, exe_sbwt, max_mismatches=10)
        if "Reads with alignment:	100 (100%)" in ret:
            print(f"all reads should not be aligned with max 10 mismatches, got:\n{ret}")
            return 3
        print("All e2e checks passed")

    except Exception as e:
        print(f"e2e test encountered an exception: {e}")
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(sbwt_e2e())