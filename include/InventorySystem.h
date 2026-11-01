#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H

#include "Product.h"
#include "Sale.h"

#include <string>
#include <utility>
#include <vector>

// The shop: the product list, the sales list, and the rules that change them.
class InventorySystem {
public:
    bool load();
    bool save() const;
    void seedSampleData();

    bool addProduct(const std::string& name, const std::string& category, double price,
                    int quantity, int reorderLevel, std::string& error, int& newId);
    bool updateProduct(int id, const std::string& name, const std::string& category,
                       double price, int reorderLevel, std::string& error);
    bool deleteProduct(int id, std::string& error);

    bool addStock(int id, int amount, std::string& error);
    bool removeStock(int id, int amount, std::string& error);

    Product* findProduct(int id);
    const Product* findProduct(int id) const;
    std::vector<Product> search(const std::string& query) const;
    std::vector<Product> lowStockProducts() const;
    double totalStockValue() const;

    bool recordSale(const std::vector<CartItem>& cart, double discountPercent,
                    Sale& created, std::string& error);
    std::vector<Sale> salesOn(const std::string& date) const;
    const Sale* findSale(int id) const;
    std::vector<std::pair<std::string, int>> topSelling(int limit) const;

    const std::vector<Product>& products() const;
    const std::vector<Sale>& sales() const;

    static std::string today();
    static std::string nowStamp();

private:
    bool writeProducts() const;
    bool writeSales() const;
    bool loadProducts();
    bool loadSales();

    std::vector<Product> products_;
    std::vector<Sale> sales_;
    int nextProductId_ = 1;
    int nextSaleId_ = 1;
};

#endif
