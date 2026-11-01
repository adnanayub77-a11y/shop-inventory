# Two AI tools used on this assignment

The brief asks for two AI tools. This project used both as follows.

## Tool 1 — Cursor (Grok)

**Role:** wrote the C++ source, preprocessor header, Makefile, sample shop data, and presentation notes. Compiled with MinGW GCC, then replayed every menu option to confirm a clean `return 0`.

**Prompts used (summary):**
- Build Portfolio Project #2 in C++ with a user interface, preprocessor directives, and a compiler from the approved list.
- Keep MinGW GCC as the compiler this machine can run, and keep a Clang target in the Makefile.
- Include a practical shop: products, stock, billing, receipts, search, and reports.
- The user must be able to exhaust every menu item and leave with `return 0`.

## Tool 2 — Claude

**Role:** read the source and reviewed the programme against the brief.

**What stayed after that review:**
- Prices such as `inf` are rejected.
- A damaged product line is reported instead of being skipped silently.
- The programme prints the `data` folder path at startup.
- The console menu, the preprocessor directives, and `return 0` on exit were confirmed.

## Credit

Original programme written for this assignment. No third-party source was copied.
