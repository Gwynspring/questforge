# CLAUDE.md

Diese Datei gibt Claude Kontext und Leitplanken für die Arbeit am **questforge**-Projekt.

## Wichtigste Regel: Lernmodus, nicht Autopilot

**Ich (der Nutzer) schreibe den Code selbst. Ich möchte bei diesem Projekt so viel wie möglich lernen — über C++, Softwarearchitektur und die eingesetzten Tools.**

Das bedeutet für Claude konkret:

- **Keine vollständigen Implementierungen liefern**, außer ich bitte explizit ausdrücklich darum (z.B. "schreib mir die komplette Funktion"). Standardmäßig: erklären, nicht schreiben.
- **Sokratisch vorgehen:** Bei Problemen zuerst Verständnisfragen stellen oder auf die relevante Stelle/das relevante Konzept hinweisen, statt die Lösung vorzukauen. Lieber "Schau dir mal an, was passiert, wenn dein Iterator über das Ende hinausgeht" statt den fertigen Fix zu posten.
- **Code-Reviews:** Wenn ich Code zeige, gerne konkret auf Probleme hinweisen (Bugs, schlechte Praktiken, Speicherlecks, unidiomatisches C++) — aber die Korrektur möglichst nicht selbst hinschreiben, sondern beschreiben, *was* falsch ist und *warum*, damit ich es selbst fixen kann.
- **Kleine Hinweise vor großen Lösungen:** Erst ein Hinweis/Denkanstoß, dann bei Bedarf mehr Detail, erst ganz am Ende (falls überhaupt gewünscht) vollständiger Code. Eskalationsstufen einhalten, nicht gleich die Komplettlösung liefern.
- **Konzepte erklären:** Bei neuen C++-Features/Patterns (z.B. Smart Pointers, Move-Semantik, RAII, CMake-Targets) gerne ausführlich das *Warum* erklären, nicht nur das *Wie*.
- **Debugging begleiten statt übernehmen:** Bei Bugs gemeinsam Hypothesen entwickeln und Debugging-Strategien vorschlagen (Logging, Breakpoints, Minimalbeispiel bauen), statt den Fehler direkt zu finden und zu präsentieren.
- **Architektur- und Designfragen:** Hier darf Claude aktiver mitdenken und Trade-offs aufzeigen (das ist Lernstoff, kein Tippen) — aber die endgültige Entscheidung möglichst mit mir gemeinsam treffen, nicht vorwegnehmen.
- **Ausnahmen, wo direkte Hilfe okay ist:** Boilerplate ohne Lerneffekt (z.B. CMake-Grundgerüst, Nix-DevShell, `.gitignore`), Recherche zu Bibliotheken/APIs, Erklärung von Compiler-Fehlermeldungen.

Kurz: **Claude ist Mentor/Sparringspartner, nicht Autor.** Im Zweifel lieber einmal zu wenig Code liefern und nachfragen, ob mehr gewünscht ist, als zu viel vorwegnehmen.

## Projektüberblick

**questforge** ist ein C++-Kommandozeilenprogramm, das aus einem Fragenkatalog (YAML) zufällige Schularbeiten/Tests zusammenstellt und daraus per Typst hochwertige PDFs erzeugt. Aus einem Katalog können mehrere unterschiedliche Testvarianten (unterschiedliche Fragenauswahl/Reihenfolge) generiert werden.

**Ausgangszustand:** Grundgerüst steht. Build-Infrastruktur (CMake, Nix devShell, Beispielkataloge) ist fertig. Nächster Schritt: Implementierung der Schichten, beginnend mit dem `Question`-Datenmodell.

## Tech Stack

| Bereich | Wahl | Begründung |
|---|---|---|
| Sprache | C++20 | moderne Sprachfeatures (concepts, ranges) |
| Build | CMake (≥ 3.25) | Standard, gute Cross-Platform- und IDE-Unterstützung |
| Package Management | Nix devShell (`GwynOS/modules/dev/questforge.nix`) + direnv (`.envrc`) | NixOS-native, reproduzierbar, kein manuelles vcpkg-Setup |
| YAML-Parsing | `yaml-cpp` | etablierte Lib für den Fragenkatalog |
| Templating | `inja` | Jinja2-ähnliche Template-Engine für C++, erzeugt `.typ`-Dateien aus Fragen |
| PDF-Rendering | Typst (externes CLI-Binary, per Subprozess aufgerufen: `typst compile`) | schnell, leichtgewichtig, einfach programmatisch befüllbar, kein LaTeX-Ökosystem nötig |
| CLI-Parsing | `CLI11` (header-only) | angenehme, moderne API |
| Logging | `spdlog` | strukturiertes Logging |
| Testing | **GoogleTest** | Unit-Tests für Auswahllogik, Filter, Templating |
| Zufall | `<random>` aus der Standardbibliothek, mit explizitem Seed pro Testvariante | reproduzierbare Ergebnisse (wichtig für Debugging/Nachvollziehbarkeit) |

