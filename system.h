#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstring>
using namespace std;

#pragma pack(push, 1)

class BootSec;
class Volume;

class ClusterTable {
private:
    vector<uint32_t> element; // Số phần tử ClusterTable
public:
    void setDefaultElement(Volume* vol);
    void setElement(uint32_t idx, uint32_t value);
    vector<uint32_t> getElement();
    uint32_t findPositionNewFileFat(uint32_t fileSize, Volume* vol, vector<uint32_t> &clusterStoreFile, const bool& numFind, const uint32_t& StartCluster); // return startcluster cho file mới import
    void writeNewFat(Volume* vol, fstream &file);
    void readAllFat(Volume* vol, fstream &file);
};

class BootSec {
private:
    uint16_t sectorByte;
    uint8_t sc;
    uint8_t sb;
    uint8_t nf;
    uint32_t sv;
    uint32_t sf;
    uint8_t volumePassword;
    uint8_t rdetStartCluster;
    uint8_t bootSecCopy;
    uint8_t fileSystemTypeName[4]; // lưu 4 byte, tên loại tập tin là: "MyFS"
public:
    BootSec();
    BootSec(const uint32_t& volSize, const string& password); // tạo volume
    BootSec(const BootSec& other);
    void writeBootSector(Volume* vol, fstream &file);
    BootSec* readBootSector(Volume* vol, fstream &file);
    void changeVolumePassword(const string& password, Volume* vol, fstream &file);
    uint8_t getSb();
    uint8_t getBootSecCopy();
    uint8_t getSc();
    uint32_t getSv();
    uint16_t getSectorByte();
    uint8_t getNf();
    uint32_t getSf();
    uint8_t getVolumePassword();
    uint8_t getRdetStartCluster();
    uint8_t* getFileSystemTypeName();
    void setSb(const uint8_t& sb);
    void setBootSecCopy(const uint8_t& bootSecCopy);
    void setSc(const uint32_t& volSize);
    void setSv(const uint32_t& volSize);
    void setSectorByte(const uint16_t& sectorByte);
    void setNf(const uint8_t& nf);
    void setSf(const uint32_t& sf);
    void setVolumePassword(const uint8_t& volumePassword);
    void setRdetStartCluster(const uint8_t& rdetStartCluster);
    void setFileSystemTypeName();
};

#pragma pack(pop)