# Lab 2 - Création d’un module Python en C/C++

## Auteur

Laboratoire créé par Alan Brucher

## Installation

Pour installer le projet, utilisez le bouton "Open with Visual Studio" situé dans "Code".

**Verifiez que vous utilisez Python 3.10 et êtes en Debug x64**

Dans les propriétés du projet, remplacer VC++ Directories->Include et VC++ Directories->Directories pour avoir le bon path.

Si ce n'est pas déja fait, modifier ces propriétés

- General -> Target Name = LogicielLab2
- General -> Configuration Type = Dynamic Library (.dll)
- Advanced -> Target File extension = .pyd

Ouvrez l'invite de commande et inserez ces commandes pour installer la librairie nécéssaire.

```bash
pip install keyboard
```

Compiler le projet.

Ouvrez "Logiciel_Lab2" et modifier aussi le path du Debug et de la vidéo "Exemple.avi" à la ligne 4 et 20.

Lancer le projet en insérant ceci (avec le bon path) dans l'invite de commandes:

```bash
python C:\Users\..\LogicielLab2\Logiciel_Lab2.py
```

Dès l'apparition de la fenêtre, utiliser les touches ci-dessous pour effectuer le contrôle de la vidéo.

- P - Play ou Pause
- S - Avance accélérée
- R - Retour au début
- Q - Quitter 

## Licence

[MIT](https://choosealicense.com/licenses/mit/)

## Sources 

Librairie Keyboard: https://github.com/boppreh/keyboard#api

https://stackoverflow.com/questions/24072790/how-to-detect-key-presses

PyObject* to String: https://docs.python.org/3/c-api/unicode.html

String to LPCWSTR: https://www.codegrepper.com/code-examples/cpp/c%2B%2B+convert+const+char%2A+to+LPCWSTR
