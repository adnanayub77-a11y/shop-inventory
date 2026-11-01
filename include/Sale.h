#ifndef SALE_H
#define SALE_H

#include <string>
#include <vector>

// One product line on a bill. The name and price are copied so later price
// changes do not rewrite old receipts.
struct SaleLine {
    int productId = 0;
    std::string productName;
    int quantity = 0;
    double unitPrice = 0.0;

    double lineTotal() const {
        return unitPrice * quantity;
    }
};

// A completed sale. The vectors of these records are the sales history.
struct Sale {
    int id = 0;
    std::string dateTime;
    double discountPercent = 0.0;
    double subtotal = 0.0;
    double discountAmount = 0.0;
    double total = 0.0;
    std::vector<SaleLine> lines;
};

// What the billing screen sends to the shop before a receipt is stored.
struct CartItem {
    int productId = 0;
    int quantity = 0;
};

#endif
