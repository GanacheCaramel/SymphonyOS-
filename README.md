cat > README.md << 'EOF'
# SymphonyOS

> Un système d'exploitation 32-bit écrit from scratch en C et Assembly.

![Statut](https://img.shields.io/badge/status-en%20d%C3%A9veloppement-yellow)
![Architecture](https://img.shields.io/badge/arch-x86-blue)
![Langage](https://img.shields.io/badge/langage-C%20%2F%20ASM-lightgrey)
![Licence](https://img.shields.io/badge/licence-MIT-green)

---

## 📖 À propos

**SymphonyOS** est un projet pédagogique de développement d'un système
d'exploitation **32-bit** pour l'architecture **x86**, avec :

- Un **bootloader maison** en Assembly (mode 16-bit → 32-bit)
- Un **noyau C** minimaliste
- Un **shell interactif** avec historique et scroll
- Des **pilotes** clavier (IRQ1) et vidéo (VGA texte)
- Une **libc minimale** (string, stdio)

---

## ✨ Fonctionnalités

| Fonctionnalité | Statut |
|----------------|--------|
| Bootloader BIOS | ✅ |
| Passage en mode protégé | ✅ |
| GDT | ✅ |
| IDT + PIC | ✅ |
| Clavier (IRQ1) | ✅ |
| Écran VGA (80×25) | ✅ |
| Shell interactif | ✅ |
| Historique des commandes | ✅ |
| Scroll | ✅ |
| Curseur clignotant | ✅ |
| libc (string, stdio) | ✅ |
| Support GRUB / Multiboot | 🚧 |
| Gestion mémoire | 🚧 |
| Système de fichiers | 🚧 |

---

## 🛠️ Prérequis

- **clang** (avec support `--target=i386-elf`)
- **ld.lld** (linker LLVM)
- **nasm** (assembleur)
- **llvm-objcopy**
- **qemu-system-i386** (émulateur)
- **make**

### Installation (Debian / Ubuntu)

```bash
sudo apt install clang lld nasm llvm qemu-system-x86 make

Installation (Fedora)

```bash
sudo dnf install clang lld nasm llvm qemu-system-x86 make
```

Installation (Arch)

```bash
sudo pacman -S clang lld nasm llvm qemu-system-x86 make
```

---

🚀 Compilation et lancement

```bash
# Cloner le repo
git clone https://github.com/GanacheCaramel/SymphonyOS.git
cd SymphonyOS

# Compiler
make

# Lancer dans QEMU
make run

# Nettoyer
make clean
```

---

📂 Structure du projet

```
SymphonyOS/
├── arch/x86/          # Code spécifique à l'architecture x86
│   ├── boot/          # Bootloader + GDT
│   ├── idt.c/h        # IDT (Interrupt Descriptor Table)
│   ├── irq.asm        # Handlers d'IRQ
│   ├── pic.c/h        # PIC (Programmable Interrupt Controller)
│   └── start.asm      # Point d'entrée du kernel
│
├── kernel/            # Noyau
│   ├── kernel.c       # kernel_main()
│   ├── shell.c/h      # Shell interactif
│   ├── banner.c/h     # Banner ASCII art
│   └── include/       # Types, I/O
│
├── drivers/           # Pilotes
│   ├── video/         # Écran VGA
│   └── input/         # Clavier
│
├── libc/              # Bibliothèque C minimale
│   ├── string.c
│   └── stdio.c
│
├── boot/iso/          # Pour le futur support GRUB
├── scripts/           # Scripts de build et run
├── docs/              # Documentation + screenshots
├── Makefile           # Build
└── linker.ld          # Linker script
```

---

🎮 Commandes du shell

Commande || Description
-------------------------------
help     || Liste les commandes
clear    || Efface l'écran
version  || Affiche la version
echo <t> || Affiche <texte>
history  || Affiche l'historique
reboot   || Redémarre le système

Raccourcis :

· ↑ / ↓ : naviguer dans l'historique

---

🗺️ Roadmap

☑ Bootloader BIOS
☑ Mode protégé
☑ IDT + PIC
☑ Clavier
☑ Écran VGA
☑ Shell
☑ Historique + Scroll
☑ Curseur clignotant
☑ libc minimale
☐ Shift / CapsLock
☐ Timer PIT
☐ Support GRUB / Multiboot
☐ Gestion mémoire (pagination)
☐ Système de fichiers
☐ Multi-tâches

---

🤝 Contribution

Les contributions sont les bienvenues !

1. Fork le projet
2. Crée une branche (git checkout -b feature/ma-fonctionnalite)
3. Commit (git commit -m "Ajout de ma fonctionnalité")
4. Push (git push origin feature/ma-fonctionnalite)
5. Ouvre une Pull Request

---

📄 Licence

Ce projet est sous licence MIT. Voir LICENSE.

---

🙏 Remerciements

· OSDev Wiki

---

⭐ Si ce projet t'a aidé, n'hésite pas à lui donner une étoile !!
