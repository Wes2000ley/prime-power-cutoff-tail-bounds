# v1.0.1: Split-license metadata clarification

Maintenance release of the verification artifact for:

**Certified Tail Bounds for Prime-Power Threshold Cutoff Sequences**

This release keeps the threshold-83 certificate and verifier behavior unchanged
from v1.0.0. It clarifies the public archival metadata and file-level licensing:

- `LICENSE.md` now states explicitly that the MIT License applies only to
  `verify_tail_certificate.cpp`;
- the certificate, checksum, and metadata files are identified as CC0-1.0 to
  the extent permitted by law;
- the paper source and PDF are identified as copyright Wesley Atwell and not
  MIT-licensed;
- `CITATION.cff`, `.zenodo.json`, `README.md`, and the paper source were
  updated to point readers to the split-license map;
- SHA256 manifests were refreshed after rebuilding the PDF.

Expected verification command:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic verify_tail_certificate.cpp -o verify_tail_certificate
./verify_tail_certificate certificate.csv
```

Expected final line:

```text
ACCEPT threshold83 tail certificate
```
