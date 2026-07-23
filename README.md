# questforge

A C++20 command-line tool that assembles randomized tests and exams from a YAML question catalog and renders them as PDFs via [Typst](https://typst.app).

## What it does

Given a catalog of questions (with metadata like topic, difficulty, and points), questforge selects a random subset matching your criteria and produces a print-ready PDF. Multiple test variants with different question selections can be generated from the same catalog using explicit seeds for reproducibility.

## Tech stack

| Area | Choice |
|---|---|
| Language | C++20 |
| Build | CMake ≥ 3.25 + Ninja |
| Dev environment | Nix devShell + direnv |
| YAML parsing | yaml-cpp |
| Templating | inja (Jinja2-like) |
| PDF rendering | Typst (via subprocess) |
| CLI | CLI11 |
| Logging | spdlog |
| Testing | GoogleTest |

## Getting started

**Prerequisites:** CMake ≥ 3.25, Ninja, a C++20 compiler, and the libraries listed in the tech stack above (yaml-cpp, inja, CLI11, spdlog, GoogleTest). Install them via your package manager of choice (apt, brew, vcpkg, Nix, ...).

```bash
# Build
cmake --preset default
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Generate a test (once fully implemented)
./build/questforge generate \
  --catalog data/catalog \
  --topics algebra,geometry \
  --count 20 \
  --out exam.pdf
```

## Question catalog format

```yaml
questions:
  - id: alg-001
    topic: algebra
    difficulty: easy      # easy | medium | hard
    points: 2
    text: "Solve: $2x + 3 = 7$"
    image: null           # optional path relative to catalog
    tags: [equations, linear]
```

Math is written in Typst syntax (not LaTeX).

## Project status

Work in progress. Currently implemented: question data model, YAML catalog loading.
