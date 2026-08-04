# questforge — Code Review

**Datum:** 2026-08-04
**Umfang:** `src/`, `tests/`, `templates/`, `CMakeLists.txt`, Projektstruktur
**Methode:** Vollständige Lektüre aller `.h`/`.cc`-Dateien, `clang-format --dry-run` gegen alle Quelldateien, gezielte Laufzeit-Reproduktion zweier vermuteter Bugs gegen die gebaute Binary.

## Zusammenfassung

Der Code ist insgesamt sauber, idiomatisch und hält sich eng an den Google Style Guide — Formatierung, Namenskonvention und Schichtentrennung sind vorbildlich umgesetzt. Es gibt keine Speicherlecks, keine rohen `new`/`delete`, kein `system()`-Aufruf (stattdessen korrekt `fork`/`execvp`, was Shell-Injection strukturell ausschließt). Die gefundenen Probleme sind überwiegend Härtungslücken (fehlende Validierung an Rändern) und kleinere Konsistenzabweichungen — keine Architekturbrüche.

| # | Finding | Dringlichkeit |
|---|---|:---:|
| 1 | Katalog ohne `questions`-Root-Key wird stillschweigend als leerer Katalog akzeptiert | **6** |
| 2 | Negative `--easy/--medium/--hard`-Werte erzeugen irreführende Fehlermeldung (signed→unsigned Cast) | **6** |
| 3 | stderr des `typst`-Subprozesses geht bei Fehlern verloren, nicht in Exception-Message übernommen | **4** |
| 4 | Include-Reihenfolge verletzt in 4 Dateien die selbstdefinierte Konvention (3rd-Party-Header vermischt mit C++-Stdlib) | **4** |
| 5 | Uneinheitliches Exception-Wrapping in `QuestionRepository::LoadCatalog` | **3** |
| 6 | Keine Kollisionsprüfung zwischen `.typ`-Zwischendatei und `--out`-Pfad | **3** |
| 7 | `TypstRenderer`-Konstruktoren inline im Header statt in `.cc` | **2** |
| 8 | `ValidateQuestion` prüft `topic` nicht auf Leerheit | **2** |
| 9 | `CLAUDE.md`-Abschnitt „Build & Run" nicht mehr synchron mit tatsächlicher CLI (README ist korrekt) | **1** |

---

## 1. Katalog ohne `questions`-Root-Key wird stillschweigend akzeptiert (Dringlichkeit: 6)

**Datei:** `src/repository/question_repository.cc:42-43`

```cpp
YAML::Node root = YAML::LoadFile(path.string());
for (const auto& entry : root["questions"]) {
```

**Verifiziert:** Eine YAML-Datei ohne `questions`-Key (z. B. Tippfehler `Questions:` oder komplett falsches Schema) liefert von `root["questions"]` einen `Undefined`-Node. Die Iteration darüber läuft 0-mal durch — kein Fehler, keine Exception. `LoadCatalog` gibt einfach einen leeren `vector` zurück.

```
$ ./questforge generate --catalog no_questions_key.yaml --easy 0 --medium 0 --hard 0 --out out.pdf
$ echo $?
0
```

Das Programm läuft durch und erzeugt ein leeres PDF, statt den Nutzer auf einen kaputten Katalog hinzuweisen. Erst wenn `--easy/--medium/--hard` > 0 gesetzt sind, fällt der Fehler zufällig in `TestGenerator::TakeN` auf ("not enough questions available") — mit einer Meldung, die nichts über die eigentliche Ursache (fehlender Root-Key) verrät.

**Vorschlag:** Vor der Schleife explizit prüfen, ob `root["questions"]` existiert und `IsSequence()` ist; sonst eine aussagekräftige Exception werfen. Das gehört fachlich klar in die Repository-Schicht (Katalog-Validierung), passt also zur bestehenden Architektur — nur eine Ergänzung zu `ValidateQuestion`, keine neue Schicht.

---

## 2. Negative Counts erzeugen irreführende Fehlermeldung (Dringlichkeit: 6)

**Datei:** `src/generator/test_generator.cc:15`

```cpp
std::vector<model::Question> TestGenerator::TakeN(
    const std::vector<model::Question>& bucket, int count,
    model::Difficulty difficulty) {
  if (bucket.size() < static_cast<size_t>(count)) {
```

