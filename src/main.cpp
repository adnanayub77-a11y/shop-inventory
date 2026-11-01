/*
 * Portfolio Project #2
 * Shop Inventory and Billing System
 * Student: AYUB ADNAN
 * Student ID: 2674270
 * Language: C++17
 * Interface: console menus
 *
 * AI tools used for this project:
 * 1. Cursor (Grok) wrote this source code, compiled it with g++, and tested every menu.
 * 2. Claude reviewed the code. Its review added the finite-number check,
 *    the warning for damaged product lines, and the data-folder path at startup.
 */

#include "config.h"
#include "InventorySystem.h"
#include "textutil.h"

#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace {

bool inputEnded = false;

std::string money(double value) {
    std::ostringstream out;
    out << CURRENCY << std::fixed << std::setprecision(2) << value;
    return out.str();
}

std::string percentText(double value) {
    std::ostringstream out;
    if (std::fabs(value - std::round(value)) < 0.001) {
        out << static_cast<int>(std::lround(value));
    } else {
        out << std::fixed << std::setprecision(2) << value;
    }
    return out.str();
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        inputEnded = true;
        return "";
    }
    return trim(line);
}

int readInt(const std::string& prompt) {
    while (!inputEnded) {
        const std::string line = readLine(prompt);
        if (inputEnded) {
            return 0;
        }
        int value = 0;
        if (parseInt(line, value)) {
            return value;
        }
        std::cout << "Please enter a whole number.\n";
    }
    return 0;
}

double readDouble(const std::string& prompt) {
    while (!inputEnded) {
        const std::string line = readLine(prompt);
        if (inputEnded) {
            return 0.0;
        }
        double value = 0.0;
        if (parseDouble(line, value)) {
            return value;
        }
        std::cout << "Please enter a number.\n";
    }
    return 0.0;
}

bool readYesNo(const std::string& prompt) {
    while (!inputEnded) {
        const std::string line = toLowerCopy(readLine(prompt));
        if (inputEnded) {
            return false;
        }
        if (line == "y" || line == "yes") {
            return true;
        }
        if (line == "n" || line == "no") {
            return false;
        }
        std::cout << "Please enter Y or N.\n";
    }
    return false;
}

std::string readLabel(const std::string& prompt) {
    while (!inputEnded) {
        const std::string line = readLine(prompt);
        if (inputEnded) {
            return "";
        }
        if (line.empty()) {
            std::cout << "This value is required.\n";
            continue;
        }
        if (line.find('|') != std::string::npos) {
            std::cout << "The | character is not allowed.\n";
            continue;
        }
        return line;
    }
    return "";
}

void saveAndTell(InventorySystem& shop) {
    if (shop.save()) {
        std::cout << "Data saved.\n";
    } else {
        std::cout << "The change is in memory, but the data file could not be saved.\n";
    }
}

void printProductTable(const std::vector<Product>& products) {
    if (products.empty()) {
        std::cout << "No products to show.\n";
        return;
    }

    std::cout << std::left
              << std::setw(6) << "ID"
              << std::setw(24) << "Name"
              << std::setw(16) << "Category"
              << std::setw(16) << "Price"
              << std::setw(8) << "Qty"
              << std::setw(18) << "Stock value"
              << "Status\n";
    std::cout << std::string(96, '-') << '\n';

    for (const Product& product : products) {
        std::cout << std::left
                  << std::setw(6) << product.id
                  << std::setw(24) << clip(product.name, 22)
                  << std::setw(16) << clip(product.category, 14)
                  << std::setw(16) << money(product.price)
                  << std::setw(8) << product.quantity
                  << std::setw(18) << money(product.stockValue())
                  << (product.isLowStock() ? "LOW" : "OK")
                  << '\n';
    }
}

