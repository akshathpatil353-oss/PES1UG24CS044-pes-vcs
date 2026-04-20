# PES-VCS Lab Report

**Name:** Akshath Patil
**SRN:** PES1UG24CS044
**Platform:** Ubuntu 22.04

---

## Build Instructions

```bash
sudo apt update && sudo apt install -y gcc build-essential libssl-dev
export PES_AUTHOR="Akshath Patil <PES1UG24CS044>"
make all
```

---

## Phase 1 — Object Storage Foundation

**Files modified:** `object.c`

`object_write` creates objects in the format `<type> <size>\0<data>`, computes SHA-256 hash using `compute_hash`, ensures deduplication, writes using a temporary file, and renames atomically.

`object_read` reads objects, verifies integrity by recomputing hash, parses header, and returns the stored data.

### Screenshot — Phase 1 Output

![Phase 1](phase1.png)

---

## Phase 2 — Tree Objects

**Files modified:** `tree.c`

`tree_from_index` builds a directory tree structure from index entries. It handles nested directories recursively and stores tree objects using content-addressable storage.

### Screenshot — Phase 2 Output

![Phase 2](phase2.png)

---

## Phase 3 — Index (Staging Area)

**Files modified:** `index.c`

`index_load` loads index entries from `.pes/index`.

`index_save` writes index atomically.

`index_add` stages files by hashing and updating index.

### Screenshot — Phase 3 Output

![Phase 3](phase3.png)

---

## Phase 4 — Commits and History

**Files modified:** `commit.c`

`commit_create`:

* Builds tree from index
* Reads parent commit
* Stores commit object
* Updates HEAD reference

### Screenshot — Commit Log

![Phase 4A](phase4a.png)

### Screenshot — Object Store

![Phase 4B](phase4b.png)

### Screenshot — HEAD and Branch

![Phase 4C](phase4c.png)

---

## Phase 5 — Branching and Checkout

### Q5.1 — Checkout Implementation

* Update `.pes/HEAD`
* Load target commit
* Update working directory files
* Handle conflicts

### Q5.2 — Dirty Check

* Compare index vs working directory
* Use metadata (mtime, size) + hashing

### Q5.3 — Detached HEAD

* HEAD points to commit directly
* New commits are not linked to any branch
* Recovery by creating a new branch

---

## Phase 6 — Garbage Collection

### Q6.1 — Mark and Sweep

* Start from HEAD and branches
* Traverse commits, trees, blobs
* Delete unreachable objects

### Q6.2 — Race Condition

* GC may delete objects during commit
* Git avoids using grace period and safe writes

---

## Conclusion

This project demonstrates how Git internally manages version control using:

* Content-addressable storage
* Trees and commits
* Index staging
* Efficient storage and integrity checks
