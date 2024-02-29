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
        return currentProduct.totalQuantity ;
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
        unordered_map<string, ComposedProduct>& composedProductsByName, unordered_map<string, Product>& productsByName, const pair<string, float> component,const int* offerDate ) {
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
    vector<pair<string, float>> quantityByNameNeededProducts;

    float GetPossibleQuantityToProduce(unordered_map<string, Product> productsByName, unordered_map<string, ComposedProduct> composedProductsByName, int offerDate) {
        float quantity = 10000000;
        for (auto& const neededProduct : quantityByNameNeededProducts) {
            float availableQuantityForProduct = 0;
            // daca nu e compus 
            if (composedProductsByName.contains(neededProduct.first)) {
                availableQuantityForProduct = composedProductsByName[neededProduct.first].GetAvailableQuantityByDate(neededProduct.first, composedProductsByName, productsByName, &offerDate);
            }
            else {
                availableQuantityForProduct = productsByName[neededProduct.first].GetAvailableQuantityByDate(productsByName[neededProduct.first], &offerDate); 
            }

            // pastram cantitatea cea mai mica permisa de unul dintre ingrediente
            quantity = quantity < (availableQuantityForProduct / neededProduct.second) ? quantity : (availableQuantityForProduct / neededProduct.second);
        }

        return quantity;
    }
};

class Solutions {
public : 
    static void A(unordered_map<string, Product>& productsByName, vector<Offer>& offers) {
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
            
                if (!expiredBatchesForCurrentOffer.empty()) {
                    for (auto& const batch : expiredBatchesForCurrentOffer) {
                        currentProduct->quantityByExpirationDate.push(batch);
                        currentProduct->totalQuantity += skippedQuantityForCurrentOffer;
                    }
                }
            }
            offersTook.push_back(OfferSatisfied(currentOffer.productName, soldQuantity));
        }
        
        for (OfferSatisfied offer : offersTook) {
            cout << offer.productName << " " << offer.quantitySold << endl;
        }
        cout << collectedMoney;
    }
    static void B(unordered_map<string, Product>& productsByName, vector<Offer>& offers, unordered_map<string, ComposedProduct>& composedProductsByName) {
        float collectedMoney = 0;

        vector<pair<int, float>> expiredBatchesForCurrentOffer;
        float skippedQuantityForCurrentOffer;
        for (const Offer& currentOffer : offers) {
            if (composedProductsByName.contains(currentOffer.productName)){
                //este produs compus

                //verificam cantitatea de pe stoc ( fara a lua in considerare data de expirare )
                float possibleQuantityToProduce = composedProductsByName[currentOffer.productName].GetPossibleQuantityToProduce(
                     productsByName, composedProductsByName, currentOffer.date);
                
                float soldQuantity = possibleQuantityToProduce > currentOffer.quantity ?  currentOffer.quantity : possibleQuantityToProduce;
                
                for (auto& const component : composedProductsByName[currentOffer.productName].quantityByNameNeededProducts) {
                    expiredBatchesForCurrentOffer = vector<pair<int, float>>();
                    skippedQuantityForCurrentOffer = 0;

                    // verificam daca lot ul din care se doreste consumatia este expirat la data vanzarii
                    if (productsByName[component.first].quantityByExpirationDate.top().first <= currentOffer.date ) {
                        //pentru comanda aceasta, scoatem loturile expirate 
                        expiredBatchesForCurrentOffer.push_back(productsByName[component.first].quantityByExpirationDate.top());
                        skippedQuantityForCurrentOffer += productsByName[component.first].quantityByExpirationDate.top().second;
                        productsByName[component.first].quantityByExpirationDate.pop();
                    }
                    
                    productsByName[component.first].totalQuantity -= soldQuantity * component.second;
                    if (productsByName[component.first].totalQuantity <= 0) {
                        productsByName.erase(productsByName[component.first].productName);
                    }
                    else {

                        int removedSoldQuantityFromStoc = 0;
                        while (removedSoldQuantityFromStoc != soldQuantity) {
                            if (soldQuantity >= removedSoldQuantityFromStoc + productsByName[component.first].quantityByExpirationDate.top().second) {
                                // luam tot lotul
                                removedSoldQuantityFromStoc += productsByName[component.first].quantityByExpirationDate.top().second;
                                productsByName[component.first].quantityByExpirationDate.pop();
                            }
                            else {
                                // nu luam tot lotul
                                float tookQuantityFromCurrentBatch = soldQuantity - removedSoldQuantityFromStoc;
                                removedSoldQuantityFromStoc += tookQuantityFromCurrentBatch;

                                // nu putem modifica pair ul din priority queue, il readaugam 
                                pair<int, float> remainingQuantityInBatch = pair<int, float>(
                                    productsByName[component.first].quantityByExpirationDate.top().first,
                                    productsByName[component.first].quantityByExpirationDate.top().second - tookQuantityFromCurrentBatch);
                                productsByName[component.first].quantityByExpirationDate.pop();
                                productsByName[component.first].quantityByExpirationDate.push(remainingQuantityInBatch);
                            }
                        }
                    }

                    if (!expiredBatchesForCurrentOffer.empty()) {
                        for (auto& const batch : expiredBatchesForCurrentOffer) {
                            productsByName[component.first].quantityByExpirationDate.push(batch);
                            productsByName[component.first].totalQuantity += skippedQuantityForCurrentOffer;
                        }
                    }
                }
                cout << currentOffer.productName << " " << soldQuantity << endl;
                collectedMoney += soldQuantity * currentOffer.price;

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
            //      stocam produsele expirate pentru comanda respectiva
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
                        // scoatem tot lotul
                        removedSoldQuantityFromStoc += currentProduct->quantityByExpirationDate.top().second;
                        currentProduct->quantityByExpirationDate.pop();
                    }
                    else {
                        // nu scoatem tot lotul
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

                if (!expiredBatchesForCurrentOffer.empty()) {
                    for (auto& const batch : expiredBatchesForCurrentOffer) {
                        currentProduct->quantityByExpirationDate.push(batch);
                        currentProduct->totalQuantity += skippedQuantityForCurrentOffer;
                    }
                }
            }
            cout << currentOffer.productName << " " << soldQuantity << endl;
        }
        cout << collectedMoney;
    }
};

