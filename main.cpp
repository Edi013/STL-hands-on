#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <unordered_map>

using namespace std;

class Product {
public:
    string productName;
    float productionPrice;
    int totalQuantity;
    // expiration date | quantity
    priority_queue <pair<int, float>, vector<pair<int, float>>, greater<pair<int, float>>> quantityByExpirationDate;

    float GetAvailableQuantityByDate(Product currentProduct, const int* offerDate) {

        while (!currentProduct.quantityByExpirationDate.empty()) {
            if (currentProduct.quantityByExpirationDate.top().first <= *offerDate) {
                currentProduct.totalQuantity -= currentProduct.quantityByExpirationDate.top().second;
                currentProduct.quantityByExpirationDate.pop();
                continue;
            }
            break;
        }
        /*if (currentProduct.quantityByExpirationDate.empty())
        {
            return 0;
        }*/
        return currentProduct.totalQuantity;
    }
};

class Offer {
public:
    int date;
    string productName;
    float price, quantity;
};

class OfferSatisfied {
public:
    string productName;
    int quantitySold;
};

class ComposedProduct {
    float QuantitySum(
        unordered_map<string, ComposedProduct>& composedProductsByName, unordered_map<string, Product>& productsByName, const pair<string, float> component,const int* offerDate) {
        if (productsByName.contains(component.first)){
            return productsByName[component.first].GetAvailableQuantityByDate(productsByName[component.first], offerDate);
        }

        int sum = 0;
        for (auto& comp : composedProductsByName[component.first].quantityByNameNeededProducts) {
            sum += QuantitySum(composedProductsByName, productsByName, comp, offerDate);
        }
        return sum;
    }

    float GetAvailableQuantityByDate(string currentProductName, unordered_map<string, ComposedProduct> composedProductsByName, unordered_map<string, Product> productsByName, int* offerDate) {
        float quantity = 0;
        for (auto const component : composedProductsByName[currentProductName].quantityByNameNeededProducts) {
            quantity += QuantitySum(composedProductsByName, productsByName, component, offerDate);
        }
        return quantity;
    }
public:
    string productName;
    // productName | quantity
    vector<pair<string, float>> quantityByNameNeededProducts;

    float GetPricePerUnit(unordered_map<string, Product> productsByName) {
        float price = 0;
        for (auto& const neededProduct : quantityByNameNeededProducts) {
            price += neededProduct.second * (productsByName[neededProduct.first].productionPrice);
        }
        return price;
    }

    float GetPossibleQuantityToProduce(unordered_map<string, Product> productsByName, unordered_map<string, ComposedProduct> composedProductsByName, int offerDate) {
        float quantity = 0;
        for (auto& const neededProduct : quantityByNameNeededProducts) {
            float availableQuantityForProduct = 0;
            // daca nu e compus 
            if (composedProductsByName.contains(neededProduct.first)) {
                quantity += availableQuantityForProduct = composedProductsByName[neededProduct.first].GetAvailableQuantityByDate(neededProduct.first, composedProductsByName, productsByName, &offerDate);
            }
            else {
                quantity += availableQuantityForProduct = productsByName[neededProduct.first].GetAvailableQuantityByDate(productsByName[neededProduct.first], &offerDate);
            }

        }

        return quantity;
    }
};

