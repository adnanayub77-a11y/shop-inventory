# Shop Inventory and Billing System — short presentation script

Use this as a 5–8 minute walk-through of the official brief. Compile first so the preprocessor message is on screen.

```bat
build.bat
shop.exe
```

While g++ runs, point at this line in the build log:

```text
note: #pragma message: Preprocessor: compiling Shop Inventory and Billing System 1.0
```

That comes from `#pragma message` in `include/config.h`. It proves the preprocessor ran **before** the compiler built `shop.exe`.

---

## 1. Why C++ (Step 1)

C is small and fast, but a shop with products, receipts, and reports is easier to keep tidy in classes. This programme is split into `Product`, `Sale`, and `InventorySystem`, so each file has one job.

## 2. Why this programme (Step 2)

The brief allows practical office use, model projections, or a game. This one is practical shop use:

- Add, edit, and delete products.
- Add or remove stock, and see low-stock items.
- Make a bill with a discount and print a receipt.
- Open an old receipt, search the catalog, and run reports.
- Records stay in `data/products.txt` and `data/sales.txt`.

## 3. Build and preprocessor (Step 3)

Original programme (no borrowed source). Preprocessor directives are in `include/config.h` and run before compiling:

| Directive | What it does here |
| --- | --- |
| `#ifndef` / `#define` / `#endif` | Header guard |
| `#define` | Programme name, student name, student ID, file names, currency, discount limit, reorder level |
| `#if` | Stop the build if the compiler is older than C++17 |
| `#error` | The message shown when the dialect is too old |
| `#pragma message` | Print the compile-time banner in the build log |
| `#ifdef` | Windows console title in `src/main.cpp` |

Build pipeline: write source → **preprocessor** → compiler → linker → run.

## 4. Compiler (Step 4)

**MinGW GCC** is the compiler used here. It is on the approved list (GCC, Clang, MinGW, Visual C++). On this PC, `build.bat` finds g++. `make` does the same build when g++ is on PATH. `make clang` is the Clang target.

```bat
build.bat
```

```bash
make
make clang
```

## 5. Live demo (Step 5)

Walk every menu item, then leave on **8**. The programme finishes with `return 0`.

The first run creates sample products. Cooking Oil and Shampoo start on low stock.

1. **Products** — choose `1`, then `4`. Show the product table and the LOW status. Choose `5` to go back.
2. **Search** — choose `6` and search for `oil`.
3. **Stock** — choose `2`, then `1`. Add 10 units to product `2` (Cooking Oil). Choose `3` to show that only Shampoo is still low. Choose `4`.
4. **Billing** — choose `3`. Sell product `1`, quantity `2`, then product `5`, quantity `3`. Enter `0`, then a discount of `10`, then `Y`. Read the receipt total: Rs. 2853.00.
5. **History** — choose `4`, then sale `1`, to open that receipt again.
6. **Reports** — choose `5`, then `2` for today's sales, then `3` for top sellers, then `4`.
7. **Save** — choose `7`.
8. **Exit** — choose `8`. The programme prints goodbye and returns 0.

Open another terminal and show:

```bat
echo %ERRORLEVEL%
```

After a normal quit it should print `0`. In Git Bash the same check is `echo $?`.

---

## Code walk (same order as a live tour)

| File | What to say |
| --- | --- |
| `include/config.h` | Header guard, `#define` limits, `#if` / `#error` for C++17, `#pragma message` at compile time. Student name and ID live here and are printed on the menu. |
| `src/main.cpp` | The menu. The `switch` sends each choice to a task. Option 8 saves and executes `return 0`. |
| `include/Product.h`, `src/Product.cpp` | One product. `isLowStock()` and `stockValue()`. One line of `data/products.txt`. |
| `include/Sale.h` | One receipt. The name and price are copied at sale time, so a later price change does not rewrite old bills. |
| `include/InventorySystem.h`, `src/InventorySystem.cpp` | The shop. `load()` and `save()`. `recordSale()` checks the discount and the stock. |

## If someone asks "what happens with bad input?"

The readers reject a blank name, a price that is not a real number, a quantity outside 0 to 100000, and a discount outside 0 to 100, then ask again. The programme does not exit until the user chooses 8.

Two AI tools: Cursor wrote the code; Claude reviewed it. See `AI_TOOLS.md`.
