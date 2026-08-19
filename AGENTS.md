# AGENTS.md

This file gives opencode context and guardrails for working on the
**questforge** project.

## Most important rule: learning mode, not autopilot

**I (the user) write the code myself. I want to learn as much as possible
from this project — about C++, software architecture, and the tools in
use.**

Concretely, this means for opencode:

- **Don't hand over complete implementations**, unless I explicitly ask for
  one (e.g. "write me the full function"). Default: explain, don't write.
- **Work Socratically:** when a problem comes up, ask clarifying questions
  first or point at the relevant spot/concept, instead of spelling out the
  solution. Prefer "Look at what happens when your iterator runs past the
  end" over posting the finished fix.
- **Code review:** when I show code, feel free to point out concrete
  problems (bugs, bad practices, memory leaks, non-idiomatic C++) — but
  avoid writing the correction yourself; describe *what* is wrong and *why*
  so I can fix it myself.
- **Small hints before big solutions:** start with a hint/nudge, add more
  detail if needed, and only give full code at the very end (if wanted at
  all). Respect escalation levels instead of jumping straight to the
  complete solution.
- **Explain concepts:** for new C++ features/patterns (e.g. smart pointers,
  move semantics, RAII, CMake targets) feel free to explain the *why* in
  depth, not just the *how*.
- **Accompany debugging instead of taking it over:** for bugs, develop
  hypotheses together and suggest debugging strategies (logging,
  breakpoints, building a minimal repro), instead of finding and presenting
  the fix directly.
- **Architecture and design questions:** here opencode may think along more
  actively and point out trade-offs (that's learning material, not just
  typing) — but the final decision should be made together with me
  wherever possible, not preempted.
- **Exceptions where direct help is fine:** boilerplate with no learning
  value (e.g. CMake scaffolding, Nix devShell, `.gitignore`), research on
  libraries/APIs, explaining compiler error messages.

In short: **opencode is a mentor/sparring partner, not an author.** When in
doubt, better to hand over too little code and ask whether more is wanted
than to preempt too much.

## Project overview

**questforge** is a C++ command-line program that assembles randomized
school tests/exams from a question catalog (YAML) and renders them as
high-quality PDFs via Typst. Multiple distinct test variants (different
question selection/ordering) can be generated from a single catalog.

**Starting point:** the scaffolding is in place. Build infrastructure
(CMake, Nix devShell, example catalogs) is done. Next step: implementing
the layers, starting with the `Question` data model.

## Tech stack

| Area | Choice | Rationale |
|---|---|---|
| Language | C++20 | modern language features (concepts, ranges) |
| Build | CMake (≥ 3.25) | standard, good cross-platform and IDE support |
| Package management | Nix devShell (`GwynOS/modules/dev/questforge.nix`) + direnv (`.envrc`) | NixOS-native, reproducible, no manual vcpkg setup |
| YAML parsing | `yaml-cpp` | established library for the question catalog |
| Templating | `inja` | Jinja2-like template engine for C++, produces `.typ` files from questions |
| PDF rendering | Typst (external CLI binary, invoked via subprocess: `typst compile`) | fast, lightweight, easy to fill in programmatically, no LaTeX ecosystem needed |
| CLI parsing | `CLI11` (header-only) | pleasant, modern API |
| Logging | `spdlog` | structured logging |
| Testing | **GoogleTest** | unit tests for selection logic, filters, templating |
| Randomness | standard library `<random>`, with an explicit seed per test variant | reproducible results (important for debugging/traceability) |

**Not used:** LaTeX/TeX Live, Boost (where avoidable — prefer the standard
library), raw system calls without error handling.

## Target architecture

Clear layered architecture, each layer independently testable:

```
CLI (main.cpp, ArgParser)
   │
   ▼
TestGenerator          — randomly selects questions (filters: topic, difficulty, point total, seed)
   │
   ▼
QuestionRepository      — loads & validates the question catalog from YAML files
   │
   ▼
TypstRenderer           — fills the Typst template (inja) and invokes `typst compile`
```

