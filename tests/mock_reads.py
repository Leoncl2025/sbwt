from sys import argv
import os
import random

def IsDna(c):
    return c == 'A' or c == 'C' or c == 'G' or c == 'T'


def mutation(reads):
    d = {'A': "CGT", 'C': "AGT", 'G': "ACT", 'T': "ACG"}
    idxs = list(range(len(reads)))
    random.shuffle(idxs)
    # max 7 mismatches
    num = random.randrange(0, 8)
    # num = random.randrange(1, 4)
    for i in range(num):
        i = idxs[i]
        c = reads[i]
        reads = reads[:i] + random.choice(d[c]) + reads[i + 1:]
    return reads, num


def generate_reads_ref(file_name_ref, file_name_reads, ref_size, kmer, reads_size):
    """Generate reference and reads files.
    
    file_name_ref: reference file name
    file_name_reads: reads file name
    ref_size: size of reference sequence
    kmer: length of each read
    reads_size: total size of reads to generate
    """

    seed = 13
    ref = gen_reference(ref_size, file_name_ref, seed)
    random.seed(seed)
    with open(file_name_reads, 'w') as f:
        ref = ref.splitlines()[1]
        if len(ref) < kmer:
            raise SystemError("Reference size is smaller than kmer size")
        for i in range(0, reads_size):
            startI = random.randint(0, len(ref) - kmer)
            line, num = mutation(ref[startI:startI + kmer])
            f.write(f">r{i}-{num}-{i}{os.linesep}")
            f.write(f"{line}{os.linesep}")


def generate_reads_kmer(file_name, kmer, size):
    """Generate k-mer reads with possible mutations.

    Assuming there exists only one reference sequence (ignoring FASTA headers).
    """

    ref = ""
    with open(file_name) as mf:
        for ml in mf:
            if ml.startswith('>'):
                continue
            ref += ml.rstrip(os.linesep).replace("N", "A")
    if len(ref) < kmer or size < kmer:
        raise SystemExit(1)
    for i in range(0, size - kmer):
        line, num = mutation(ref[i:i + kmer])
        print(f">r{i}-{num}-{i}")
        print(line)


def cleanN_fasta(file_name):
    """Clean FASTA file, removing Ns and wrapping lines (Python 3 version)."""
    line_width = 70
    line = ""

    with open(file_name) as mf:
        for ml in mf:
            if ml.startswith('>'):
                if line:
                    print(line)
                    line = ""
                # replicate Python2 print with trailing space suppression
                print(ml.rstrip("\n"))
            else:
                for c in ml.strip():
                    if IsDna(c):
                        if len(line) == line_width:
                            print(line)
                            line = c
                        else:
                            line += c
    if line:
        print(line)


def generate_reads_kmer_main():
    """Main function for generating k-mer reads with possible mutations."""
    try:
        ref_fileName = argv[1]
        reads_fileName = argv[2]
        ref_size = int(argv[3])
        kmer = int(argv[4])
        reads_size = int(argv[5])
    except ValueError:
        raise SystemExit(f"usage: {argv[0]} [ref.fa name] [reads.fa name] [ref size] [kmer length (150)] [size of reads]{os.linesep}")

    generate_reads_ref(ref_fileName, reads_fileName, ref_size, kmer, reads_size)


def print_hamming_weight(r, f):
    num_diff = 0
    line0 = ""

    c = 0
    for i in range(len(r)):
        if i % 32 == 0:
            line0 += str(c)
            c += 1
        else:
            line0 += " "

    line = ""

    for i, j in zip(f, r):
        if i != j:
            num_diff += 1
            line += "|"
        else:
            line += " "
    print(str(num_diff))
    print(line0)
    print(r)
    print(line)
    print(f)


def gen_random_dnas(size, seed=None):
    """Generate a random DNA string containing equal numbers of A,C,G,T.
    """
    if (size / 4) < 1:
        print("size is too small")
        return ""
    if seed is not None:
        random.seed(seed)
    size = int(size / 4)
    a = "A" * size + "C" * size + "G" * size + "T" * size
    a = list(a)
    random.shuffle(a)
    return "".join(a)


def gen_reference(size, fileName, seed):
    """Generate a random reference sequence of given size."""
    ref = ">1" + os.linesep + gen_random_dnas(size, seed) + os.linesep
    with open(fileName, 'w') as f:
        f.write(ref)
    return ref


if __name__ == '__main__':
    generate_reads_kmer_main()

