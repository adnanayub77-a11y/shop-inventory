#ifndef CONFIG_H
#define CONFIG_H

// The preprocessor checks the language version before the compiler builds the program.
#if __cplusplus < 201703L
#error This program requires C++17. Compile with: g++ -std=c++17
#endif

#define APP_NAME "Shop Inventory and Billing System"
#define APP_VERSION "1.0"
#define STUDENT_NAME "AYUB ADNAN"
#define STUDENT_ID "2674270"
#define CURRENCY "Rs. "
#define MENU_LINE "================================================"

#define PRODUCT_FILE "data/products.txt"
#define SALES_FILE "data/sales.txt"

// A product at or below this level is marked LOW when a reorder level is not typed.
#define LOW_STOCK_LIMIT 5

// Business rules kept in one place so the menus and the checks stay the same.
#define MAX_DISCOUNT_PERCENT 100
#define MAX_STOCK 100000

// Printed while compiling, so the log shows the preprocessor ran before the compiler.
#ifdef SHOP_ANNOUNCE_BUILD
#pragma message("Preprocessor: compiling " APP_NAME " " APP_VERSION)
#endif

#endif
