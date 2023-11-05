#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <bitset>

using namespace std;

#pragma pack(2) 
// Ensure proper alignment of the BMP header

// BMP file header structure (14 bytes)
struct BMPFileHeader
{
    uint16_t type;       // Signature: "BM" (0x4D42)
    uint32_t size;       // Total file size in bytes
    uint16_t reserved1;  // Reserved (unused)
    uint16_t reserved2;  // Reserved (unused)
    uint32_t dataOffset; // Offset to the beginning of pixel data
};

// BMP information header structure (40 bytes)
struct BMPInfoHeader
{
    uint32_t headerSize;      // Size of this header (40 bytes)
    int32_t width;            // Image width in pixels
    int32_t height;           // Image height in pixels (positive for bottom-up)
    uint16_t planes;          // Number of color planes (usually 1)
    uint16_t bitCount;        // Bits per pixel (e.g., 32 for 32-bit BMP)
    uint32_t compression;     // Compression method (0 for no compression)
    uint32_t dataSize;        // Size of pixel data (can be 0 for uncompressed)
    int32_t hResolution;      // Horizontal resolution (pixels per meter)
    int32_t vResolution;      // Vertical resolution (pixels per meter)
    uint32_t colors;          // Number of colors in the color palette (usually 0 for 32-bit BMPs)
    uint32_t importantColors; // Number of important colors (usually 0)
};

