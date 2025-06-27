#include <vector>
#include <fstream>

std::vector<uint8_t> readFileToByteArray(const std::string& filePath) {
	// �������� ����� � �������� ������
	std::ifstream file(filePath, std::ios::binary);

	// ��������, ��� ���� ������
	if (!file.is_open()) {
		throw std::runtime_error("Unable to open file: " + filePath);
	}

	// ����������� ��������� � ����� ����� ��� ����������� ��� �������
	file.seekg(0, std::ios::end);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// �������� ������� ������� ������� � ������ ������ �� �����
	std::vector<uint8_t> byteArray(fileSize);
	if (!file.read(reinterpret_cast<char*>(byteArray.data()), fileSize)) {
		throw std::runtime_error("Error reading file: " + filePath);
	}

	return byteArray;
}