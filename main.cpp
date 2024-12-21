#include <iostream>
#include <unordered_map>
#include <map>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cctype>
#include <queue>
#include <fstream> // For file handling
using namespace std;

// Product class (same as before, no changes)
class Product {
private:
    int id;
    string name;
    int quantity;
    double price;
    string supplier;

public:
    Product() : id(0), name(""), quantity(0), price(0.0), supplier("") {}

    Product(int id, const string& name, int quantity, double price, const string& supplier)
        : id(id), name(name), quantity(quantity), price(price), supplier(supplier) {}

    void setId(int id) { this->id = id; }
    void setName(const string& name) { this->name = name; }
    void setQuantity(int quantity) { this->quantity = quantity; }
    void setPrice(double price) { this->price = price; }
    void setSupplier(const string& supplier) { this->supplier = supplier; }

    int getId() const { return id; }
    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    string getSupplier() const { return supplier; }

    void display() const {
        cout << setw(10) << "ID: " << id << "\n"
             << setw(10) << "Name: " << name << "\n"
             << setw(10) << "Quantity: " << quantity << "\n"
             << setw(10) << "Price: $" << fixed << setprecision(2) << price << "\n"
             << setw(10) << "Supplier: " << supplier << "\n";
    }

    string serialize() const {
        ostringstream oss;
        oss << id << "," << name << "," << quantity << "," << price << "," << supplier;
        return oss.str();
    }

    static Product deserialize(const string& line) {
        istringstream iss(line);
        string token;
        int id, quantity;
        double price;
        string name, supplier;

        getline(iss, token, ',');
        id = stoi(token);
        getline(iss, name, ',');
        getline(iss, token, ',');
        quantity = stoi(token);
        getline(iss, token, ',');
        price = stod(token);
        getline(iss, supplier, ',');

        return Product(id, name, quantity, price, supplier);
    }
    void updateQuantity(int newQuantity) { quantity = newQuantity; }

    void updatePrice(double newPrice) { price = newPrice; }

    bool isLowStock(int threshold) const { return quantity < threshold; }
    void addQuantity(int quan) { quantity += quan; }

    // Function to check if name ends with the given suffix
    bool endsWith(const string& suffix) const {
        if (suffix.size() > name.size()) return false;
        return equal(suffix.rbegin(), suffix.rend(), name.rbegin());
    }

    // Function for case-insensitive comparison of names
    bool nameCaseInsensitiveCompare(const string& nameToCompare) const {
        string nameLower = name;
        transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        string compareLower = nameToCompare;
        transform(compareLower.begin(), compareLower.end(), compareLower.begin(), ::tolower);
        return nameLower == compareLower;
    }

    bool operator<(const Product& other) const {
        return this->price < other.price; // For min-heap based on price
    }

    bool operator>(const Product& other) const {
        return this->price > other.price; // For max-heap based on price
    }
};
struct node {
    Product product;
    node* next;
    node* prev;
};

void insertionAtFront(node*& listt, Product prod) {
    node* p = new node;
    p->product = prod;
    p->next = listt;
    p->prev = nullptr;

    if (listt != nullptr) {
        listt->prev = p;
    }
    listt = p;
}

void insertionAtLast(node*& listt, Product prod) {
    node* p = new node;
    p->product = prod;
    p->next = nullptr;

    if (listt == nullptr) {
        p->prev = nullptr;
        listt = p;
    } else {
        node* q = listt;
        while (q->next != nullptr) {
            q = q->next;
        }
        q->next = p;
        p->prev = q;
    }
}

void deleteAtLast(node*& listt) {
    node* p = listt;
    if (listt == nullptr) {
        cout << "No Element to delete\n";
    } else {
        node* q = listt->next;
        while (q->next != nullptr) {
            p = q;
            q = q->next;
        }
        p->next = nullptr;
        delete q;
    }
}

void display(node* listt) {
    node* q = listt;
    while (q != nullptr) {
        cout << q->product.getName() << " ";
        q = q->next;
    }
    cout << endl;
}

