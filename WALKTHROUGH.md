# Shop Inventory and Billing System

Portfolio Project #2. Student: AYUB ADNAN. Student ID: 2674270. Language: C++. Interface: console menus.

Open a terminal in this folder, run `build.bat`, then run `shop.exe`. Stay in this folder while the program is running, because it reads and writes the `data` folder here. Choose option 8 to exit. A normal exit returns 0.

The compiler is g++ from MinGW (w64devkit, GCC 16.2.0). `build.bat` uses that compiler when `g++` is not already on your PATH.

## What to say first

This program is a small shop system. A shopkeeper can add and edit products, change stock, make a bill with a discount, look up old receipts, and see reports. The records stay in two text files, so they are still there the next time the program starts.

## How the code is organized

Explain the files in this order.

1. `include/config.h` is handled by the preprocessor before compilation. `#include` pulls in headers, `#ifndef` stops a header from being included twice, and `#define` stores the program name, file names, currency, discount limit, and default reorder level. `#if` stops the build if the compiler is older than C++17. When you run g++, the preprocessor runs first, then the compiler builds `shop.exe`.

2. `include/Product.h` and `src/Product.cpp` define one product: id, name, category, price, quantity, and reorder level. `isLowStock()` is true when the quantity is at or below the reorder level. `stockValue()` is price times quantity. `toLine()` and `fromLine()` turn a product into one line of `data/products.txt`.

3. `include/Sale.h` defines a receipt. `SaleLine` is one item on the bill. The name and price are copied at sale time, so changing a price later does not change old receipts. `Sale` stores the date, discount, totals, and the list of lines.

4. `include/InventorySystem.h` and `src/InventorySystem.cpp` are the shop itself. A `vector` holds the products and another `vector` holds the sales. `load()` reads the files, or creates seven sample products the first time. `save()` writes both files. `recordSale()` checks the discount and the stock, subtracts the quantities, and stores the receipt.

5. `src/main.cpp` is the user interface. `showDashboard()` prints the totals and the eight menu options. The `switch` in `main()` sends each choice to product management, stock, billing, history, reports, search, or save. Option 8 saves and executes `return 0`.

## Live demo

The first run creates sample products. Cooking Oil and Shampoo start on low stock.

1. Choose `1`, then `4`. Show the product table and the LOW status. Choose `5` to go back.
2. Choose `6` and search for `oil`.
3. Choose `2`, then `1`. Add 10 units to product `2` (Cooking Oil). Choose `3` to show that only Shampoo is still low. Choose `4`.
4. Choose `3`. Sell product `1`, quantity `2`, then product `5`, quantity `3`. Enter `0`, then a discount of `10`, then `Y`. Read the receipt total: Rs. 2853.00.
5. Choose `4`, then sale `1`, to open that receipt again.
6. Choose `5`, then `2` for today's sales, then `3` for top sellers, then `4`.
7. Choose `8`. The program prints goodbye and returns 0.

If you want a clean sample shop again, close the program, delete the `data` folder, and run `shop.exe`.

`demo-input.txt` types through every menu, including invalid choices. It changes the saved shop. From this folder you can rehearse with:

```bat
shop.exe < demo-input.txt
```

Delete `data` afterward if the live demo should start from the sample products.

## Two AI tools

The AI Agent version of this assignment asks for two AI tools.

1. Cursor (Grok) wrote the C++ source, compiled it with g++, and tested every menu.
2. Claude read all eight source files and reviewed the program. It confirmed the console menu, the preprocessor directives, and `return 0` on exit. The review also found three edge cases, and those fixes are now in the code: prices such as `inf` are rejected, damaged product lines are reported instead of skipped silently, and the program prints the `data` folder path at startup.

You can say this in the presentation: "Claude reviewed the code and found that it meets the requirements. The only issues were edge cases, and those were fixed."