// RGBA structure with integer components
struct RGBA
{
    int red;
    int green;
    int blue;
    int alpha;
    RGBA(int r, int g, int b, int a)
        : red(r), green(g), blue(b), alpha(a) {}
    // Constructor for rgba
    RGBA() : red(0), green(0), blue(0), alpha(0) {}
    // Just a comparison based operator.
    bool operator<(const RGBA &other) const
    {
        if (red != other.red)
        {
            return red < other.red;
        }
        if (green != other.green)
        {
            return green < other.green;
        }
        if (blue != other.blue)
        {
            return blue < other.blue;
        }
        return alpha < other.alpha;
    }
};
// Structure of huffmannode.
struct HuffmanNode
{
    RGBA pixel;
    int frequency;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(const RGBA &p, int freq) : pixel(p), frequency(freq), left(nullptr), right(nullptr) {}
};
// strucutre of comaring nodes of huffmannodes.
struct CompareHuffmanNode
{
    bool operator()(HuffmanNode *a, HuffmanNode *b) const
    {
        return a->frequency > b->frequency;
    }
};
// Function to bulid huffman tre
HuffmanNode *buildHuffmanTree(map<RGBA, int> &frequency_table)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, CompareHuffmanNode> pq;

    // Create leaf nodes for each unique pixel value and add them to the priority queue
    for (const auto &pair : frequency_table)
    {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    if (pq.size() == 1)
    {
        HuffmanNode *right = pq.top();
        pq.pop();
        HuffmanNode *newNode = new HuffmanNode(RGBA(-1, -1, -1, -1), right->frequency);
        newNode->right = right;
        newNode->left = nullptr;
        pq.push(newNode);
    }

    // Build the Huffman tree by combining nodes
    while (pq.size() > 1)
    {
        HuffmanNode *left = pq.top();
        pq.pop();
        HuffmanNode *right = pq.top();
        pq.pop();
        HuffmanNode *newNode = new HuffmanNode(RGBA(-1, -1, -1, -1), left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    return pq.top();
}
// This function generally writes a huffman codes for each huffman node in tree.
void generateHuffmanCodes(HuffmanNode *root, map<RGBA, string> &code_table, string currentCode)
{
    if (root)
    {
        if (root->left == nullptr && root->right == nullptr)
        {
            code_table[root->pixel] = currentCode;
            return;
        }
        if (root->left)
        {
            generateHuffmanCodes(root->left, code_table, currentCode + "0");
        }
        if (root->right)
        {
            generateHuffmanCodes(root->right, code_table, currentCode + "1");
        }
    }
}
// This function generaaly writes (pixel values -> huffman code) for decoding)
void writeHuffmanTreeToTextFile(HuffmanNode *root, ofstream &treeFile, const string &currentCode)
{
    if (!root)
    {
        return;
    }

    if (root->left == nullptr && root->right == nullptr)
    {
        // Write the pixel value and its Huffman code to the text file
        treeFile << "Pixel: " << root->pixel.red << ", " << root->pixel.green << ", " << root->pixel.blue << ", " << root->pixel.alpha << " Huffman Code: " << currentCode << endl;
    }

    // Recursively traverse the Huffman tree
    writeHuffmanTreeToTextFile(root->left, treeFile, currentCode + "0");
    writeHuffmanTreeToTextFile(root->right, treeFile, currentCode + "1");
}

// Function to write the pixel values and their Huffman codes to a binary file
void writeBinaryFile(vector<vector<RGBA>> &pixelData, map<RGBA, string> &huffmanTable, ofstream &binaryOutput)
{
    for (int y = 0; y < pixelData.size(); ++y)
    {
        for (int x = 0; x < pixelData[y].size(); ++x)
        {
            RGBA pixel = pixelData[y][x];
            string huffmanCode = huffmanTable[pixel];
            binaryOutput << huffmanCode;
            binaryOutput.put('\0');
            // Add a null character ('\0') as a delimiter after each pixel's Huffman code
        }
    }
}

int main()
{
    // Open the BMP file
    ifstream bmpFile("input.bmp", ios::binary);

    if (!bmpFile)
    {
        cerr << "Error: Cannot open input BMP file." << endl;
        return 1;
    }

    // Read the BMP file header
    BMPFileHeader fileHeader;
    bmpFile.read(reinterpret_cast<char *>(&fileHeader), sizeof(BMPFileHeader));

    if (fileHeader.type != 0x4D42)
    {
        cerr << "Error: Not a valid BMP file." << endl;
        return 1;
    }

    // Read the BMP information header
    BMPInfoHeader infoHeader;
    bmpFile.read(reinterpret_cast<char *>(&infoHeader), sizeof(BMPInfoHeader));

    // Extract the width and height
    int width = infoHeader.width;
    int height = abs(infoHeader.height); // Use abs to make it positive

    cout << "Image Width: " << width << " pixels" << endl;
    cout << "Image Height: " << height << " pixels" << endl;

    // Ensure it's a 32-bit BMP
    if (infoHeader.bitCount != 32)
    {
        cerr << "Error: This code supports 32-bit BMP only." << endl;
        return 1;
    }

    // Create an array to store pixel data as RGBA structures
    vector<vector<RGBA>> pixelData(height, vector<RGBA>(width));
    map<RGBA, int> frequency_table;
    // Read pixel data
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            RGBA pixel;
            bmpFile.read(reinterpret_cast<char *>(&pixel.blue), 1);
            bmpFile.read(reinterpret_cast<char *>(&pixel.green), 1);
            bmpFile.read(reinterpret_cast<char *>(&pixel.red), 1);
            bmpFile.read(reinterpret_cast<char *>(&pixel.alpha), 1);
            pixelData[y][x] = pixel;
            frequency_table[pixel]++;
        }
    }
    HuffmanNode *huffmanTree = buildHuffmanTree(frequency_table);

    map<RGBA, string> code_table;

    generateHuffmanCodes(huffmanTree, code_table, "");

    ofstream textTreeFile("huffman_tree.txt"); // Open the text file

    if (!textTreeFile)
    {
        cerr << "Error: Cannot open huffman_tree.txt file." << endl;
        return 1;
    }

    // Write the Huffman tree to the text file
    writeHuffmanTreeToTextFile(huffmanTree, textTreeFile, "");

    textTreeFile.close(); // Close the text file

    // Write the pixel values and their Huffman codes to a binary file
    // Open the binary output file
    ofstream binaryOutput("output.bin", ios::binary);

    if (!binaryOutput)
    {
        cerr << "Error: Cannot open output binary file." << endl;
        return 1;
    }

    // Call the modified writeBinaryFile function
    writeBinaryFile(pixelData, code_table, binaryOutput);

    binaryOutput.close();

    cout << "Huffman tree structure is written" << endl;
    cout << "Pixel data and Huffman codes are written " << endl;

    bmpFile.close();

    return 0;
}
