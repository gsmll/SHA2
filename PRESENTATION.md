# Everything SHA-2

## What is SHA-2

SHA-2 stands for **Secure Hashing Algorithm 2**.

SHA-2 is a family of hashing algorithms designed and published by the **National Security Agency (NSA)** in 2001. 

SHA-2's predecessor was **SHA-1**. It is discouraged to use SHA-1 as it is no longer considered secure. SHA-2 is meant to be its successor. 

SHA-2 consists of many SHA algorithms. These are formally designated by the NSA.

- SHA256
- SHA224
- SHA512
- SHA384
- SHA512/256
- SHA512/224

Some hashing algorithms use SHA-2 as part of its hashing. Notable ones are the crypt implementation of SHA256 and SHA512 used to hash passwords on linux. See [here](https://www.akkadia.org/drepper/SHA-crypt.txt) for more details and implementation. *(Its complicated!)*

SHA-2 has a lot of applications. Some are listed below:

- Secure shell protocol (SSH)
- Transport Layer Security (TLS) *Used in HTTPS and email among other things*
- Used within the US government

SHA-2 algorithms are still considered secure.

## Properties of a Hashing Algorithm

> A hash algorithm takes a variable length input and outputs a fixed length output.

There are some crucial properties that makes a hash function good:

- **Deterministic**: The same input always yields the same output.
- **Slight changes sigificantly changes output**
- **Collision resistant**: It is difficult to find two inputs that yield the same output.
- **One-way function**: You cannot determine the input from the output.

Collisions are unavoidable because there are an infinite inputs and finite outputs (**Pidgeonhole principle**). Hash algorithms with larger size of outputs will have lower change of a collision.

> There is an upper bound on collision resistance due to the **Birthday Paradox**. That upper bound is 2<sup>N/2</sup>.