// Inventory class with added file I/O functionality
class Inventory {
public:
    unordered_map<int, Product> productMap;
    vector<Product*> productList;
    map<string, Product*> productByName;
    priority_queue<Product, vector<Product>, greater<Product>> minHeap; // Min-heap (sorted by price)
    priority_queue<Product, vector<Product>, less<Product>> maxHeap; // Max-heap (sorted by price)
     void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error: Unable to open file for reading." << endl;
            return;
        }

        string line;
        while (getline(file, line)) { // Read each line
            Product prod = Product::deserialize(line);

            if (productMap.find(prod.getId()) != productMap.end()) {
                cout << "Warning: Product with ID " << prod.getId() << " already exists. Skipping duplicate.\n";
            } else {
                productMap[prod.getId()] = prod; // Add product to map
                productList.push_back(&productMap[prod.getId()]);
                productByName[prod.getName()] = &productMap[prod.getId()];
                minHeap.push(prod); // Add to min-heap
                maxHeap.push(prod); // Add to max-heap
            }
        }

        file.close();
        cout << "All products loaded from file successfully.\n";
    }
    void addProduct(Product prod) {
        if (productMap.find(prod.getId()) != productMap.end()) {
            cout << "Error: Product with ID " << prod.getId() << " already exists.\n";
            return;
        }
        productMap[prod.getId()] = prod;
        productList.push_back(&productMap[prod.getId()]);
                productByName[prod.getName()] = &productMap[prod.getId()];
                minHeap.push(prod); // Add to min-heap
                maxHeap.push(prod); // Add to max-heap
        cout << "Product added successfully.\n";
    }



    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error: Unable to open file for writing." << endl;
            return;
        }
        for (const auto& pair : productMap) {
            file << pair.second.serialize() << endl;
        }
        file.close();
        cout << "Products saved to file successfully.\n";
    }
    void displayProductsByPriceMin() {
        cout << "Products sorted by price (low to high):\n";
        priority_queue<Product, vector<Product>, greater<Product>> tempHeap = minHeap;
        while (!tempHeap.empty()) {
            tempHeap.top().display();
            tempHeap.pop();
        }
    }

    void displayProductsByPriceMax() {
        cout << "Products sorted by price (high to low):\n";
        priority_queue<Product, vector<Product>, less<Product>> tempHeap = maxHeap;
        while (!tempHeap.empty()) {
            tempHeap.top().display();
            tempHeap.pop();
        }
    }

    void displayAllProducts() const {
        cout << "All Products:\n";
        for (const auto& product : productList) {
            product->display();
        }
    }

    void searchById(int id) const {
        auto it = productMap.find(id);
        if (it != productMap.end()) {
            cout << "Product Found:\n";
            it->second.display();
        } else {
            cout << "Product with ID " << id << " not found.\n";
        }
    }

    void searchByAlphabeticalOrder() {
        sort(productList.begin(), productList.end(),
            [](Product* a, Product* b) {
                return a->getName() < b->getName();
            });
        cout << "Products sorted in alphabetical order:\n";
        for (const auto& product : productList) {
            product->display();
        }
    }

    void filterByPrice(double price) {
        priority_queue<Product, vector<Product>, greater<Product>> tempHeap = minHeap;
        cout << "Products with exact price $" << price << ":\n";
        while (!tempHeap.empty()) {
            if (tempHeap.top().getPrice() == price) {
                tempHeap.top().display();
            }
            tempHeap.pop();
        }
    }

    void filterByPriceRange(double minPrice, double maxPrice) {
        priority_queue<Product, vector<Product>, greater<Product>> tempHeap = minHeap;
        cout << "Products with price between $" << minPrice << " and $" << maxPrice << ":\n";
        while (!tempHeap.empty()) {
            if (tempHeap.top().getPrice() >= minPrice && tempHeap.top().getPrice() <= maxPrice) {
                tempHeap.top().display();
            }
            tempHeap.pop();
        }
    }

    void filterByQuantityRange(int minQuantity, int maxQuantity) {
        priority_queue<Product, vector<Product>, greater<Product>> tempHeap = minHeap;
        cout << "Products with quantity between " << minQuantity << " and " << maxQuantity << ":\n";
        while (!tempHeap.empty()) {
            if (tempHeap.top().getQuantity() >= minQuantity && tempHeap.top().getQuantity() <= maxQuantity) {
                tempHeap.top().display();
            }
            tempHeap.pop();
        }
    }

    void filterByLowStock(int threshold) {
        priority_queue<Product, vector<Product>, greater<Product>> tempHeap = minHeap;
        cout << "Products with low stock (below threshold " << threshold << "):\n";
        while (!tempHeap.empty()) {
            if (tempHeap.top().getQuantity() < threshold) {
                tempHeap.top().display();
            }
            tempHeap.pop();
        }
    }

    void searchBySuffix(const string& suffix) {
        cout << "Products ending with \"" << suffix << "\":\n";
        for (auto& product : productList) {
            if (product->endsWith(suffix)) {
                product->display();
            }
        }
    }

    void searchByCaseInsensitiveName(const string& nameToCompare) {
        cout << "Products matching (case-insensitive) \"" << nameToCompare << "\":\n";
        for (auto& product : productList) {
            if (product->nameCaseInsensitiveCompare(nameToCompare)) {
                product->display();
            }
        }
    }

};

