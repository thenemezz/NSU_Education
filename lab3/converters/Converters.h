#pragma once
#include "../util/Util.h"

//входные потоки данных
class ParamsForConvert {
private:
    std::vector<Stream> inputFiles;
public:
    ParamsForConvert(const Stream& input1);
    Stream& operator[](size_t ind);
    const Stream& operator[](size_t ind) const;
    void pushBack(const Stream& stream);
    void reserve(size_t size);
};

//интерфейс для конвертеров
class AbstractConverter {
protected:
    size_t startSecond = 0;
    size_t endSecond = 0;
    std::vector<size_t> numberOfInputs;
public:
    virtual Stream convert(const ParamsForConvert& streams) const = 0;
    virtual void setConfiguration(const std::vector<std::string>& config) = 0;
    size_t getStartSecond() const;
    size_t getEndSecond() const;
    size_t getIndexOfInput(size_t ind) const;
    size_t getCountOfInputs() const;
    virtual ~AbstractConverter() = default;
    virtual std::string getInfo() const = 0;
};


class MuteConverter: public AbstractConverter {
public:
    Stream convert(const ParamsForConvert& streams) const override;
    void setConfiguration(const std::vector<std::string>& config) override;
    std::string getInfo() const;
};

class MixConverter: public AbstractConverter {
public:
    Stream convert(const ParamsForConvert& streams) const override;
    void setConfiguration(const std::vector<std::string>& config) override;
    std::string getInfo() const override;
};

class InsertConverter: public AbstractConverter {
public:
    Stream convert(const ParamsForConvert& streams) const override;
    void setConfiguration(const std::vector<std::string>& config) override;
    std::string getInfo() const override;
};