`count` kommt letztlich unvalidiert aus `CliOptions::easy_count` etc. (`src/cli/arg_parser.cc` — kein `->check(...)` auf den Optionen). Bei negativem `count` wird `static_cast<size_t>(count)` zu einer riesigen Zahl (Integer-Wraparound), wodurch die Bedingung praktisch immer `true` ist.

**Verifiziert:**
```
$ ./questforge generate --catalog data/catalog/algebra.yaml --easy -1 --out out.pdf
not enough easy questions available. Available easy questions: 5
```
5 Fragen sind reichlich vorhanden — die eigentliche Ursache (negativer, unsinniger Wert) wird durch eine falsche Diagnose verschleiert. Kein Crash/UB (weil `copy_n` wegen des vorherigen `throw` nie mit negativem `count` erreicht wird), aber ein handfester UX-/Robustheits-Bug.

**Vorschlag:** Zwei mögliche Stellen zur Behebung — Diskussionswert für die Architektur:
- CLI-seitig mit `CLI::NonNegativeNumber` validieren (`gen->add_option(...)​->check(CLI::NonNegativeNumber)`), scheitert früh mit klarer `CLI::ParseError`.
- Oder in `TakeN`/`Generate` selbst defensiv prüfen, falls `FilterCriteria` auch außerhalb der CLI erzeugt werden könnte (z. B. später von Tests oder einer GUI).

Vermutlich beides sinnvoll fürs Lernen: CLI11-Validatoren sind ein gutes Konzept, das noch nicht genutzt wird.

---

## 3. stderr des `typst`-Subprozesses geht verloren (Dringlichkeit: 4)

**Datei:** `src/platform/process_runner.cc:31-45`

```cpp
if (WIFEXITED(status)) {
  int exit_code = WEXITSTATUS(status);
  switch (exit_code) {
    case 0:
      break;
    case 127:
      throw std::runtime_error("typst not found - is it installed?");
    default:
      throw std::runtime_error(
          std::format("failed with code {}", exit_code));
  }
}
```

Exit-Codes und Signale werden vorbildlich unterschieden (das war explizit als Lernziel in `CLAUDE.md` genannt — hier gut umgesetzt). Was fehlt: `typst compile` schreibt seine eigentliche Fehlermeldung (z. B. Syntaxfehler im generierten `.typ`, fehlendes Bild) nach stderr, das aktuell einfach an das Terminal des Elternprozesses durchgereicht wird (kein Pipe/Redirect). Die geworfene Exception (`"failed with code 1"`) enthält diese Information nicht — der Nutzer muss den Output oberhalb der eigenen Fehlermeldung im Terminal suchen.

**Vorschlag:** Stdout/stderr des Kindprozesses per `pipe()` + `dup2()` einfangen und den eingefangenen Text der Exception-Message anhängen. Guter, in sich abgeschlossener nächster Lernschritt zu POSIX-Prozess-APIs (`pipe`, `dup2`), passt auch zur in `CLAUDE.md` Punkt 4 explizit genannten Erwartung an robuste Subprozess-Fehlerbehandlung.

---

## 4. Include-Reihenfolge verletzt eigene Konvention (Dringlichkeit: 4)

`CLAUDE.md` legt fest: *"Include-Reihenfolge nach Guide (zugehöriger Header zuerst, dann C-System, C++-System, andere Libraries, eigenes Projekt)"*. In 4 Dateien werden 3rd-Party-Bibliotheks-Header alphabetisch in die C++-Standardheader-Gruppe gemischt, statt eine eigene Gruppe danach zu bilden:

```cpp
// src/repository/question_repository.cc
#include "repository/question_repository.h"

#include <yaml-cpp/yaml.h>     // ← Library-Header, sollte eigene Gruppe sein

#include <format>
#include <stdexcept>
#include <string>
#include <unordered_set>
```

```cpp
// src/renderer/typst_renderer.cc
#include <format>
#include <fstream>
#include <inja/inja.hpp>       // ← mitten in der C++-Stdlib-Gruppe
#include <stdexcept>
```

```cpp
// src/cli/arg_parser.h
#include <CLI/CLI.hpp>          // ← sollte nach den Stdlib-Headern stehen
#include <cstdint>
#include <filesystem>
...
```

```cpp
// src/main.cc
#include <CLI/App.hpp>          // ← gleiches Muster
#include <cstdlib>
#include <iostream>
#include <vector>
```

