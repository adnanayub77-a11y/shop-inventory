#include "config.h"
#include "InventorySystem.h"
#include "textutil.h"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

namespace {

bool validLabel(const std::string& text, std::string& error, const char* fieldName) {
    if (text.empty()) {
        error = std::string(fieldName) + " is required.";
        return false;
    }
    if (text.find('|') != std::string::npos) {
        error = std::string(fieldName) + " cannot contain the | character.";
        return false;
    }
    return true;
}

}  // namespace

bool InventorySystem::load() {
    std::filesystem::create_directories("data");

    const bool productsMissing = !std::filesystem::exists(PRODUCT_FILE);
    if (productsMissing) {
        seedSampleData();
    } else if (!loadProducts()) {
        return false;
    }

    if (std::filesystem::exists(SALES_FILE) && !loadSales()) {
        return false;
    }

    if (productsMissing) {
        return save();
    }
    return true;
}

bool InventorySystem::save() const {
    std::filesystem::create_directories("data");
    return writeProducts() && writeSales();
}

void InventorySystem::seedSampleData() {
    struct Row {
        const char* name;
        const char* category;
        double price;
        int quantity;
        int reorderLevel;
    };

    const Row rows[] = {
        {"Basmati Rice 5kg", "Grocery", 1450.0, 20, 5},
        {"Cooking Oil 1L", "Grocery", 580.0, 4, 8},
        {"Sugar 1kg", "Grocery", 180.0, 35, 10},
        {"Tea 900g", "Grocery", 1250.0, 12, 4},
        {"Soap Bar", "Household", 90.0, 40, 10},
        {"Shampoo 200ml", "Household", 420.0, 3, 6},
        {"Notebook A4", "Stationery", 150.0, 25, 5},
    };

    for (const Row& row : rows) {
        std::string error;
        int newId = 0;
        addProduct(row.name, row.category, row.price, row.quantity, row.reorderLevel, error, newId);
    }
}

bool InventorySystem::addProduct(const std::string& name, const std::string& category, double price,
                                 int quantity, int reorderLevel, std::string& error, int& newId) {
    const std::string cleanName = trim(name);
    const std::string cleanCategory = trim(category);
    if (!validLabel(cleanName, error, "Product name") ||
        !validLabel(cleanCategory, error, "Category")) {
        return false;
    }
    if (price <= 0) {
        error = "Price must be greater than zero.";
        return false;
    }
    if (quantity < 0 || quantity > MAX_STOCK) {
        error = "Quantity must be from 0 to " + std::to_string(MAX_STOCK) + ".";
        return false;
    }
    if (reorderLevel < 0) {
        error = "Reorder level cannot be negative.";
        return false;
    }

    Product product;
    product.id = nextProductId_++;
    product.name = cleanName;
    product.category = cleanCategory;
    product.price = price;
    product.quantity = quantity;
    product.reorderLevel = reorderLevel;
    products_.push_back(product);
    newId = product.id;
    error.clear();
    return true;
}

bool InventorySystem::updateProduct(int id, const std::string& name, const std::string& category,
                                    double price, int reorderLevel, std::string& error) {
    Product* product = findProduct(id);
    if (product == nullptr) {
        error = "No product uses ID " + std::to_string(id) + ".";
        return false;
    }

    const std::string cleanName = trim(name);
    const std::string cleanCategory = trim(category);
    if (!validLabel(cleanName, error, "Product name") ||
        !validLabel(cleanCategory, error, "Category")) {
        return false;
    }
    if (price <= 0) {
        error = "Price must be greater than zero.";
        return false;
    }
    if (reorderLevel < 0) {
        error = "Reorder level cannot be negative.";
        return false;
    }

    product->name = cleanName;
    product->category = cleanCategory;
    product->price = price;
    product->reorderLevel = reorderLevel;
    error.clear();
    return true;
}

bool InventorySystem::deleteProduct(int id, std::string& error) {
    const auto it = std::find_if(products_.begin(), products_.end(),
                                 [id](const Product& product) { return product.id == id; });
    if (it == products_.end()) {
        error = "No product uses ID " + std::to_string(id) + ".";
        return false;
    }
    products_.erase(it);
    error.clear();
    return true;
}