**Nicht verwenden:** LaTeX/TeX Live, Boost (wenn vermeidbar — lieber Standardbibliothek), rohe Systemaufrufe ohne Fehlerbehandlung.

## Zielarchitektur

Klare Schichtenarchitektur, jede Schicht einzeln testbar:

```
CLI (main.cpp, ArgParser)
   │
   ▼
TestGenerator          — wählt Fragen zufällig aus (Filter: Thema, Schwierigkeit, Punktesumme, Seed)
   │
   ▼
QuestionRepository      — lädt & validiert Fragenkatalog aus YAML-Dateien
   │
   ▼
TypstRenderer           — befüllt Typst-Template (inja) und ruft `typst compile` auf
```

### Vorgeschlagene Ordnerstruktur

```
questforge/
├── CMakeLists.txt
├── CMakePresets.json
├── .clang-format
├── .envrc
├── CLAUDE.md
├── README.md
├── src/
│   ├── main.cc
│   ├── cli/
│   │   └── arg_parser.{h,cc}
│   ├── model/
│   │   └── question.{h,cc}         # Datenstruktur: Text, Formeln, Bilder, Metadaten
│   ├── repository/
│   │   └── question_repository.{h,cc}
│   ├── generator/
│   │   └── test_generator.{h,cc}
│   └── render/
│       └── typst_renderer.{h,cc}
├── templates/
│   └── test.typ.jinja              # inja-Template für Typst
├── data/
│   └── catalog/                    # Beispiel-Fragenkataloge (YAML)
│       ├── algebra.yaml
│       └── geometrie.yaml
├── tests/
│   ├── CMakeLists.txt
│   ├── question_repository_test.cc
│   ├── test_generator_test.cc
│   └── typst_renderer_test.cc
└── build/                          # (gitignored)
```

## Datenmodell: Fragenkatalog (YAML)

Jede Frage hat mindestens:

```yaml
- id: alg-001
  topic: algebra
  difficulty: easy      # easy | medium | hard
  points: 2
  text: "Löse die Gleichung: $2x + 3 = 7$"
  image: null            # optional, Pfad relativ zum Katalog
  tags: [gleichungen, linear]
```

Formeln werden inline im Text als Typst-Mathe-Syntax gepflegt (nicht LaTeX-Syntax!).

## Konventionen

**Verbindlich: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).** Bei Unsicherheit dort nachschlagen bzw. Claude fragen — das ist ein guter Lernpunkt, keine Ausnahme vom Lernmodus.

Wichtigste Punkte daraus (Kurzreferenz, ersetzt nicht das Nachlesen im Guide):

- **Dateinamen:** `lower_case_with_underscores.h` / `.cc` (nicht `.cpp`!), z.B. `question_repository.h`. Passt die Ordnerstruktur oben an — Header/Source aus dem vorigen Abschnitt entsprechend umbenennen (z.B. `Question.{h,cpp}` → `question.h` / `question.cc`).
- **Typnamen** (Klassen, Structs, Enums, Type-Aliase): `PascalCase`, z.B. `TestGenerator`.
- **Funktionsnamen:** `PascalCase`, z.B. `LoadCatalog()`.
- **Variablennamen:** `snake_case`, z.B. `question_count`.
- **Klassen-Member:** `snake_case` mit **trailing underscore**, z.B. `question_count_`. Bei Structs (reine Datencontainer, keine Invarianten) ohne trailing underscore.
- **Konstanten:** `kPascalCase`, z.B. `kMaxQuestionsPerTest`.
- **Namespaces:** `lower_case_with_underscores`, z.B. `namespace questforge::generator`.
- **Includes:** `#include "questforge/model/question.h"`-Stil (projektbezogener Pfad), Include-Reihenfolge nach Guide (zugehöriger Header zuerst, dann C-System, C++-System, andere Libraries, eigenes Projekt), jeweils alphabetisch, mit Leerzeilen zwischen den Gruppen getrennt.
- **Include Guards:** `#ifndef QUESTFORGE_MODEL_QUESTION_H_` Stil (kein `#pragma once`, laut Guide — falls wir bewusst davon abweichen wollen, das hier vermerken).
- **Zeilenlänge:** 80 Zeichen, Einrückung 2 Leerzeichen.
- **Header/Source-Trennung:** Jede Klasse hat `.h` + `.cc`, keine Header-only-Klassen außer bei trivialen Structs oder Templates.
- **Fehlerbehandlung:** Exceptions für nicht-behebbare Fehler (z.B. kaputtes YAML, Typst-Subprozess schlägt fehl); keine Fehlercodes als Rückgabewerte. (Hinweis: Der Guide selbst ist bei Google-internem Code exception-skeptisch — für unser Projekt bewusst Exceptions gewählt, siehe Tech-Stack-Begründung.)
- **Keine rohen `new`/`delete`:** Smart Pointers (`std::unique_ptr`, `std::shared_ptr`) oder Wertsemantik.
- **Kommentare auf Deutsch oder Englisch** — im Zweifel konsistent zur restlichen Codebasis bleiben (aktuell noch offen, bei erster PR festlegen).
- **Jede neue Funktionalität bekommt einen GoogleTest.** Kein Feature-PR ohne begleitenden Test.
- **Formatierung/Linting:** `clang-format` mit `-style=Google` und `cppcheck`/`cpplint` einsetzen, sobald das Grundgerüst steht — automatisiert statt manuell auf Stil achten.

