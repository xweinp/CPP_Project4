Overview
--------
This is an exploratory C++20 metaprogramming project (templates, concepts, constexpr tricks). It is not a polished library.

Important Notes
---------------
* Complex metaprogramming: Many constructs push compilers in edge‑case directions; compile-time calculations were the primary goal.
* Compiler sensitivity: Successful compilation depends heavily on the compiler (version, vendor, standard library). The provided `Makefile` defaults to `clang++`; other compilers (or older versions) may fail.
* Partial functionality: Not everything works. We haven't debugged or completed all ideas. 
* Test code provenance: Portions of test code were likely sourced or adapted from university materials, Discord discussions, or communal snippets. Exact original authorship is uncertain.
* No reuse permitted: Do not copy, redistribute, or incorporate any code from this repository into other projects. Treat all code as non‑licensable and for local study only.
* Whole code is in `.h` file -- it needs to be so that all calculations happen at compile time.