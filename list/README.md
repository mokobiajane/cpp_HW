# SKIP_LIST

<div align="center">
Mokobia Jane Chidima

Group:24.B83-mm

st135735@student.spbu.ru

</div>
---

---

## 🏗️ Project Structure

```
parallel-bmp/
├── include/
│   └── skip_list.hpp              # Function declarations, structs
├── src/
│   └── main.cpp & test.cpp            # Image processing logic and entry p
|               
├── docs/                     # Auto-generated Doxygen documentation
     ├── Doxyfile & mainpage.dox                 # Doxygen configuration
├── Makefile                  # Build script with doc generation
└── .github/
    └── workflows/
        └── ci.yml            # GitHub Actions CI config
```

---

## 🔧 Dependencies and Installation

### Requirements

- **C++17** compatible compiler (GCC, Clang, MSVC)
- **Makefile**
- ** Test** (for unit tests)
- **Doxygen** (for documentation generation)

# Build the project
make
./main
```

---


## 🧪 Testing

The project has complete unit test coverage using the googletest framework.

```bash
# Run tests
make tets


```
---
## 📚 Documentation

The project code is fully documented using Doxygen.

```bash
# Generate documentation
doxygen docs/Doxyfile
xdg-open docs/html/index.html
```

After running this command, you will find the generated documentation in the `docs/html` directory.



