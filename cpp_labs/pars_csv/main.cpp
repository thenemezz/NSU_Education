#include "module1.h"
#include "module2.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <iomanip>
#include <locale>


int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Правильное использование: " << argv[0] << " [input].txt [output].csv" << std::endl;
        return 1;
    }
    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];

    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName);
    if (!inputFile) {
        std::cerr << "Не удалось открыть файл: " << inputFileName << std::endl;
        return 1;
    }
    if (!outputFile) {
        std::cerr << "Не удалось создать файл: " << outputFileName << std::endl;
        return 1;
    }
    // Создаем мапу для подсчета частоты каждого слова
    std::map<std::string, unsigned> word_count;
    int total_words = 0;
    std::string tmp_str;
    //читаем файл построчно
    while(getline(inputFile, tmp_str)) {
        std::string word;
        for (const auto ch : tmp_str) {
            //проверка на буквы
            if (isalnum(ch) || ch == '-') {
                word += tolower(ch);
            } else {
                if (!word.empty()) {
                    word_count[word]++;
                    total_words++;
                    word.clear();
                }
            }
        }
        if (!word.empty()) {
            word_count[word]++;
            total_words++;
            word.clear();
        }
    }
    if (word_count.empty()) {
        std::cerr << "Input file is empty or contains no words\n";
        return EXIT_FAILURE;
    }
    //создаем вектор где будет храниится наши пары (для последующей сортировки)
    std::vector<std::pair<std::string, unsigned>>
    sortedVector(word_count.begin(), word_count.end());
    // Определяем функцию сравнения для сортировки по убыванию значения (частоты) принимает ссылки на мапу первую - мапу вторую
    auto sortByValueDescending = [](const std::pair<std::string, unsigned>& a, const std::pair<std::string, unsigned>& b) {
        return a.second > b.second;
    };

    // Сортируем вектор по убыванию значения
    sort(sortedVector.begin(), sortedVector.end(), sortByValueDescending);
    outputFile << "Word, Frequency, Frequency(%) \n";
    for (const auto& [word, count] : sortedVector) {
        double frequency = (double)(count) / (double)(total_words) * 100.0;
        outputFile << word << ", " << count << ", " << frequency << "\n";
    }

    std::cout << "Результаты сохранены в файл: " << outputFileName << std::endl;
    inputFile.close();
    outputFile.close();

    return 0;
}