void printReceipt(const Sale& sale) {
    std::cout << "\n---------------- RECEIPT ----------------\n";
    std::cout << "Sale #" << sale.id << '\n';
    std::cout << sale.dateTime << '\n';
    std::cout << std::string(42, '-') << '\n';
    for (const SaleLine& line : sale.lines) {
        std::ostringstream label;
        label << line.productName << " x" << line.quantity;
        std::cout << std::left << std::setw(28) << clip(label.str(), 26)
                  << money(line.lineTotal()) << '\n';
    }
    std::cout << std::string(42, '-') << '\n';
    std::cout << std::left << std::setw(28) << "Subtotal" << money(sale.subtotal) << '\n';
    std::cout << std::left << std::setw(28) << ("Discount (" + percentText(sale.discountPercent) + "%)")
              << money(sale.discountAmount) << '\n';
    std::cout << std::left << std::setw(28) << "TOTAL" << money(sale.total) << '\n';
    std::cout << "-----------------------------------------\n";
}

void showDashboard(const InventorySystem& shop) {
    const std::vector<Product> lowStock = shop.lowStockProducts();
    std::cout << '\n' << MENU_LINE << '\n';
    std::cout << "  " << APP_NAME << '\n';
    std::cout << "  " << STUDENT_NAME << "  |  ID " << STUDENT_ID << '\n';
    std::cout << "  Version " << APP_VERSION << '\n';
    std::cout << MENU_LINE << '\n';
    std::cout << "  Products in catalog : " << shop.products().size() << '\n';
    std::cout << "  Stock value         : " << money(shop.totalStockValue()) << '\n';
    std::cout << "  Low-stock items     : " << lowStock.size() << '\n';
    std::cout << "  Sales recorded      : " << shop.sales().size() << '\n';
    std::cout << MENU_LINE << '\n';
    std::cout << "  1. Product management\n";
    std::cout << "  2. Stock management\n";
    std::cout << "  3. New sale / billing\n";
    std::cout << "  4. Sales history\n";
    std::cout << "  5. Reports\n";
    std::cout << "  6. Search products\n";
    std::cout << "  7. Save data\n";
    std::cout << "  8. Exit\n";
}

void addProductFlow(InventorySystem& shop) {
    const std::string name = readLabel("Product name: ");
    if (inputEnded) {
        return;
    }
    const std::string category = readLabel("Category: ");
    if (inputEnded) {
        return;
    }

    double price = 0.0;
    while (!inputEnded) {
        price = readDouble("Price: ");
        if (inputEnded) {
            return;
        }
        if (price > 0) {
            break;
        }
        std::cout << "Price must be greater than zero.\n";
    }

    int quantity = 0;
    while (!inputEnded) {
        quantity = readInt("Starting quantity: ");
        if (inputEnded) {
            return;
        }
        if (quantity >= 0 && quantity <= MAX_STOCK) {
            break;
        }
        std::cout << "Quantity must be from 0 to " << MAX_STOCK << ".\n";
    }

    int reorderLevel = LOW_STOCK_LIMIT;
    while (!inputEnded) {
        const std::string text = readLine("Reorder level (press Enter for " +
                                          std::to_string(LOW_STOCK_LIMIT) + "): ");
        if (inputEnded) {
            return;
        }
        if (text.empty()) {
            break;
        }
        if (parseInt(text, reorderLevel) && reorderLevel >= 0) {
            break;
        }
        std::cout << "Enter a whole number that is 0 or higher, or press Enter.\n";
    }

    std::string error;
    int newId = 0;
    if (shop.addProduct(name, category, price, quantity, reorderLevel, error, newId)) {
        std::cout << "Product added with ID " << newId << ".\n";
        saveAndTell(shop);
    } else {
        std::cout << error << '\n';
    }
}

