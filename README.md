# Certified Tail Bounds for Prime-Power Threshold Cutoff Sequences

This repository contains the verification artifact for the paper:

**Certified Tail Bounds for Prime-Power Threshold Cutoff Sequences**  
Wesley Atwell

Archived releases DOI: https://doi.org/10.5281/zenodo.20725626

The paper proves an explicit certified tail bound for a family of prime-power threshold cutoff sequences. The proof combines an elementary analytic handoff with a finite exact-prefix verification by a deterministic exact-integer verifier.

The result is independent of the Riemann hypothesis, Robin's criterion, and Lagarias's criterion. Those topics motivate the cutoff sequence, but the theorem in this repository is a standalone certified cutoff-sequence theorem.

## Repository contents

| File | Purpose |
|---|---|
| `certified_tail_bounds_prime_power_cutoffs.tex` | LaTeX source of the paper |
| `certified_tail_bounds_prime_power_cutoffs.pdf` | Compiled paper |
| `certificate.csv` | Frozen threshold certificate: the 19 handoff rows |
| `verify_tail_certificate.cpp` | Standalone deterministic C++17 verifier |
| `BUILD_AND_VERIFY.txt` | Build, verification, and audit instructions |
| `SHA256SUMS` | SHA256 hashes for the archived artifact files |
| `SHA256SUMS_ALL` | SHA256 hashes for repository metadata and artifact files |
| `CITATION.cff` | Citation metadata for this artifact |
| `.zenodo.json` | Zenodo archive metadata for the current release |
| `LICENSE.md` | Licensing information |

## Main theorem

For a prime `t >= 83`, let `u` be the next prime after `t`, and define `C_k(t)` to be the largest prime `q`, or `0` if none exists, satisfying

```text
powerTailSum(q,k) < powerTailSum(u,4),
```

where

```text
powerTailSum(x,k) = x + x^2 + ... + x^(k+1).
```

The paper proves that for every integer `m >= 13`,

```text
sum_{k >= m} C_k(t) <= t - min(3m + 1, 2m + 21, 83).
```

## Verify the certificate

On a system with a C++17 compiler:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic verify_tail_certificate.cpp -o verify_tail_certificate
./verify_tail_certificate certificate.csv
```

Expected final line:

```text
ACCEPT threshold83 tail certificate
```

The verifier uses exact integer arithmetic only. It recomputes the handoff inequalities, all relevant primes, successor primes, power-tail sums, cutoffs, tail sums, and margins. It does not use floating-point arithmetic or exploratory search code.

To verify file integrity:

```bash
sha256sum -c SHA256SUMS
```

## Licensing

This is a mixed-license archival artifact. The verifier source code is
MIT-licensed, the certificate/checksum/metadata files are released under CC0
1.0 Universal to the extent permitted by law, and the paper source/PDF are
copyright Wesley Atwell and are not MIT-licensed. See `LICENSE.md` for the
file-level license map. If a platform asks for one record-level license, use
"Other (Open)" or "Mixed licenses; see LICENSE.md" rather than MIT.

## Archival release

Current release tag:

```text
v1.0.1
```

Release title:

```text
v1.0.1: Split-license metadata clarification
```

The release archive is configured for Zenodo metadata import through `.zenodo.json`.
