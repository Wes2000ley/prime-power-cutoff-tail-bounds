# v1.0.0: Threshold-83 tail certificate

Initial archival release of the verification artifact for:

**Certified Tail Bounds for Prime-Power Threshold Cutoff Sequences**

This release contains:

- the paper source and PDF;
- `certificate.csv`, the frozen threshold certificate containing the 19 handoff rows;
- `verify_tail_certificate.cpp`, a standalone deterministic C++17 exact-integer verifier;
- `BUILD_AND_VERIFY.txt`, with build and verification instructions;
- `SHA256SUMS`, with file integrity hashes.

Expected verification command:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic verify_tail_certificate.cpp -o verify_tail_certificate
./verify_tail_certificate certificate.csv
```

Expected final line:

```text
ACCEPT threshold83 tail certificate
```

The verifier uses no floating-point arithmetic and does not depend on exploratory search code.

License note: this is a mixed-license archival artifact. The verifier source
code is MIT-licensed, the certificate/checksum/metadata files are CC0-1.0 to
the extent permitted by law, and the paper source/PDF are copyright Wesley
Atwell and are not MIT-licensed. See `LICENSE.md`.