void editProductFlow(InventorySystem& shop) {
    printProductTable(shop.products());
    const int id = readInt("Product ID to edit: ");
    if (inputEnded) {
        return;
    }
    const Product* product = shop.findProduct(id);
    if (product == nullptr) {
        std::cout << "No product uses ID " << id << ".\n";
        return;
    }

    std::string name = readLine("Name [" + product->name + "]: ");
    if (inputEnded) {
        return;
    }
    if (name.empty()) {
        name = product->name;
    }

    std::string category = readLine("Category [" + product->category + "]: ");
    if (inputEnded) {
        return;
    }
    if (category.empty()) {
        category = product->category;
    }

    double price = product->price;
    while (!inputEnded) {
        const std::string text = readLine("Price [" + money(product->price) + "]: ");
        if (inputEnded) {
            return;
        }
        if (text.empty()) {
            break;
        }
        if (parseDouble(text, price) && price > 0) {
            break;
        }
        std::cout << "Enter a price greater than zero, or press Enter to keep it.\n";
    }

    int reorderLevel = product->reorderLevel;
    while (!inputEnded) {
        const std::string text = readLine("Reorder level [" + std::to_string(product->reorderLevel) + "]: ");
        if (inputEnded) {
            return;
        }
        if (text.empty()) {
            break;
        }
        if (parseInt(text, reorderLevel) && reorderLevel >= 0) {
            break;
        }
        std::cout << "Enter a whole number that is 0 or higher, or press Enter to keep it.\n";
    }

    std::string error;
    if (shop.updateProduct(id, name, category, price, reorderLevel, error)) {
        std::cout << "Product " << id << " updated. Stock quantity was left unchanged.\n";
        saveAndTell(shop);
    } else {
        std::cout << error << '\n';
    }
}

void deleteProductFlow(InventorySystem& shop) {
    printProductTable(shop.products());
    const int id = readInt("Product ID to delete: ");
    if (inputEnded) {
        return;
    }
    const Product* product = shop.findProduct(id);
    if (product == nullptr) {
        std::cout << "No product uses ID " << id << ".\n";
        return;
    }
    if (!readYesNo("Delete " + product->name + "? (Y/N): ")) {
        std::cout << "Delete cancelled.\n";
        return;
    }

    std::string error;
    if (shop.deleteProduct(id, error)) {
        std::cout << "Product deleted. Older receipts still keep its name.\n";
        saveAndTell(shop);
    } else {
        std::cout << error << '\n';
    }
}

void manageProducts(InventorySystem& shop) {
    while (!inputEnded) {
        std::cout << "\n-- Product management --\n";
        std::cout << "1. Add product\n";
        std::cout << "2. Edit product\n";
        std::cout << "3. Delete product\n";
        std::cout << "4. List all products\n";
        std::cout << "5. Back to main menu\n";
        const int choice = readInt("Select an option (1-5): ");
        if (inputEnded) {
            return;
        }

        switch (choice) {
        case 1:
            addProductFlow(shop);
            break;
        case 2:
            editProductFlow(shop);
            break;
        case 3:
            deleteProductFlow(shop);
            break;
        case 4:
            printProductTable(shop.products());
            break;
        case 5:
            return;
        default:
            std::cout << "Choose a number from 1 to 5.\n";
            break;
        }
    }
}

void changeStock(InventorySystem& shop, bool adding) {
    printProductTable(shop.products());
    const int id = readInt("Product ID: ");
    if (inputEnded) {
        return;
    }
    const int amount = readInt(adding ? "Quantity to add: " : "Quantity to remove: ");
    if (inputEnded) {
        return;
    }

    std::string error;
    const bool ok = adding ? shop.addStock(id, amount, error) : shop.removeStock(id, amount, error);
    if (!ok) {
        std::cout << error << '\n';
        return;
    }

    const Product* product = shop.findProduct(id);
    std::cout << product->name << " now has " << product->quantity << " in stock.\n";
    saveAndTell(shop);
}

void manageStock(InventorySystem& shop) {
    while (!inputEnded) {
        std::cout << "\n-- Stock management --\n";
        std::cout << "1. Add stock\n";
        std::cout << "2. Remove stock (damage, loss, or internal use)\n";
        std::cout << "3. View low-stock items\n";
        std::cout << "4. Back to main menu\n";
        const int choice = readInt("Select an option (1-4): ");
        if (inputEnded) {
            return;
        }

        switch (choice) {
        case 1:
            changeStock(shop, true);
            break;
        case 2:
            changeStock(shop, false);
            break;
        case 3: {
            const std::vector<Product> lowStock = shop.lowStockProducts();
            if (lowStock.empty()) {
                std::cout << "Every product is above its reorder level.\n";
            } else {
                printProductTable(lowStock);
            }
            break;
        }
        case 4:
            return;
        default:
            std::cout << "Choose a number from 1 to 4.\n";
            break;
        }
    }
}