bool InventorySystem::addStock(int id, int amount, std::string& error) {
    Product* product = findProduct(id);
    if (product == nullptr) {
        error = "No product uses ID " + std::to_string(id) + ".";
        return false;
    }
    if (amount <= 0) {
        error = "Enter a quantity greater than zero.";
        return false;
    }
    if (product->quantity > MAX_STOCK - amount) {
        error = "That would put the stock above " + std::to_string(MAX_STOCK) + ".";
        return false;
    }
    product->quantity += amount;
    error.clear();
    return true;
}

bool InventorySystem::removeStock(int id, int amount, std::string& error) {
    Product* product = findProduct(id);
    if (product == nullptr) {
        error = "No product uses ID " + std::to_string(id) + ".";
        return false;
    }
    if (amount <= 0) {
        error = "Enter a quantity greater than zero.";
        return false;
    }
    if (amount > product->quantity) {
        error = "Only " + std::to_string(product->quantity) + " is in stock.";
        return false;
    }
    product->quantity -= amount;
    error.clear();
    return true;
}

Product* InventorySystem::findProduct(int id) {
    for (Product& product : products_) {
        if (product.id == id) {
            return &product;
        }
    }
    return nullptr;
}

const Product* InventorySystem::findProduct(int id) const {
    for (const Product& product : products_) {
        if (product.id == id) {
            return &product;
        }
    }
    return nullptr;
}

std::vector<Product> InventorySystem::search(const std::string& query) const {
    std::vector<Product> matches;
    const std::string clean = trim(query);
    int idQuery = 0;

    // A whole number searches the ID column. Any other text searches name and category.
    if (parseInt(clean, idQuery)) {
        const Product* product = findProduct(idQuery);
        if (product != nullptr) {
            matches.push_back(*product);
        }
        return matches;
    }

    for (const Product& product : products_) {
        if (containsIgnoreCase(product.name, clean) || containsIgnoreCase(product.category, clean)) {
            matches.push_back(product);
        }
    }
    return matches;
}

std::vector<Product> InventorySystem::lowStockProducts() const {
    std::vector<Product> matches;
    for (const Product& product : products_) {
        if (product.isLowStock()) {
            matches.push_back(product);
        }
    }
    return matches;
}

double InventorySystem::totalStockValue() const {
    double total = 0.0;
    for (const Product& product : products_) {
        total += product.stockValue();
    }
    return total;
}

bool InventorySystem::recordSale(const std::vector<CartItem>& cart, double discountPercent,
                                 Sale& created, std::string& error) {
    if (cart.empty()) {
        error = "Add at least one product before completing the sale.";
        return false;
    }
    if (discountPercent < 0 || discountPercent > MAX_DISCOUNT_PERCENT) {
        error = "Discount must be from 0 to " + std::to_string(MAX_DISCOUNT_PERCENT) + " percent.";
        return false;
    }

    std::vector<CartItem> merged;
    for (const CartItem& item : cart) {
        if (item.quantity <= 0) {
            error = "Quantity must be greater than zero.";
            return false;
        }
        bool found = false;
        for (CartItem& existing : merged) {
            if (existing.productId == item.productId) {
                existing.quantity += item.quantity;
                found = true;
                break;
            }
        }
        if (!found) {
            merged.push_back(item);
        }
    }

    Sale sale;
    for (const CartItem& item : merged) {
        const Product* product = findProduct(item.productId);
        if (product == nullptr) {
            error = "Product ID " + std::to_string(item.productId) + " does not exist.";
            return false;
        }
        if (item.quantity > product->quantity) {
            error = "Not enough stock for " + product->name + ". Available: " +
                    std::to_string(product->quantity) + ".";
            return false;
        }

        SaleLine line;
        line.productId = product->id;
        line.productName = product->name;
        line.quantity = item.quantity;
        line.unitPrice = product->price;
        sale.lines.push_back(line);
        sale.subtotal += line.lineTotal();
    }

    for (const CartItem& item : merged) {
        findProduct(item.productId)->quantity -= item.quantity;
    }

    sale.id = nextSaleId_++;
    sale.dateTime = nowStamp();
    sale.discountPercent = discountPercent;
    sale.discountAmount = sale.subtotal * (discountPercent / 100.0);
    sale.total = sale.subtotal - sale.discountAmount;
    sales_.push_back(sale);
    created = sale;
    error.clear();
    return true;
}

std::vector<Sale> InventorySystem::salesOn(const std::string& date) const {
    std::vector<Sale> matches;
    for (const Sale& sale : sales_) {
        if (sale.dateTime.compare(0, date.size(), date) == 0) {
            matches.push_back(sale);
        }
    }
    return matches;
}

