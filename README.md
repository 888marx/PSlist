## Conditions préalables

- Un système d'exploitation Windows
- Visual Studio C++ Build Tools, qui peut être téléchargé à partir de [ici](https://visualstudio.microsoft.com/visual-cpp-build-tools/). Lors de l'installation, assurez-vous d'ajouter le plugin d'environnement C++ Desktop.
## Compilation

1. Ouvrez l'invite de commande de Visual Studio C++ Build Tools et placez-vous dans le répertoire contenant le fichier `pslist.c`.
2. Exécutez la commande suivante pour compiler le programme :

```bash
cl pslist.c
```

## Utilisation

Ce programme est un outil en ligne de commande qui répertorie les processus et leurs détails. Voici comment l'utiliser :

1. Ouvrez une invite de commande.
2. Naviguez jusqu'au répertoire contenant le fichier `pslist.exe`.
3. Exécutez le programme avec l'une des commandes suivantes :

```bash
pslist.exe # Liste tous les processus
pslist.exe <nom> # Liste les détails du nom du processus spécifié
pslist.exe -d <PID> # Affiche les détails des threads pour l'ID du processus spécifié
pslist.exe -h # Affiche le message d'aide
```
