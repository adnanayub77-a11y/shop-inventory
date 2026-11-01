#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

// One row in the shop catalog. The object holds both the price and the stock on hand.
class Product {
public:
    int id = 0;
    std::string name;
    std::string category;
    double price = 0.0;
    int quantity = 0;
    int reorderLevel = 0;

    bool isLowStock() const;
    double stockValue() const;
    std::string toLine() const;
    static bool fromLine(const std::string& line, Product& product);
};

#endif