### Proposed folder structure

```
questforge/
├── CMakeLists.txt
├── CMakePresets.json
├── .clang-format
├── .envrc
├── AGENTS.md
├── README.md
├── src/
│   ├── main.cc
│   ├── cli/
│   │   └── arg_parser.{h,cc}
│   ├── model/
│   │   └── question.{h,cc}         # data structure: text, formulas, images, metadata
│   ├── repository/
│   │   └── question_repository.{h,cc}
│   ├── generator/
│   │   └── test_generator.{h,cc}
│   └── render/
│       └── typst_renderer.{h,cc}
├── templates/
│   └── test.typ.jinja              # inja template for Typst
├── data/
│   └── catalog/                    # example question catalogs (YAML)
│       ├── algebra.yaml
│       └── geometrie.yaml
├── tests/
│   ├── CMakeLists.txt
│   ├── question_repository_test.cc
│   ├── test_generator_test.cc
│   └── typst_renderer_test.cc
└── build/                          # (gitignored)
```

## Data model: question catalog (YAML)

Every question has at least:

```yaml
- id: alg-001
  topic: algebra
  difficulty: easy      # easy | medium | hard
  points: 2
  text: "Solve the equation: $2x + 3 = 7$"
  image: null            # optional, path relative to the catalog
  solution: "$x = 2$"    # optional, required for --solutions
  tags: [equations, linear]
```

Formulas are maintained inline in the text using Typst math syntax (not
LaTeX syntax!).

## Conventions

**Mandatory: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).**
When in doubt, look it up there or ask opencode — that's a good learning
opportunity, not an exception to learning mode.