int quantityInCart(const std::vector<SaleLine>& cart, int productId) {
    int total = 0;
    for (const SaleLine& line : cart) {
        if (line.productId == productId) {
            total += line.quantity;
        }
    }
    return total;
}

void printCart(const std::vector<SaleLine>& cart) {
    double subtotal = 0.0;
    std::cout << "Cart:\n";
    for (const SaleLine& line : cart) {
        std::cout << "  " << line.productName << " x" << line.quantity
                  << " = " << money(line.lineTotal()) << '\n';
        subtotal += line.lineTotal();
    }
    std::cout << "  Subtotal: " << money(subtotal) << '\n';
}

void newSale(InventorySystem& shop) {
    printProductTable(shop.products());
    std::vector<SaleLine> cart;

    while (!inputEnded) {
        const int productId = readInt("Product ID (0 to finish): ");
        if (inputEnded) {
            return;
        }
        if (productId == 0) {
            break;
        }

        const Product* product = shop.findProduct(productId);
        if (product == nullptr) {
            std::cout << "No product uses ID " << productId << ".\n";
            continue;
        }

        const int already = quantityInCart(cart, productId);
        const int available = product->quantity - already;
        if (available <= 0) {
            std::cout << product->name << " has no remaining stock for this sale.\n";
            continue;
        }

        const int quantity = readInt("Quantity (available " + std::to_string(available) + "): ");
        if (inputEnded) {
            return;
        }
        if (quantity <= 0 || quantity > available) {
            std::cout << "Enter a quantity from 1 to " << available << ".\n";
            continue;
        }

        bool merged = false;
        for (SaleLine& existing : cart) {
            if (existing.productId == product->id) {
                existing.quantity += quantity;
                merged = true;
                break;
            }
        }
        if (!merged) {
            SaleLine line;
            line.productId = product->id;
            line.productName = product->name;
            line.quantity = quantity;
            line.unitPrice = product->price;
            cart.push_back(line);
        }
        printCart(cart);
    }

    if (cart.empty()) {
        std::cout << "No items were added. Sale cancelled.\n";
        return;
    }

    double discount = 0.0;
    while (!inputEnded) {
        discount = readDouble("Discount percent (0-" + std::to_string(MAX_DISCOUNT_PERCENT) + "): ");
        if (inputEnded) {
            return;
        }
        if (discount >= 0 && discount <= MAX_DISCOUNT_PERCENT) {
            break;
        }
        std::cout << "Discount must be from 0 to " << MAX_DISCOUNT_PERCENT << ".\n";
    }

    printCart(cart);
    if (!readYesNo("Confirm sale (Y/N): ")) {
        std::cout << "Sale cancelled. Stock was not changed.\n";
        return;
    }

    std::vector<CartItem> items;
    for (const SaleLine& line : cart) {
        items.push_back(CartItem{line.productId, line.quantity});
    }

    Sale created;
    std::string error;
    if (!shop.recordSale(items, discount, created, error)) {
        std::cout << error << '\n';
        return;
    }

    printReceipt(created);
    saveAndTell(shop);
}

void salesHistory(const InventorySystem& shop) {
    if (shop.sales().empty()) {
        std::cout << "No sales have been recorded yet.\n";
        return;
    }

    std::cout << std::left
              << std::setw(8) << "Sale"
              << std::setw(20) << "Date"
              << std::setw(16) << "Total"
              << "Items\n";
    std::cout << std::string(52, '-') << '\n';
    for (const Sale& sale : shop.sales()) {
        std::cout << std::left
                  << std::setw(8) << sale.id
                  << std::setw(20) << sale.dateTime
                  << std::setw(16) << money(sale.total)
                  << sale.lines.size() << '\n';
    }

    const int id = readInt("Sale ID for the receipt (0 to go back): ");
    if (inputEnded || id == 0) {
        return;
    }
    const Sale* sale = shop.findSale(id);
    if (sale == nullptr) {
        std::cout << "No sale uses ID " << id << ".\n";
        return;
    }
    printReceipt(*sale);
}

