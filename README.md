### Known hurdles and solutions:
- `send(fd, buf, **exact_num_of_bytes**, 0)` is known to cause an issue. To resolve this:
    1. Send header and body seperately
    2. While sending body, change `exact_num_of_bytes` to `exact_num_of_bytes + 1`
    - aka, claim you are sending more bytes than you actually plan to