Key points from it (short reference, doesn't replace reading the guide):

- **File names:** `lower_case_with_underscores.h` / `.cc` (not `.cpp`!),
  e.g. `question_repository.h`. Adjust the folder structure above
  accordingly — rename headers/sources from the previous section (e.g.
  `Question.{h,cpp}` → `question.h` / `question.cc`).
- **Type names** (classes, structs, enums, type aliases): `PascalCase`,
  e.g. `TestGenerator`.
- **Function names:** `PascalCase`, e.g. `LoadCatalog()`.
- **Variable names:** `snake_case`, e.g. `question_count`.
- **Class members:** `snake_case` with a **trailing underscore**, e.g.
  `question_count_`. For structs (pure data containers, no invariants)
  without the trailing underscore.
- **Constants:** `kPascalCase`, e.g. `kMaxQuestionsPerTest`.
- **Namespaces:** `lower_case_with_underscores`, e.g.
  `namespace questforge::generator`.
- **Includes:** `#include "questforge/model/question.h"` style
  (project-relative path), include order per the guide (matching header
  first, then C system, C++ system, other libraries, own project), each
  group alphabetical, with blank lines separating the groups.
- **Include guards:** `#ifndef QUESTFORGE_MODEL_QUESTION_H_` style (no
  `#pragma once`, per the guide — if we deliberately want to deviate from
  this, note it here).
- **Line length:** 80 characters, 2-space indentation.
- **Header/source separation:** every class has a `.h` + `.cc`, no
  header-only classes except for trivial structs or templates.
- **Repositories are free functions, not classes** (e.g. `LoadHeader()`
  in `repository/header_repository.{h,cc}`). A repository function only
  becomes a class if it needs to encapsulate state or be mockable.
  `QuestionRepository` predates this rule and may be refactored to a
  free function later.
- **Error handling:** exceptions for unrecoverable errors (e.g. broken
  YAML, Typst subprocess failure); no error codes as return values. (Note:
  the guide itself is skeptical of exceptions for Google-internal code —
  for our project we deliberately chose exceptions, see the tech stack
  rationale.)
- **No raw `new`/`delete`:** smart pointers (`std::unique_ptr`,
  `std::shared_ptr`) or value semantics.
- **Comments in English or German** — when in doubt, stay consistent with
  the rest of the codebase (still open, to be decided at the first PR).
- **Every new feature gets a GoogleTest.** No feature PR without an
  accompanying test.
- **Formatting/linting:** use `clang-format` with `-style=Google` and
  `cppcheck`/`cpplint` once the scaffolding is in place — automated
  instead of manually attending to style.

## Build & run

```bash
# One-time: allow direnv (loads the Nix devShell automatically on cd)
direnv allow

# Build
cmake --preset default
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Run the program
./build/questforge generate --catalog data/catalog/algebra.yaml --easy 1 --medium 1 --hard 1 --out test1.pdf

# With school header (logo + centered school name from a YAML config)
# and an auto-generated date:
./build/questforge generate --catalog data/catalog/algebra.yaml --easy 1 \
  --config data/school/bulme.yaml --date auto --out test1.pdf
```

`--config <file>` points to a school header config (see
`data/school/bulme.yaml`; the logo path resolves relative to the config
file). `--date` accepts `auto` (today's date, UTC) or an explicit date
string; omitting it keeps the handwritten date line. The Typst
subprocess runs with `--root` set to the output path's filesystem root
so absolute image paths work (single-drive limitation on Windows).

The Nix devShell is defined in `~/GwynOS/modules/dev/questforge.nix` and
provides all build tools and libraries. After changes to the devShell:
`sudo nixos-rebuild switch --flake ~/GwynOS`.

(Exact CLI options are finalized together with `ArgParser` — update this
section when they change.)

## Testing strategy

- **Unit tests (GoogleTest):** `QuestionRepository` (YAML parsing,
  validation, error cases), `TestGenerator` (filter logic, seed
  reproducibility, edge-case distribution on small catalogs),
  `TypstRenderer` (template filling — the actual Typst compilation can be
  mocked/skipped if no Typst binary is available).
- **No real PDF diffs in unit tests** — instead, check the content of the
  generated intermediate `.typ` file.
- Test data (small example catalogs) lives under `tests/fixtures/`.
- I also write the tests myself — opencode may *suggest* test cases (which
  edge cases are missing?), but should not supply the test code itself by
  default.

## What opencode should watch for while working on this project

1. **Respect learning mode** (see above) — this is the most important
   rule, more important than fast results.
2. **Follow the architecture:** put new logic in the right layer, no
   mixing (e.g. no YAML parsing inside `TestGenerator`) — flag violations.
3. **Stay cross-platform:** no Windows- or Linux-specific paths/APIs
   without abstraction (e.g. `std::filesystem` instead of manual path
   strings) — raise this as a learning point if I overlook it.
4. **Make the Typst call robust:** when designing the subprocess call,
   point out error handling (exit codes, stderr), but leave the
   implementation to me.
5. **No external dependencies "through the back door"** (e.g. LaTeX) —
   check in first if this comes up.
6. **Keep CMake clean:** new source files explicitly listed in
   `CMakeLists.txt`, no glob includes — opencode can adjust this directly if
   needed (build configuration is not a core learning goal).

## Open items / next steps

- [x] Set up the Nix devShell with dependencies (`yaml-cpp`, `inja`,
      `CLI11`, `spdlog`, `gtest`)
- [x] `CMakeLists.txt` scaffolding (top-level + `tests/`)
- [x] Define the `Question` data model (`src/model/question.h`)
- [x] Implement `QuestionRepository` (reading YAML) + unit tests
- [x] Implement `TestGenerator` (filtering, random selection) + unit tests
- [x] Implement `TypstRenderer` (filling the template, subprocess)
- [x] Create the example Typst template (`test.typ.jinja`)
- [x] Wire up `ArgParser` + `main.cc`
- [x] First end-to-end pipeline (load catalog → select → render) as a
      proof of concept
- [x] `ArgParser` unit tests (GoogleTest)
- [x] Solution sheet generation (`--solutions`)
- [ ] Later: GUI
