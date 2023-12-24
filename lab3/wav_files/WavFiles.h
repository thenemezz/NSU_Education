#pragma once
#include "../util/Util.h"
#include <fstream>
#include <vector>

class WavInFile {
    private:
        size_t sizeOfFile = 0; //размер файла
        size_t sizeOfData = 0; //размер данных
        size_t currentPosition = 0; //текущая позиция при чтении файла
        std::ifstream file; //поток для чтения данных из файла
        bool gotSecond = false; //флаг, указывающий, был ли уже прочитан второй файл; используется в методе updateSecond
    public:
        WavInFile(const std::string& ifile); //Конструктор, принимающий имя файла и открывающий поток для чтения из этого файла.
        void updateSecond(); //Метод для обновления текущего состояния, вызывается после чтения данных из второго файла
        size_t getSizeOfData() const; //Метод для получения размера данных
        Stream getStream(); //Метод для получения потока данных
        bool isEnd() const; //Метод для проверки, достигнут ли конец файла
};

class WavOutFile {
    private:
        size_t sizeOfData = 0; //Переменная для отслеживания размера данных при записи
        std::ofstream file; //Поток для записи данных в файл
    public:
        WavOutFile(const std::string& ofile); // Конструктор, принимающий имя файла и открывающий поток для записи в этот файл
        void setStream(const Stream& stream); //Метод для установки потока данных для записи
        ~WavOutFile(); //Деструктор, закрывает файл после завершения работы объекта
};

class StreamManager {
    private:
        std::vector<WavInFile> inFiles; //Вектор объектов WavInFile для управления входными файлами
        WavOutFile outFile; //Объект WavOutFile для управления выходным файлом
    public:
        StreamManager(const std::string& ostream); //Конструктор, принимающий имя выходного файла и открывающий поток для записи в этот файл
        size_t getNumberOfSeconds() const; //Метод для получения количества секунд данных
        void pushWavInStream(const std::string& istream); //Метод для добавления входного файла в управление
        Stream getStream(size_t ind); //Метод для получения потока данных из указанного входного файла
        void setToOutStream(const Stream& data); //Метод для установки потока данных для записи в выходной файл
        void updateSecond(); //Метод для обновления текущего состояния, вызывается после чтения данных из второго файла
};