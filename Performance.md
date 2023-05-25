# Performance

## Items Per Second

| SHA Algorithm | Non-vectorized | Vectorized | Instrinsic |
| :-----------: | :------------: | :--------: | :--------: |
|    SHA256     |   1.358 M/s    | 1.085 M/s  | 5.633 M/s  |
|    SHA224     |   1.399 M/s    |            | 5.733 M/s  |

## Bytes Per Second

| SHA Algorithm | Non-vectorized | Vectorized | Intrinsic  |
| :-----------: | :------------: | :--------: | :--------: |
|    SHA256     |  ~20.732 T/s   | 16.567 T/s | 85.947 T/s |
|    SHA224     |  ~21.361 T/s   |            | 87.483 T/s |

SHA intrinsic implementation is ~4x faster than the non-vectorized version.

Need to further optimize vectorized version of SHA256/224.