#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>


using namespace std;








vector<char> readBMP(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        throw runtime_error("Error opening BMP file");
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        throw runtime_error("Error reading BMP file");
    }

    return buffer;
}













void compressAndWrite(const vector<char>& data, const string& outputFilename) {
    vector<unsigned char> compressedData;
    uLongf compressedSize = compressBound(data.size());

    compressedData.resize(compressedSize);
    compress(compressedData.data(), &compressedSize, reinterpret_cast<const Bytef*>(data.data()), data.size());

    ofstream outputFile(outputFilename, ios::binary);
    if (!outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize)) {
        throw runtime_error("Error writing compressed data to file");
    }

    cout << "Compression complete. Compressed size: " << compressedSize/1024 << " kilobytes." << endl;
}













int main() {
    string inputFilename = "i1.bmp";
    string outputFilename = "compressed_data.bin";

    try {
        vector<char> bmpData = readBMP(inputFilename);
        compressAndWrite(bmpData, outputFilename);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
