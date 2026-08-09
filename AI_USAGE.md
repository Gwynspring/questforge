# AI usage in this project

This project is developed with the help of opencode. This document is a
transparent, short summary of how AI is and isn't used here.

## What AI is used for

- **Learning support:** Socratic guidance on problems — pointers toward
  the relevant concept or bug, rather than the finished answer.
- **Concept explanations:** the *why* behind C++ features and patterns
  (smart pointers, move semantics, RAII), CMake, and architecture
  trade-offs.
- **Code review:** pointing out bugs, unidiomatic C++, or design issues in
  code — described in words, not rewritten.
- **Debugging strategy:** developing hypotheses and suggesting debugging
  approaches (logging, breakpoints, minimal repros) alongside the author.
- **Non-learning boilerplate and research:** scaffolding with no learning
  value (Nix devShell, CMake setup, `.gitignore`), library/API research,
  and explaining compiler error messages.

## What AI does not do

- Write core application logic, features, or bug fixes on the author's
  behalf.
- Write test code by default.
- Make final architecture or design decisions unilaterally.

All source code and tests in this repository are written by the project's
author.

## How this is enforced

opencode reads project-specific instructions from [`AGENTS.md`](AGENTS.md)
in this repository on every session, which encodes the rules above in
detail. The dedicated `mentor` and `code-reviewer` agents in
`.opencode/agents/` reinforce the same rules (no unrequested code, no
unrequested edits).