const Sale* InventorySystem::findSale(int id) const {
    for (const Sale& sale : sales_) {
        if (sale.id == id) {
            return &sale;
        }
    }
    return nullptr;
}

std::vector<std::pair<std::string, int>> InventorySystem::topSelling(int limit) const {
    std::map<std::string, int> totals;
    for (const Sale& sale : sales_) {
        for (const SaleLine& line : sale.lines) {
            totals[line.productName] += line.quantity;
        }
    }

    std::vector<std::pair<std::string, int>> rows(totals.begin(), totals.end());
    std::sort(rows.begin(), rows.end(), [](const auto& left, const auto& right) {
        if (left.second != right.second) {
            return left.second > right.second;
        }
        return left.first < right.first;
    });
    if (static_cast<int>(rows.size()) > limit) {
        rows.resize(static_cast<std::size_t>(limit));
    }
    return rows;
}

const std::vector<Product>& InventorySystem::products() const {
    return products_;
}

const std::vector<Sale>& InventorySystem::sales() const {
    return sales_;
}

std::string InventorySystem::nowStamp() {
    const std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream out;
    out << std::put_time(&localTime, "%Y-%m-%d %H:%M");
    return out.str();
}

std::string InventorySystem::today() {
    const std::string stamp = nowStamp();
    return stamp.substr(0, 10);
}

bool InventorySystem::writeProducts() const {
    std::ofstream out(PRODUCT_FILE, std::ios::trunc);
    if (!out) {
        return false;
    }
    for (const Product& product : products_) {
        out << product.toLine() << '\n';
    }
    return static_cast<bool>(out);
}

bool InventorySystem::writeSales() const {
    std::ofstream out(SALES_FILE, std::ios::trunc);
    if (!out) {
        return false;
    }
    out << std::fixed << std::setprecision(2);
    for (const Sale& sale : sales_) {
        out << "SALE|" << sale.id << '|' << sale.dateTime << '|'
            << sale.discountPercent << '|' << sale.subtotal << '|'
            << sale.discountAmount << '|' << sale.total << '\n';
        for (const SaleLine& line : sale.lines) {
            out << "ITEM|" << line.productId << '|' << line.productName << '|'
                << line.quantity << '|' << line.unitPrice << '|' << line.lineTotal() << '\n';
        }
        out << "END\n";
    }
    return static_cast<bool>(out);
}

bool InventorySystem::loadProducts() {
    std::ifstream in(PRODUCT_FILE);
    if (!in) {
        return false;
    }

    int skipped = 0;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        Product product;
        if (!Product::fromLine(line, product)) {
            ++skipped;
            continue;
        }
        products_.push_back(product);
        if (product.id >= nextProductId_) {
            nextProductId_ = product.id + 1;
        }
    }
    if (skipped > 0) {
        std::cout << "Skipped " << skipped
                  << " damaged product line(s) in " << PRODUCT_FILE << ".\n";
    }
    return true;
}

bool InventorySystem::loadSales() {
    std::ifstream in(SALES_FILE);
    if (!in) {
        return false;
    }

    Sale current;
    bool insideSale = false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        const std::vector<std::string> parts = split(line, '|');
        if (parts.empty()) {
            continue;
        }

        if (parts[0] == "SALE" && parts.size() == 7) {
            current = Sale{};
            insideSale = parseInt(parts[1], current.id) && parseDouble(parts[3], current.discountPercent) &&
                         parseDouble(parts[4], current.subtotal) && parseDouble(parts[5], current.discountAmount) &&
                         parseDouble(parts[6], current.total);
            current.dateTime = parts[2];
            if (!insideSale) {
                current = Sale{};
            }
        } else if (parts[0] == "ITEM" && insideSale && parts.size() == 6) {
            SaleLine item;
            if (parseInt(parts[1], item.productId) && parseInt(parts[3], item.quantity) &&
                parseDouble(parts[4], item.unitPrice)) {
                item.productName = parts[2];
                current.lines.push_back(item);
            }
        } else if (parts[0] == "END" && insideSale) {
            sales_.push_back(current);
            if (current.id >= nextSaleId_) {
                nextSaleId_ = current.id + 1;
            }
            current = Sale{};
            insideSale = false;
        }
    }
    return true;
}
