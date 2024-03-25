#include "system.h"
#include "data.h"
using namespace std;

#pragma pack(push, 1)

const uint32_t EOC = 0xFFFFFFFF; // End of cluster
const uint32_t RESERVED = 0x00000001; // Reserved cluster

uint32_t findAvailableCluster(const uint32_t& currentCluster, ClusterTable* table, const bool& numFind);
int stringHash(const string& str);
uint32_t Sizeof(const vector<uint32_t>& x);
uint32_t Sizeof(const vector<pair<ShortEntry, vector<LongEntry>>>& x);
uint32_t Sizeof(const pair<ShortEntry, vector<LongEntry>>& x);
bool checkTwoHighestBits(const uint8_t& byte);
vector<uint8_t> readBinaryFile(const string& filename, const uint32_t& size, const uint32_t& idx, fstream &file);
bool writeBinaryFile(const string& filename, const vector<uint8_t>& dataSource, const uint32_t& size, const uint32_t& idx, fstream &file);

class Volume {
private:
    ClusterTable* table;
    BootSec* bootSec;
    Data* data;
    string FileSystemName; // tên file của Volume này
    uint32_t numEmptyCluster;
public:
    Volume();
    Volume(const string& fileSystemName, const string& password, fstream &file);
    ~Volume();
    void changeNumEmptyCluster(const uint32_t& numEmptyCluster);
    void setNumEmptyCluster(const uint32_t& numEmptyCluster);
    uint32_t getNumEmptyCluster();
    void setVolumeFileName(const string& volumeFileName);
    string getVolumeFileName();
    BootSec* getBootSec();
    void setBootSec(BootSec* bootSec);
    ClusterTable* getClusterTable();
    void setClusterTable(ClusterTable* newTable, Volume* vol);
    Data* getData();
    void setData(Data* data);
    uint32_t volumeLeft(); // dung lượng còn lại của volume
};

// Hàm chuyển đổi từ vector<uint8_t> sang vector<T>
template<typename T> 
vector<T> convertVectorByteToVectorT(const vector<uint8_t>& dataSource) {
    vector<T> dataDest(dataSource.size() / sizeof(T));
    memcpy(dataDest.data(), dataSource.data(), dataSource.size());
    return dataDest;
}

// Hàm chuyển đổi từ vector<uint8_t> sang T
template<typename T>
void convertVectorByteToT(const vector<uint8_t>& dataSource, T& dataDest) {
    memcpy(&dataDest, dataSource.data(), sizeof(T));
}

// Hàm chuyển đổi từ T sang vector<uint8_t>
template<typename T>
vector<uint8_t> convertTtoVectorByte(const T& dataSource) {
    vector<uint8_t> dataDest(sizeof(T));
    memcpy(dataDest.data(), reinterpret_cast<const uint8_t*>(&dataSource), sizeof(T));
    return dataDest;
}

// Hàm chuyển đổi từ vector<T> sang vector<uint8_t>
template<typename T>
vector<uint8_t> convertVectorTtoVectorByte(const vector<T>& dataSource) {
    vector<uint8_t> dataDest;
    memcpy(dataDest.data(), dataSource.data(), sizeof(T) * dataSource.size());
    return dataDest;
}

#pragma pack(pop)