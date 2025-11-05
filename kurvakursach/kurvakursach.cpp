#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

string OpenFileDialog(const char* filter = "All Files\0*.*\0\0") {
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return string(fileName);
    }
    return "";
}

// Функция для копирования файла (простое решение)
bool copyFile(const string& from, const string& to) {
    ifstream src(from, ios::binary);
    ofstream dst(to, ios::binary);

    if (!src || !dst) return false;

    dst << src.rdbuf();
    return true;
}

// Функция для встраивания водяного знака в любой файл
void embedWatermarkToFile(const string& hostFile, const string& watermarkFile, const string& outputFile) {
    // Просто копируем основной файл и добавляем водяной знак в конец
    if (!copyFile(hostFile, outputFile)) {
        cout << "Ошибка копирования файла!" << endl;
        return;
    }

    // Добавляем водяной знак в конец файла (простой метод)
    ofstream file(outputFile, ios::binary | ios::app);
    ifstream wm(watermarkFile, ios::binary);

    if (!file || !wm) {
        cout << "Ошибка добавления водяного знака!" << endl;
        return;
    }

    // Добавляем маркер начала водяного знака
    const char* marker = "WATERMARK_START";
    file.write(marker, strlen(marker));

    // Копируем водяной знак в конец
    file << wm.rdbuf();

    cout << "Водяной знак встроен в файл!" << endl;
}

int main() {
    setlocale(LC_ALL, "rus");
    cout << "=== РЕАЛЬНЫЙ ВСТРАИВАТЕЛЬ ВОДЯНЫХ ЗНАКОВ ===" << endl;
    cout << "       Работает с ЛЮБЫМИ файлами!" << endl << endl;

    // Выбор основного файла
    cout << "Выберите основной файл (любой):" << endl;
    string hostPath = OpenFileDialog();
    if (hostPath.empty()) {
        cout << "Файл не выбран! Программа завершена." << endl;
        system("pause");
        return 0;
    }
    cout << "Основной файл: " << hostPath << endl;

    // Выбор водяного знака  
    cout << "\nВыберите файл водяного знака (любой):" << endl;
    string watermarkPath = OpenFileDialog();
    if (watermarkPath.empty()) {
        cout << "❌ Файл не выбран! Программа завершена." << endl;
        system("pause");
        return 0;
    }
    cout << "✅ Водяной знак: " << watermarkPath << endl;

    // Получаем информацию о файлах
    ifstream host(hostPath, ios::binary | ios::ate);
    ifstream wm(watermarkPath, ios::binary | ios::ate);

    if (!host || !wm) {
        cout << "❌ Ошибка открытия файлов!" << endl;
        system("pause");
        return 0;
    }

    size_t hostSize = host.tellg();
    size_t wmSize = wm.tellg();

    cout << "\nИнформация о файлах:" << endl;
    cout << "   Основной файл: " << hostSize << " байт" << endl;
    cout << "   Водяной знак: " << wmSize << " байт" << endl;

    // Создаем имя для выходного файла
    string outputPath = "watermarked_" + hostPath.substr(hostPath.find_last_of("\\") + 1);

    cout << "\nНачинаю встраивание..." << endl;

    // Встраиваем водяной знак
    embedWatermarkToFile(hostPath, watermarkPath, outputPath);

    // Проверяем результат
    ifstream result(outputPath, ios::binary | ios::ate);
    if (result) {
        size_t resultSize = result.tellg();
        cout << "Результирующий файл: " << outputPath << endl;
        cout << "Размер: " << resultSize << " байт" << endl;
        cout << "Водяной знак успешно добавлен!" << endl;
    }
    else {
        cout << "Ошибка создания результата!" << endl;
    }

    cout << "\nДля извлечения водяного знака используйте программу извлечения." << endl;
    cout << "Нажмите любую клавишу для выхода..." << endl;
    system("pause");
    return 0;
}