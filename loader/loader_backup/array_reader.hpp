#include <vector>
#include <fstream>

std::vector<uint8_t> readFileToByteArray(const std::string& filePath) {
	// Открытие файла в бинарном режиме
	std::ifstream file(filePath, std::ios::binary);

	// Проверка, что файл открыт
	if (!file.is_open()) {
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	// Перемещение указателя в конец файла для определения его размера
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// Создание вектора нужного размера и чтение данных из файла
	std::vector<uint8_t> byteArray(fileSize);
	if (!file.read(reinterpret_cast<char*>(byteArray.data()), fileSize)) {
		throw std::runtime_error("Error reading file: " + filePath);
	}

	return byteArray;
}