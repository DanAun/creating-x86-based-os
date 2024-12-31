# Memory Mapping Overview

## **Physical Memory Layout**
| **Range**                | **Description**                |
|--------------------------|---------------------------------|
| `0x100000 - 0x1fffff`    | PGD and PTB memory range       |
| `0x200000 - 0x2fffff`    | Kernel memory range            |
| `0x200000 - 0x200fff`    | Process 1 kernel stack         |
| `0x201000 - 0x201fff`    | Process 2 kernel stack         |
| `0x300000 - 0x3fffff`    | Process 1 memory range         |
| `0x400000 - 0x4fffff`    | Process 2 memory range         |
| `0x500000 - 0x500fff`    | Shared memory range            |


## **Shared Memory**
- **Physical Address**: `0x00500000 - 0x00500fff` (4KB).
- **Task 1 Virtual Address**: `0x500000 - 0x500fff`.
- **Task 2 Virtual Address**: `0x600000 - 0x600fff`.
  - Different virtual addresses prevent mistakes in pointer handling.

---

## **Task Stacks**
- Each task has **two 4KB stacks** (user and kernel).

### **User Stacks**
- Identity-mapped:
  - **Process 1**: Grows downward from `0x3fffff`.
  - **Process 2**: Grows downward from `0x4fffff`.

### **Kernel Stacks**
- Mapped at virtual address `0xC0000000`:
  - **Process 1 Physical**: `0x200000 - 0x200fff`.
  - **Process 2 Physical**: `0x201000 - 0x201fff`.
- Physical locations are consecutive but separated to avoid overlap.

---

### Ce que nous avons pu faire :
- Mise en place de la **segmentation**.  
- Implémentation de la **pagination**, avec identity mapping du noyau et création d'espaces mémoire pour **process1** et **process2**.  
- Création d'une pile noyau et utilisateur pour chaque processus.  
- Mise en place d'un **espace mémoire partagé** entre les deux processus.  

### Ce qu'il reste à faire :
- Gestion des **interruptions** pour permettre l'incrémentation du compteur dans **process1** et sa lecture dans **process2**.  
- Implémentation d'un **appel système** pour le passage en **ring3**.  
- Démarrage des processus.  