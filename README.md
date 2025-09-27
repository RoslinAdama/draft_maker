# draft_maker
Tools ton automatically generate football drafts

La version cpp passe de ~1300 iter/s à 50000 KAPPACHUNGUS PYTHON DELUXE O(n²) EDITION

## Installation

### Cmake & Clang

```bash
sudo apt install clang cmake
```

## Données

Créez votre compo suivant le modèle dans `data/exemple.xlsx` et placez-la dans le dossier `data/`.
Puis exporter en csv les deu sheets sous `data/name.csv` et `data/rate.csv`

## Usage

### Build 

```bash
cmake -S . -B build && cmake --build build --target draftMaker -- -j$(nproc)
```

Lancez l'interface avec :

```bash
./build/draftMaker ./data/name.csv ./data/rate.csv
```
