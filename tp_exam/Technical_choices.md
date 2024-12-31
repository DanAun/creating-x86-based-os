# Memory Mapping Overview

## **Physical Memory Layout**
| **Range**                | **Description**                |
|--------------------------|---------------------------------|
| `0x1000000 - 0x1ffffff`  | PGD and PTB memory range       |
| `0x000000 - 0x3fffff`    | Kernel memory range            |
| `0x200000 - 0x200fff`    | Process 1 kernel stack         |
| `0x201000 - 0x201fff`    | Process 2 kernel stack         |
| `0x400000 - 0x4fffff`    | Process 1 memory range         |
| `0x500000 - 0x5fffff`    | Process 2 memory range         |
| `0x600000 - 0x600fff`    | Shared memory range            |


## **Shared Memory**
- **Physical Address**: `0x00600000 - 0x00600fff` (4KB).
- **Task 1 Virtual Address**: `0x600000 - 0x600fff`.
- **Task 2 Virtual Address**: `0x700000 - 0x700fff`.

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

## Memory Segmentation Overview

### **Global Descriptor Table (GDT)**
- **Location**: `0x307fc0` (inside the kernel's bss for simplicity)  

#### **Descriptor Table Entries**
| **Index** | **Range**        | **Type**                              | **Details**                                                                                  |
|-----------|------------------|---------------------------------------|----------------------------------------------------------------------------------------------|
| **0**     | `0x0 - 0x0`      | System Segment                       | Reserved or invalid system type.                                                            |
| **1**     | `0x0 - 0xFFFFFFFF` | Code Segment (Kernel)                | - Conforming: No<br>- Readable: Yes<br>- Accessed: No                                        |
| **2**     | `0x0 - 0xFFFFFFFF` | Data Segment (Kernel)                | - Expansion Direction: Up<br>- Writable: Yes<br>- Accessed: Yes                              |
| **3**     | `0x0 - 0xFFFFFFFF` | Code Segment (User)                  | - Conforming: No<br>- Readable: Yes<br>- Accessed: No                                        |
| **4**     | `0x0 - 0xFFFFFFFF` | Data Segment (User)                  | - Expansion Direction: Up<br>- Writable: Yes<br>- Accessed: No                               |
| **5**     | `0x308160 - 0x30a1e9`      | System Segment                       | TSS.                                                            |

#### **Key Attributes**
- **Code Segment**:
  - Specifies executable memory.
  - Kernel-level segments (Index 1) are not accessible in user mode.
  - User-level segments (Index 3) conform to ring-3 privilege.
- **Data Segment**:
  - Defines readable/writable regions.
  - Kernel-level segments (Index 2) are isolated from user space.
  - User-level segments (Index 4) allow controlled access to user applications.
- **System Segment**:
  - Reserved entries (Index 0, 5) for TSS.


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