void displayMenu() {

     cout << "===========================";
    cout << "\nInventory Management System\n";
    cout << "===========================\n";
    cout << "Menu:\n";
    cout << "1. Add Product\n";
    cout << "2. Search Product by ID\n";
    cout << "3. Display All Products\n";
    cout << "4. Display Products Sorted by Name\n";
    cout << "5. Display Products Sorted by Price (Low to High)\n";
    cout << "6. Display Products Sorted by Price (High to Low)\n";
    cout << "7. Update Product Quantity\n";
    cout << "8. Update Product Price\n";
    cout << "9. Filter Products by Exact Price\n";
    cout << "10. Filter Products by Price Range\n";
    cout << "11. Filter Products by Quantity Range\n";
    cout << "12. Filter Products by Low Stock\n";
    cout << "13. Search Products by Suffix in Name\n";
    cout << "14. Search Products by Case-Insensitive Name\n";
    cout << "15. Exit\n";
}


int main() {
    Inventory inv;
    int choice;
    string filename = "products.txt";
    inv.loadFromFile(filename);
    while (true) {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int id, quantity;
                double price;
                string name, supplier;
                cout << "Enter Product ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Product Name: ";
                getline(cin, name);
                cout << "Enter Quantity: ";
                cin >> quantity;
                cout << "Enter Price: ";
                cin >> price;
                cin.ignore();
                cout << "Enter Supplier: ";
                getline(cin, supplier);

                Product prod(id, name, quantity, price, supplier);
                inv.addProduct(prod);
                break;
            }
            case 2: {
                int id;
                cout << "Enter Product ID to search: ";
                cin >> id;
                inv.searchById(id);
                break;
            }
            case 3:
                inv.displayAllProducts();
                break;
            case 4:
                inv.searchByAlphabeticalOrder();
                break;
            case 5:
                inv.displayProductsByPriceMin();
                break;
            case 6:
                inv.displayProductsByPriceMax();
                break;
            case 7: {
                int id, newQuantity;
                cout << "Enter Product ID to update: ";
                cin >> id;
                cout << "Enter new Quantity: ";
                cin >> newQuantity;
                inv.productMap[id].updateQuantity(newQuantity);
                break;
            }
            case 8: {
                int id;
                double newPrice;
                cout << "Enter Product ID to update: ";
                cin >> id;
                cout << "Enter new Price: ";
                cin >> newPrice;
                inv.productMap[id].updatePrice(newPrice);
                break;
            }
            case 9: {
                double price;
                cout << "Enter exact price: ";
                cin >> price;
                inv.filterByPrice(price);
                break;
            }
            case 10: {
                double minPrice, maxPrice;
                cout << "Enter min price: ";
                cin >> minPrice;
                cout << "Enter max price: ";
                cin >> maxPrice;
                inv.filterByPriceRange(minPrice, maxPrice);
                break;
            }
            case 11: {
                int minQuantity, maxQuantity;
                cout << "Enter min quantity: ";
                cin >> minQuantity;
                cout << "Enter max quantity: ";
                cin >> maxQuantity;
                inv.filterByQuantityRange(minQuantity, maxQuantity);
                break;
            }
            case 12: {
                int threshold;
                cout << "Enter low stock threshold: ";
                cin >> threshold;
                inv.filterByLowStock(threshold);
                break;
            }
            case 13: {
                string suffix;
                cout << "Enter suffix to search for: ";
                cin >> suffix;
                inv.searchBySuffix(suffix);
                break;
            }
            case 14: {
                string name;
                cout << "Enter name to search (case-insensitive): ";
                cin.ignore();
                getline(cin, name);
                inv.searchByCaseInsensitiveName(name);
                break;
            }
            case 15:
                cout << "Exiting program.\n";
                inv.saveToFile(filename);
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

}