class Solutions {
public : 
    void A(unordered_map<string, Product> productsByName, vector<Offer> offers) {
        vector<OfferSatisfied> offersTook;
        float collectedMoney = 0;
        
        vector<pair<int, float>> expiredBatchesForCurrentOffer;
        float skippedQuantityForCurrentOffer;
        for (const Offer& currentOffer : offers) {
            if (!productsByName.contains(currentOffer.productName)) {
                continue;
            }

            Product* currentProduct = &productsByName[currentOffer.productName];
            if (currentProduct->totalQuantity <= 0 || currentOffer.price < currentProduct->productionPrice) {
                continue;
            }
            
            //  trebuie verificata data de expirare la fiecare lot
            //      stocam produsele expirate pt comanda respectiva
            //          cele care se potrivesc, dpdv al datei de expirare, raman in priority_queue si sunt procesate;
            expiredBatchesForCurrentOffer = vector<pair<int, float>>();
            skippedQuantityForCurrentOffer = 0;
            while (!currentProduct->quantityByExpirationDate.empty()) {
                if (currentProduct->quantityByExpirationDate.top().first <= currentOffer.date) {
                    expiredBatchesForCurrentOffer.push_back(currentProduct->quantityByExpirationDate.top());
                    skippedQuantityForCurrentOffer += currentProduct->quantityByExpirationDate.top().second;

                    currentProduct->totalQuantity -= currentProduct->quantityByExpirationDate.top().second;
                    currentProduct->quantityByExpirationDate.pop();
                    continue;
                }
                break;
            }
            if (currentProduct->quantityByExpirationDate.empty())
            {
                for (auto& const batch : expiredBatchesForCurrentOffer) {
                    currentProduct->quantityByExpirationDate.push(batch);
                    currentProduct->totalQuantity += skippedQuantityForCurrentOffer;
                }
                continue;
            }

            int soldQuantity;
            if (currentOffer.quantity <= currentProduct->totalQuantity) {
                // avem cat e nevoie
                soldQuantity = currentOffer.quantity;
            }
            else {
                // vindem doar cat avem
                soldQuantity = currentProduct->totalQuantity;
            }
            
            // primim banii.
            collectedMoney += soldQuantity * currentOffer.price;

            // reducem cantitatea din stoc 
            currentProduct->totalQuantity -= soldQuantity;
            if (currentProduct->totalQuantity == 0 ) {
                if(skippedQuantityForCurrentOffer == 0){
                    productsByName.erase(currentProduct->productName);
                }
            }
            else {
                // scoatem din coada de prioritati ce e pe 0;
                int removedSoldQuantityFromStoc = 0;
                while (removedSoldQuantityFromStoc !=  soldQuantity) {
                    if (soldQuantity >= removedSoldQuantityFromStoc + currentProduct->quantityByExpirationDate.top().second) {
                        // luam tot lotul
                        removedSoldQuantityFromStoc += currentProduct->quantityByExpirationDate.top().second;
                        currentProduct->quantityByExpirationDate.pop();
                    }
                    else {
                        // nu luam tot lotul
                        float tookQuantityFromCurrentBatch = soldQuantity - removedSoldQuantityFromStoc;
                        removedSoldQuantityFromStoc += tookQuantityFromCurrentBatch;
                        
                        // nu putem modifica pair ul din priority queue, il readaugam 
                        pair<int, float> remainingQuantityInBatch = pair<int, float>(
                                currentProduct->quantityByExpirationDate.top().first,
                                currentProduct->quantityByExpirationDate.top().second - tookQuantityFromCurrentBatch);
                        currentProduct->quantityByExpirationDate.pop();
                        currentProduct->quantityByExpirationDate.push(remainingQuantityInBatch);
                    }
                }
            
                // re-adaugam produsele expirate pentru oferta curenta
                if (!expiredBatchesForCurrentOffer.empty()) {
                    for (auto& const batch : expiredBatchesForCurrentOffer) {
                        currentProduct->quantityByExpirationDate.push(batch);
                        currentProduct->totalQuantity += skippedQuantityForCurrentOffer;

                    }
                }
            }

            // adaugam of
            offersTook.push_back(OfferSatisfied(currentOffer.productName, soldQuantity));
        }
        
        for (OfferSatisfied offer : offersTook) {
            cout << offer.productName << " " << offer.quantitySold << endl;
        }
        cout << collectedMoney;
    }
    void B(unordered_map<string, Product> productsByName, vector<Offer> offers, unordered_map<string, ComposedProduct> composedProductsByName) {
        vector<OfferSatisfied> offersTook;
        float collectedMoney = 0;

        vector<pair<int, float>> expiredBatchesForCurrentOffer;
        float skippedQuantityForCurrentOffer;
        for (const Offer& currentOffer : offers) {
            if (composedProductsByName.contains(currentOffer.productName)){
                //este produs compus

                //verificam cantitatea de pe stoc

                continue;
            }
            // nu e produs compus, ramane implementarea de la A

            if (!productsByName.contains(currentOffer.productName)) {
                continue;
            }

            Product* currentProduct = &productsByName[currentOffer.productName];
            if (currentProduct->totalQuantity <= 0 || currentOffer.price < currentProduct->productionPrice) {
                continue;
            }

            //  trebuie verificata data de expirare la fiecare lot
            //      stocam produsele expirate pt comanda respectiva
            //          cele care se potrivesc, dpdv al datei de expirare, raman in priority_queue si sunt procesate;
            expiredBatchesForCurrentOffer = vector<pair<int, float>>();
            skippedQuantityForCurrentOffer = 0;
            while (!currentProduct->quantityByExpirationDate.empty()) {
                if (currentProduct->quantityByExpirationDate.top().first <= currentOffer.date) {
                    expiredBatchesForCurrentOffer.push_back(currentProduct->quantityByExpirationDate.top());
                    skippedQuantityForCurrentOffer += currentProduct->quantityByExpirationDate.top().second;

                    currentProduct->totalQuantity -= currentProduct->quantityByExpirationDate.top().second;
                    currentProduct->quantityByExpirationDate.pop();
                    continue;
                }
                break;
            }
            if (currentProduct->quantityByExpirationDate.empty())
            {
                for (auto& const batch : expiredBatchesForCurrentOffer) {
                    currentProduct->quantityByExpirationDate.push(batch);
                    currentProduct->totalQuantity += skippedQuantityForCurrentOffer;
                }
                continue;
            }

            int soldQuantity;
            if (currentOffer.quantity <= currentProduct->totalQuantity) {
                // avem cat e nevoie
                soldQuantity = currentOffer.quantity;
            }
            else {
                // vindem doar cat avem
                soldQuantity = currentProduct->totalQuantity;
            }

            // primim banii.
            collectedMoney += soldQuantity * currentOffer.price;

            // reducem cantitatea din stoc 
            currentProduct->totalQuantity -= soldQuantity;
            if (currentProduct->totalQuantity == 0) {
                if (skippedQuantityForCurrentOffer == 0) {
                    productsByName.erase(currentProduct->productName);
                }
            }
            else {
                // scoatem din coada de prioritati ce e pe 0;
                int removedSoldQuantityFromStoc = 0;
                while (removedSoldQuantityFromStoc != soldQuantity) {
                    if (soldQuantity >= removedSoldQuantityFromStoc + currentProduct->quantityByExpirationDate.top().second) {
                        // luam tot lotul
                        removedSoldQuantityFromStoc += currentProduct->quantityByExpirationDate.top().second;
                        currentProduct->quantityByExpirationDate.pop();
                    }
                    else {
                        // nu luam tot lotul
                        float tookQuantityFromCurrentBatch = soldQuantity - removedSoldQuantityFromStoc;
                        removedSoldQuantityFromStoc += tookQuantityFromCurrentBatch;

                        // nu putem modifica pair ul din priority queue, il readaugam 
                        pair<int, float> remainingQuantityInBatch = pair<int, float>(
                            currentProduct->quantityByExpirationDate.top().first,
                            currentProduct->quantityByExpirationDate.top().second - tookQuantityFromCurrentBatch);
                        currentProduct->quantityByExpirationDate.pop();
                        currentProduct->quantityByExpirationDate.push(remainingQuantityInBatch);
                    }
                }

                // re-adaugam produsele expirate pentru oferta curenta
                if (!expiredBatchesForCurrentOffer.empty()) {
                    for (auto& const batch : expiredBatchesForCurrentOffer) {
                        currentProduct->quantityByExpirationDate.push(batch);
                        currentProduct->totalQuantity += skippedQuantityForCurrentOffer;

                    }
                }
            }

            // adaugam of
            offersTook.push_back(OfferSatisfied(currentOffer.productName, soldQuantity));
        }

        for (OfferSatisfied offer : offersTook) {
            cout << offer.productName << " " << offer.quantitySold << endl;
        }
        cout << collectedMoney;
    }
};