Als **positives Gegenbeispiel** — korrekt nach der eigenen Regel gruppiert — dient `src/platform/process_runner.cc`:
```cpp
#include "platform/process_runner.h"

#include <sys/wait.h>      // C-System
#include <unistd.h>

#include <cerrno>          // C++-System
#include <cstdio>
#include <format>
#include <stdexcept>
#include <system_error>
```

**Vorschlag:** In den 4 betroffenen Dateien die Library-Header (`yaml-cpp`, `inja`, `CLI11`) in eine eigene Gruppe nach den C++-Standardheadern verschieben — rein mechanisch, guter Kandidat für eine spätere `cpplint`-Automatisierung (ohnehin für später vorgesehen laut `CLAUDE.md`).

---

## 5. Uneinheitliches Exception-Wrapping in `QuestionRepository` (Dringlichkeit: 3)

**Datei:** `src/repository/question_repository.cc:41,74-77`

```cpp
try {
  YAML::Node root = YAML::LoadFile(path.string());
  ...
  ValidateQuestion(q);   // wirft ggf. std::invalid_argument — NICHT gefangen
  ...
} catch (const YAML::Exception& e) {
  throw std::runtime_error("Failed to load catalog: " + std::string(e.what()));
}
```

Der `catch`-Block fängt ausschließlich `YAML::Exception` (kaputtes YAML, falscher Typ bei `.as<...>()`). Semantische Validierungsfehler (`ValidateQuestion`, `StringToDifficulty`) werfen dagegen `std::invalid_argument` **ohne** den `"Failed to load catalog: "`-Kontext-Präfix und ohne Umwandlung in `runtime_error`. Die Tests bestätigen, dass dies **beabsichtigt** ist (unterschiedliche Exception-Typen für "kaputtes YAML" vs. "semantisch ungültige Frage") — das ist an sich ein legitimes Design, aber nirgends dokumentiert. Für `main.cc` macht es aktuell keinen Unterschied (beide werden über `catch (const std::exception&)` gefangen), aber ein späterer Aufrufer, der gezielt nach Exception-Typ unterscheiden will, muss dieses Verhalten aus dem Code erschließen statt aus einem Kommentar.

**Vorschlag:** Kurzer Kommentar an der `catch`-Klausel, der die bewusste Trennung "Parse-Fehler → `runtime_error`, Validierungs-Fehler → `invalid_argument`" festhält — oder, falls das nicht beabsichtigt war, beide Pfade vereinheitlichen.

---

## 6. Keine Kollisionsprüfung `.typ`-Zwischendatei vs. `--out` (Dringlichkeit: 3)

**Datei:** `src/renderer/typst_renderer.cc:39-40, 56-59`

```cpp
auto typ_path = output_path;
typ_path.replace_extension(".typ");
...
std::vector<std::string> args{"typst", "compile", typ_path.string(),
                              output_path.string()};
```

Wird `--out` versehentlich mit `.typ`-Endung angegeben (z. B. `--out test.typ`), ist `typ_path == output_path`: Die generierte Zwischendatei und das `typst compile`-Ausgabeziel sind identisch. Kein Crash, aber ein verwirrender Edge Case (Quelle und Ziel des Compile-Aufrufs sind dieselbe Datei).

**Vorschlag:** Beim CLI-Parsing (`ArgParser`) oder zu Beginn von `Render` prüfen, dass `output_path.extension() != ".typ"`, und andernfalls eine klare Fehlermeldung werfen.

---

## 7. `TypstRenderer`-Konstruktoren inline im Header (Dringlichkeit: 2)

**Datei:** `src/renderer/typst_renderer.h:15-21`

```cpp
class TypstRenderer {
 public:
  TypstRenderer(std::filesystem::path template_path,
                std::unique_ptr<platform::ProcessRunner> runner)
      : template_path_(std::move(template_path)), runner_(std::move(runner)) {}

  explicit TypstRenderer(std::filesystem::path template_path)
      : TypstRenderer(std::move(template_path),
                      std::make_unique<platform::PosixProcessRunner>()) {}
```