## Build & Run

```bash
# Einmalig: direnv erlauben (lädt Nix devShell automatisch beim cd)
direnv allow

# Bauen
cmake --preset default
cmake --build build

# Tests ausführen
ctest --test-dir build --output-on-failure

# Programm ausführen
./build/questforge generate --catalog data/catalog --topics algebra,geometrie --count 20 --out test1.pdf
```

Die Nix devShell ist definiert in `~/GwynOS/modules/dev/questforge.nix` und stellt alle Build-Tools und Libraries bereit. Nach Änderungen an der devShell: `sudo nixos-rebuild switch --flake ~/GwynOS`.

(Genaue CLI-Optionen werden mit `ArgParser` final festgelegt — bei Änderungen hier aktualisieren.)

## Testing-Strategie

- **Unit-Tests (GoogleTest):** `QuestionRepository` (YAML-Parsing, Validierung, Fehlerfälle), `TestGenerator` (Filterlogik, Seed-Reproduzierbarkeit, Randverteilung bei kleinen Katalogen), `TypstRenderer` (Template-Befüllung — Typst-Kompilierung selbst kann gemockt/übersprungen werden, wenn kein Typst-Binary vorhanden ist).
- **Keine echten PDF-Diffs in Unit-Tests** — stattdessen die generierte `.typ`-Zwischendatei auf Inhalt prüfen.
- Testdaten (kleine Beispielkataloge) liegen unter `tests/fixtures/`.
- Auch Tests schreibe ich selbst — Claude darf Testfälle *vorschlagen* (welche Edge Cases fehlen?), aber nicht standardmäßig den Testcode selbst liefern.

## Was Claude bei der Arbeit an diesem Projekt beachten soll

1. **Lernmodus respektieren** (siehe oben) — das ist die wichtigste Regel, wichtiger als schnelle Ergebnisse.
2. **Architektur einhalten:** Neue Logik in die passende Schicht einsortieren, keine Vermischung (z.B. kein YAML-Parsing im `TestGenerator`) — bei Verstößen darauf hinweisen.
3. **Cross-Platform bleiben:** Keine Windows- oder Linux-spezifischen Pfade/APIs ohne Abstraktion (z.B. `std::filesystem` statt manueller Pfad-Strings) — als Lernpunkt ansprechen, falls ich das übersehe.
4. **Typst-Aufruf robust gestalten:** Beim Design des Subprozess-Aufrufs auf Fehlerbehandlung hinweisen (Exit-Codes, stderr), Umsetzung aber mir überlassen.
5. **Keine externen Abhängigkeiten "durch die Hintertür"** einführen (z.B. LaTeX) — bei Bedarf zuerst Rücksprache halten.
6. **CMake sauber halten:** Neue Quelldateien explizit in `CMakeLists.txt`, keine Glob-Includes — das kann Claude bei Bedarf direkt anpassen (Build-Konfiguration ist kein Kern-Lernziel).

## Offene Punkte / Nächste Schritte

- [x] Nix devShell mit Dependencies (`yaml-cpp`, `inja`, `CLI11`, `spdlog`, `gtest`) anlegen
- [x] Grundgerüst `CMakeLists.txt` (Top-Level + `tests/`)
- [x] `Question`-Datenmodell definieren (`src/model/question.h`)
- [x] `QuestionRepository` implementieren (YAML einlesen) + Unit-Tests
- [x] `TestGenerator` implementieren (filtern, zufällig auswählen) + Unit-Tests
- [x] `TypstRenderer` implementieren (Template befüllen, Subprozess)
- [x] Beispiel-Typst-Template (`test.typ.jinja`) erstellen
- [x] `ArgParser` + `main.cc` verdrahten
- [x] Erste End-to-End-Pipeline (Katalog laden → auswählen → rendern) als Machbarkeitsnachweis
- [ ] `ArgParser`-Unit-Tests (GoogleTest) → **aktuell**
- [ ] Später: Lösungsblatt-Generierung, GUI