void reports(const InventorySystem& shop) {
    while (!inputEnded) {
        std::cout << "\n-- Reports --\n";
        std::cout << "1. Inventory report\n";
        std::cout << "2. Today's sales\n";
        std::cout << "3. Top-selling products\n";
        std::cout << "4. Back to main menu\n";
        const int choice = readInt("Select an option (1-4): ");
        if (inputEnded) {
            return;
        }

        switch (choice) {
        case 1:
            printProductTable(shop.products());
            std::cout << "Total stock value: " << money(shop.totalStockValue()) << '\n';
            break;
        case 2: {
            const std::string date = InventorySystem::today();
            const std::vector<Sale> todaysSales = shop.salesOn(date);
            std::cout << "Sales on " << date << ":\n";
            if (todaysSales.empty()) {
                std::cout << "No sales have been recorded today.\n";
                break;
            }
            double total = 0.0;
            for (const Sale& sale : todaysSales) {
                std::cout << "  Sale #" << sale.id << "  " << sale.dateTime
                          << "  " << money(sale.total) << '\n';
                total += sale.total;
            }
            std::cout << "Today's total: " << money(total) << '\n';
            break;
        }
        case 3: {
            const auto top = shop.topSelling(5);
            if (top.empty()) {
                std::cout << "No sales yet, so there are no top sellers.\n";
                break;
            }
            std::cout << "Top-selling products:\n";
            int rank = 1;
            for (const auto& row : top) {
                std::cout << "  " << rank << ". " << row.first << "  -  " << row.second << " sold\n";
                ++rank;
            }
            break;
        }
        case 4:
            return;
        default:
            std::cout << "Choose a number from 1 to 4.\n";
            break;
        }
    }
}

void searchProducts(const InventorySystem& shop) {
    const std::string query = readLine("Search by name, category, or ID: ");
    if (inputEnded) {
        return;
    }
    if (query.empty()) {
        std::cout << "Enter a name, category, or ID.\n";
        return;
    }
    printProductTable(shop.search(query));
}

void setConsoleTitle() {
#ifdef _WIN32
    SetConsoleTitleA(APP_NAME);
#endif
}

}  // namespace

int main() {
    setConsoleTitle();
    InventorySystem shop;

    try {
        if (!shop.load()) {
            std::cout << "The saved shop data could not be read.\n";
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cout << "Could not prepare the data folder: " << ex.what() << '\n';
        return 1;
    }

    std::cout << "Shop data loaded from " << std::filesystem::absolute("data") << ".\n";
    std::cout << "Choose a number from the menu.\n";

    while (!inputEnded) {
        showDashboard(shop);
        const int choice = readInt("Select an option (1-8): ");
        if (inputEnded) {
            break;
        }

        switch (choice) {
        case 1:
            manageProducts(shop);
            break;
        case 2:
            manageStock(shop);
            break;
        case 3:
            newSale(shop);
            break;
        case 4:
            salesHistory(shop);
            break;
        case 5:
            reports(shop);
            break;
        case 6:
            searchProducts(shop);
            break;
        case 7:
            if (shop.save()) {
                std::cout << "Saved " << PRODUCT_FILE << " and " << SALES_FILE << ".\n";
            } else {
                std::cout << "Save failed. Check that this folder can be written.\n";
            }
            break;
        case 8:
            if (!shop.save()) {
                std::cout << "Save failed, but the program will still close.\n";
            }
            std::cout << "\nData saved. Goodbye.\n";
            return 0;
        default:
            std::cout << "Choose a number from 1 to 8.\n";
            break;
        }
    }

    shop.save();
    return 0;
}