static string FILE_NAME = "Input.txt";

int main()
{
    ifstream inFile(FILE_NAME);

    unordered_map<string, Product> productsByName;

    int noStocProducts;
    inFile >> noStocProducts;
    
    string productName;
    float productionPrice, totalQuantity;
    pair<int, float> quantityByExpirationDate;
    for (int i = 0; i < noStocProducts; i++) {
        totalQuantity = 0;
        inFile >> productName >> productionPrice >> quantityByExpirationDate.second >> quantityByExpirationDate.first;
        totalQuantity += quantityByExpirationDate.second;

        if(productsByName.contains(productName))
        {
            productsByName[productName].quantityByExpirationDate.push(quantityByExpirationDate);
            productsByName[productName].totalQuantity += totalQuantity;
            continue;
        }
        Product currentProduct = Product(productName, productionPrice, totalQuantity);
        currentProduct.quantityByExpirationDate.push(quantityByExpirationDate);
        productsByName[productName] = currentProduct;
    }

    int noOffers;
    inFile >> noOffers;

    vector<Offer> offers;
    for (int i = 0; i < noOffers; i++) {
        int offerDate, offerNoProducts;
        inFile >> offerDate >> offerNoProducts;

        for (int j = 0; j < offerNoProducts; j++) {
            Offer currentOffer = Offer();
            currentOffer.date = offerDate;
            inFile >> currentOffer.productName >> currentOffer.price >> currentOffer.quantity;

            offers.push_back(currentOffer);
        }
    }


    unordered_map<string, ComposedProduct> composedProductsByName;
    int noComposedProducts;
    inFile >> noComposedProducts;

    ComposedProduct currentProduct;
    for (int i = 0; i < noComposedProducts; i++) {
        currentProduct = ComposedProduct();
        inFile >> currentProduct.productName;
       
        int productsNeededForComposedProduct;
        inFile >> productsNeededForComposedProduct;

        pair<string, float> neededProduct;
        for (int j = 0; j < productsNeededForComposedProduct; j++) {
            inFile >> neededProduct.first >> neededProduct.second;
            currentProduct.quantityByNameNeededProducts.push_back(neededProduct);
        }

        composedProductsByName[currentProduct.productName] = currentProduct;
    }

    Solutions solutions = Solutions();
    solutions.A(productsByName, offers);
    //solutions.B(productsByName, offers, composedProductsByName);

    return 0;
}