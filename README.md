# wordle_elysee_Ben_Yanid
Ce programme charge une base de mots de 5 lettres (tirée du fichier `ressource/bdd_wordle.txt`) et permet de filtrer ces mots selon différents critères.  
Il est conçu pour aider à résoudre les grilles de **Wordle** en affinant la liste des mots possibles à chaque essai.

Fonctionnalités :
- Filtrer les mots qui **contiennent** certaines lettres
- Filtrer les mots qui **ne contiennent pas** certaines lettres
- Filtrer les mots qui contiennent une **sous‑chaîne** (ex: `"at"`, `"ing"`)
- Combiner les critères d’inclusion et d’exclusion

---

## Structure du projet
```
wordle_solver/
├── bin/                  
├── include/
│   └── wordle_solver.h   
├── src/
│   ├── main.c            
│   └── wordle_solver.c   
├── ressource/
│   └── bdd_wordle.txt 
|
└── README.md
```
### Commandes générales

 Commande        | Description                              
                 |
 `help`          | Afficher l'aide                         
 `list`          | Lister tous les mots restants            
 `suggest [n]`   | Afficher les n meilleures suggestions    
 `count`         | Nombre de mots restants                  
 `reset`         | Réinitialiser la liste complète          
 `quit`          | Quitter le programme                    

### Filtres disponibles

#### `include <lettres>`
Garde uniquement les mots **contenant toutes** les lettres spécifiées.
```
[2309 mots] > include ae
Filtre 'include ae' : 2309 → 172 mots
```

#### `exclude <lettres>`
Supprime les mots contenant **au moins une** des lettres spécifiées.
```
[2309 mots] > exclude xyz
Filtre 'exclude xyz' : 2309 → 2240 mots
```

#### `pos <lettre> <position>`
Garde les mots ayant la lettre **exactement** à la position donnée (1 à 5).
```
[2309 mots] > pos a 1
Filtre 'pos a 1' : 2309 → 141 mots
```

#### `misplace <lettre> <position>`
Lettre **présente** dans le mot, mais **pas** à cette position (indice jaune Wordle).
```
[172 mots] > misplace e 2
Filtre 'misplace e 2' : 172 → 98 mots
```

#### `sub <sous-chaîne>`
Garde les mots contenant la **sous-chaîne** exacte.
```
[2309 mots] > sub ain
Filtre 'sub ain' : 2309 → 42 mots
```

#### `result <mot> <code>`
Applique un résultat Wordle complet en une seule commande.  
Le code utilise :
- `v` = **v**ert (bonne lettre, bonne position)
- `o` = **o**range (bonne lettre, mauvaise position)
- `x` = gris (lettre absente)

```
[2309 mots] > result adieu oxovx
Résultat 'adieu' / 'oxovx' : 2309 → 8 mots
```

### Exemple de session complète

```
[2309 mots] > result adieu oxovx
Résultat 'adieu' / 'oxovx' : 2309 → 8 mots

[8 mots] > result image oxoxo
Résultat 'image' / 'oxoxo' : 8 → 1 mots
Suggestions (1/1 mot(s) restants) :
  linea

[1 mots] > quit
Au revoir !
```

## Ateurs:
```
Elysee et Ben Yanid
```

