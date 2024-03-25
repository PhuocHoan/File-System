#include "volume.h"

uint32_t findAvailableCluster(const uint32_t& currentCluster, ClusterTable* table, const bool& numFind) {
    for (uint32_t i = currentCluster; i < table->getElement().size(); i++) {
        if (table->getElement()[i] == numFind) {
            table->setElement(i, EOC); // không dùng cluster 0 va 1
            return i;
        }
    }
    return 0; // Không có cluster trống
}

// mã hóa password bằng hash
int stringHash(const string& str) {
    const int tableSize = 256;
    const double a = 0.357840;
    int hashValue = 0;
    for (char ch : str)
        hashValue = static_cast<int>(hashValue * a + ch) % tableSize;

    return hashValue;
}

void Volume::changeNumEmptyCluster(const uint32_t& numEmptyCluster) {
    this->numEmptyCluster += numEmptyCluster;
}

void Volume::setNumEmptyCluster(const uint32_t& numEmptyCluster) {
    this->numEmptyCluster = numEmptyCluster;
}

uint32_t Volume::getNumEmptyCluster() {
    return numEmptyCluster;
}

void Volume::setVolumeFileName(const string& volumeFileName) {
    this->FileSystemName = volumeFileName;
}

string Volume::getVolumeFileName() {
    return FileSystemName;
}

BootSec *Volume::getBootSec() {
    return bootSec;
}

void Volume::setBootSec(BootSec *bootSec) {
    this->bootSec = bootSec;
}

ClusterTable* Volume::getClusterTable() {
    return table;
}

void Volume::setClusterTable(ClusterTable* newTable, Volume* vol) {
    this->table = newTable;
    newTable->setDefaultElement(vol);
}

Data* Volume::getData() {
    return data;
}

void Volume::setData(Data* data) {
    this->data = data;
}

uint32_t Volume::volumeLeft() {
    return numEmptyCluster * bootSec->getSc() * 512;
}

Volume::Volume() {
    string fileSystemName, password;
    cout << "File system name: "; // tên file system cần lưu
    cin >> fileSystemName;
    if (findExistedFile(fileSystemName)) {
        fileSystemName = convertFileName(fileSystemName);
        cout << "File system name has existed\n"
            << "File system name after change: " << fileSystemName << '\n';
    }
    cout << "Set Volume Password: ";
    cin >> password;
    uint32_t volSize;
    cout << "Set volume size: ";
    cin >> volSize;
    this->setVolumeFileName(fileSystemName);
    this->setBootSec(new BootSec(volSize, password));
    uint32_t size = this->getBootSec()->getSv() * 512;
    vector<uint8_t> nullBuffer(size, '\0');
    fstream file(fileSystemName, ios::out | ios::binary);
    if (!file)
    {
        cout << "Fail to open file\n";
        return;
    }
    file.write(reinterpret_cast<const char*>(nullBuffer.data()), size);
    this->getBootSec()->writeBootSector(this, file); // viết bootsector vào volume
    setClusterTable(new ClusterTable, this);
    this->getClusterTable()->writeNewFat(this, file);
    file.close();
}

Volume::Volume(const string& fileSystemName, const string& password, fstream &file) {
    this->setVolumeFileName(fileSystemName);
    this->setBootSec(new BootSec);
    this->setBootSec(this->getBootSec()->readBootSector(this, file));
}

Volume::~Volume() {
    delete table;
    delete bootSec;
    delete data;
}

uint32_t Sizeof(const vector<uint32_t>& x) {
    return 4 * x.size();
}

uint32_t Sizeof(const vector<pair<ShortEntry, vector<LongEntry>>>& x) {
    uint32_t count = 0;
    for (auto i : x) {
        count += Sizeof(i);
    }
    return count;
}

uint32_t Sizeof(const pair<ShortEntry, vector<LongEntry>>& x) {
    uint32_t num = 1 + x.second.size();
    return 32 * num;
}

bool checkTwoHighestBits(const uint8_t& byte) {
    // Trích xuất hai bit cao nhất bằng cách dịch phải 6 bit và loại bỏ các bit dư thừa
    uint8_t highestBits = byte >> 6;

    // So sánh với giá trị mong muốn (01)
    return highestBits == 0b01;
}

vector<uint8_t> readBinaryFile(const string& filename, const uint32_t& size, const uint32_t& idx, fstream &file) {
    // size là số byte cần đọc
    // idx là vị trí byte cần đọc tính từ đầu file
    file.seekg(idx, ios::beg);
    // Đọc toàn bộ nội dung của tệp vào vector<uint8_t>
    vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        cerr << "Failed to read file: " << filename << endl;
        return vector<uint8_t>();
    }
    return buffer;
}

bool writeBinaryFile(const string& filename, const vector<uint8_t>& dataSource, const uint32_t& size, const uint32_t& idx, fstream &file) {
    // size là số byte cần ghi
    // idx là vị trí byte cần ghi tính từ đầu file
    file.seekp(idx, ios::beg);
    // Ghi nội dung từ vector<uint8_t> vào tệp
    if (!file.write(reinterpret_cast<const char*>(dataSource.data()), size)) {
        cerr << "Failed to write to file: " << filename << endl;
        return false;
    }
    return true;
}