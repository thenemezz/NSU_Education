#pragma once
#include "../converters/Converters.h"
#include <map>
#include <string>
#include <iostream>

class AbstractConvertCreator {
public:
    virtual ~AbstractConvertCreator() {};
    virtual AbstractConverter* create() const = 0;
};

template <class Converter>
class ConvertCreator: public AbstractConvertCreator {
public:
    AbstractConverter* create() const override {
        return new Converter();
    };
};

class ConvertersFactory {
private:
    std::map<std::string, AbstractConvertCreator*> mapOfProduct;
public:
    ConvertersFactory() = default;

    virtual ~ConvertersFactory() {};

    template <class Converter>
    void add(const std::string& id) {
        auto iter = mapOfProduct.find(id);
        if (iter == mapOfProduct.end()) {
            mapOfProduct[id] = new ConvertCreator<Converter> ();
        }
    };

    AbstractConverter* create(const std::string& id) const {
        auto iter = mapOfProduct.find(id);
        if (iter != mapOfProduct.end()) {
            return iter->second->create();
        }
        return nullptr;
    };

    std::vector<std::string> getInfoAboutProducts() {
        std::vector<std::string> info;
        for (auto iter: mapOfProduct) {
            auto product = iter.second->create();
            info.push_back(product->getInfo());
            delete product;
        }
        return info;
    }
};