`CLAUDE.md`: *"Jede Klasse hat `.h` + `.cc`, keine Header-only-Klassen außer bei trivialen Structs oder Templates."* `TypstRenderer` ist keine triviale Struct, hat aber beide Konstruktoren komplett im Header definiert. Für reine Member-Initialisierung ist das vertretbar (sehr verbreitetes Muster, auch im Google Style Guide für triviale Konstruktoren akzeptiert), weicht aber formal von der selbstgesetzten Regel ab.

**Vorschlag:** Bewusste Entscheidung treffen und ggf. in `CLAUDE.md` als Ausnahme ("triviale Konstruktoren dürfen inline bleiben") ergänzen, oder beide Konstruktor-Bodies nach `typst_renderer.cc` verschieben, um die Regel konsequent durchzuziehen.

---

## 8. `ValidateQuestion` prüft `topic` nicht auf Leerheit (Dringlichkeit: 2)

**Datei:** `src/repository/question_repository.cc:16-33`

`id`, `text` und `points` werden validiert — `topic` nicht. Eine Frage mit `topic: ""` wird klaglos geladen. Da `--topics` als Filter auf exakten String-Vergleich arbeitet (`test_generator.cc:34-36`), führt das nur zu "Frage taucht nie in einem gefilterten Test auf", kein Crash — aber inkonsistent zur sonst strikten Validierung der übrigen Pflichtfelder.

**Vorschlag:** Analog zu `id`/`text` eine Leer-Prüfung ergänzen, falls `topic` als Pflichtfeld gedacht ist (Katalogformat-Doku in `CLAUDE.md` deutet das an).

---

## 9. `CLAUDE.md` „Build & Run" nicht synchron mit tatsächlicher CLI (Dringlichkeit: 1)

**Datei:** `CLAUDE.md`, Abschnitt „Build & Run"

```bash
./build/questforge generate --catalog data/catalog --topics algebra,geometrie --count 20 --out test1.pdf
```

Dieses Beispiel passt nicht mehr zur implementierten CLI: `--catalog` erwartet eine einzelne YAML-Datei (kein Verzeichnis), es gibt keinen `--count`-Parameter (stattdessen `--easy/--medium/--hard`). Das `README.md` ist an dieser Stelle bereits korrekt und aktuell (`--catalog data/catalog/algebra.yaml --easy 1 --medium 1 --hard 1`). Reine Doku-Divergenz, kein Code-Problem — aber erwähnenswert, da `CLAUDE.md` selbst als Leitplanke für zukünftige Arbeit dient und hier auf einen überholten Stand verweist.

**Vorschlag:** Codeblock in `CLAUDE.md` an das README angleichen.

---

## Was bereits gut ist

- **Formatierung:** `clang-format --dry-run -style=Google` liefert für **alle** `src/`- und `tests/`-Dateien einen leeren Diff — keine Formatierungsabweichungen.
- **Namenskonvention:** Typen (`PascalCase`), Funktionen (`PascalCase`), Variablen (`snake_case`), Member (`snake_case_` mit Trailing-Underscore), Namespaces (`lower_case`), Konstanten (`kPascalCase`) — durchgängig korrekt angewendet.
- **Include Guards:** Konsequent im `QUESTFORGE_<PFAD>_<DATEI>_H_`-Schema.
- **Speicherverwaltung:** Kein rohes `new`/`delete`, `std::unique_ptr` für `ProcessRunner`, Wertsemantik sonst überall.
- **Sicherheit:** `fork`/`execvp` statt `system()` — Subprozess-Argumente werden nicht durch eine Shell interpretiert, strukturell keine Command-Injection möglich, selbst bei bösartigem Fragetext oder Dateipfad.
- **Schichtentrennung:** Keine Vermischung gefunden — YAML-Parsing bleibt im Repository, Zufallsauswahl im Generator, Templating/Subprozess im Renderer. Die Umbenennung `render/` → `renderer/` (Commit `94168b2`) hat Verzeichnis, Namespace und Include-Guard konsistent nachgezogen.
- **Testbarkeit:** `ProcessRunner`-Interface mit `PosixProcessRunner`-Default und Dependency Injection ermöglicht sauberes Mocking (`MockProcessRunner` in `typst_renderer_test.cc`) ohne echten `typst`-Aufruf in Unit-Tests.
- **Seed-Determinismus:** `TestGeneratorTest.SameSeedGivesSameOrder` / `DifferentSeedGivesDifferentOrder` verifizieren genau das in `CLAUDE.md` geforderte Reproduzierbarkeits-Verhalten.
