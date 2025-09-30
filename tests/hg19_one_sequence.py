import os
import sys
from mock_reads_from_refernce import read_fasta_with_headers


def run_tests():
    try:
        if len(sys.argv) < 3:
            print("Usage: hg19_one_sequence.py <path-to-fasta> <output-path-fasta>")
            return 1
        fasta_path = sys.argv[1]
        output_fa = sys.argv[2]
        records = read_fasta_with_headers(fasta_path)
        if not records:
            raise SystemExit("No sequences found in the provided FASTA file.")
        write_header = False
        for header, sequence in records:  # only the first sequence
            with open(output_fa, 'a') as f:
                if not write_header:
                    write_header = True
                    f.write(f">{header}{os.linesep}")
                f.write(f"{sequence}{os.linesep}")
        print(f"Wrote one sequence to {output_fa}")
    except Exception as e:
        print(f"Error reading arguments: {e}")
        return 1


if __name__ == "__main__":
    run_tests()