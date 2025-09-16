import os
from mock_reads import mutation
import sys


def read_fasta_with_headers(path):
    records = []
    header = None
    current = []
    with open(path, 'r') as fh:
        for line in fh:
            line = line.strip()
            if not line:
                continue
            if line.startswith('>'):
                if header is not None:
                    records.append((header, ''.join(current)))
                header = line[1:].strip()  # drop leading '>'
                current = []
            else:
                current.append(line)
    if header is not None:
        records.append((header, ''.join(current)))
    return records


def run_tests():
    try:
        if len(sys.argv) < 6:
            print("Usage: mock_reads_from_reference.py <path-to-fasta> <output-reads-fa> <num-reads> <kmer-size> <max-mismatches>")
            return 1
        fasta_path = sys.argv[1]
        output_reads_fa = sys.argv[2]
        num_reads = int(sys.argv[3])
        kmer_size = int(sys.argv[4])
        max_mismatches = int(sys.argv[5])
    except Exception as e:
        print(f"Error reading arguments: {e}")
        return 1

    records = read_fasta_with_headers(fasta_path)
    if not records:
        raise SystemExit("No sequences found in the provided FASTA file.")
    for i in range(num_reads):
        header, sequence = records[i % len(records)]
        if len(sequence) < kmer_size:
            continue
        start_index = i % (len(sequence) - kmer_size + 1)
        read_seq, num_mutations = mutation(sequence[start_index:start_index + kmer_size], max_mismatches)
        with open(output_reads_fa, 'a') as f:
            f.write(f">r{i}-{num_mutations}-{header}{os.linesep}")
            f.write(f"{read_seq}{os.linesep}")
    print(f"Generated {num_reads} reads in {output_reads_fa}")


if __name__ == "__main__":
    run_tests()