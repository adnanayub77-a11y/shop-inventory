#include "config.h"
#include "Product.h"
#include "textutil.h"

#include <sstream>
#include <iomanip>

bool Product::isLowStock() const {
    return quantity <= reorderLevel;
}

double Product::stockValue() const {
    return price * quantity;
}

std::string Product::toLine() const {
    std::ostringstream out;
    out << id << '|'
        << name << '|'
        << category << '|'
        << std::fixed << std::setprecision(2) << price << '|'
        << quantity << '|'
        << reorderLevel;
    return out.str();
}

bool Product::fromLine(const std::string& line, Product& product) {
    std::stringstream stream(line);
    std::string idText;
    std::string name;
    std::string category;
    std::string priceText;
    std::string quantityText;
    std::string reorderText;

    if (!std::getline(stream, idText, '|') ||
        !std::getline(stream, name, '|') ||
        !std::getline(stream, category, '|') ||
        !std::getline(stream, priceText, '|') ||
        !std::getline(stream, quantityText, '|') ||
        !std::getline(stream, reorderText)) {
        return false;
    }

    Product parsed;
    if (!parseInt(idText, parsed.id) || !parseDouble(priceText, parsed.price) ||
        !parseInt(quantityText, parsed.quantity) || !parseInt(reorderText, parsed.reorderLevel)) {
        return false;
    }

    parsed.name = trim(name);
    parsed.category = trim(category);
    if (parsed.id <= 0 || parsed.name.empty() || parsed.category.empty() ||
        parsed.price < 0 || parsed.quantity < 0 || parsed.reorderLevel < 0) {
        return false;
    }

    product = parsed;
    return true;
}