class FileReader {
public :
    static void ReadFile(string fileName,
        unordered_map<string, Product>& productsByName, vector<Offer>& offers, unordered_map<string, ComposedProduct>& composedProductsByName) {
        ifstream inFile(fileName);

        // ReadProducts
            int noStocProducts;
            inFile >> noStocProducts;

            string productName;
            float productionPrice, totalQuantity;
            pair<int, float> quantityByExpirationDate;
            for (int i = 0; i < noStocProducts; i++) {
                totalQuantity = 0;
                inFile >> productName >> productionPrice >> quantityByExpirationDate.second >> quantityByExpirationDate.first;
                totalQuantity += quantityByExpirationDate.second;

                if (productsByName.contains(productName))
                {
                    productsByName[productName].quantityByExpirationDate.push(quantityByExpirationDate);
                    productsByName[productName].totalQuantity += totalQuantity;
                    continue;
                }
                Product currentProduct = Product(productName, productionPrice, totalQuantity);
                currentProduct.quantityByExpirationDate.push(quantityByExpirationDate);
                productsByName[productName] = currentProduct;
            }
        
        // ReadOffers
            int noOffers;
            inFile >> noOffers;

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
        

        // ReadComposedProducts
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

        inFile.close();
    }
};

static string FILE_NAME = "Input.txt";

int main()
{
    unordered_map<string, Product> productsByName;
    vector<Offer> offers;
    unordered_map<string, ComposedProduct> composedProductsByName;
    
    FileReader::ReadFile(FILE_NAME, productsByName, offers, composedProductsByName);


    //Solutions::A(productsByName, offers);
    Solutions::B(productsByName, offers, composedProductsByName);

/*
        Rezumat :

        A - implementat complet
            -> not spaghetti code, pretty clean

        B - implementat pentru produse compuse din produse simple
            -> spaghetti code
*/
    return 0;
}
    