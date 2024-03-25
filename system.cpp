#include "volume.h"

BootSec::BootSec() {
    setSb(2);
    setSectorByte(512);
    setNf(2);
    setRdetStartCluster(2);
    setBootSecCopy(1);
}

BootSec::BootSec(const BootSec& other) {
    sectorByte = other.sectorByte;
    sc = other.sc;
    sb = other.sb;
    nf = other.nf;
    sv = other.sv;
    sf = other.sf;
    volumePassword = other.volumePassword;
    rdetStartCluster = other.rdetStartCluster;
    bootSecCopy = other.bootSecCopy;
    memcpy(fileSystemTypeName, other.fileSystemTypeName, sizeof(fileSystemTypeName));
}

BootSec::BootSec(const uint32_t& volSize, const string& password) {
    setSb(2);
    setSectorByte(512);
    setNf(2);
    setRdetStartCluster(2);
    setBootSecCopy(1);
    setSc(volSize);
    setSv(volSize);
    setSf(volSize);
    setFileSystemTypeName();
    setVolumePassword(stringHash(password));
}

uint8_t BootSec::getSc() {
    return sc;
}

uint32_t BootSec::getSv() {
    return sv;
}

uint16_t BootSec::getSectorByte() {
    return sectorByte;
}

uint8_t BootSec::getNf() {
    return nf;
}

uint32_t BootSec::getSf() {
    return sf;
}

uint8_t BootSec::getVolumePassword() {
    return volumePassword;
}

uint8_t BootSec::getRdetStartCluster() {
    return rdetStartCluster;
}

uint8_t BootSec::getSb() {
    return sb;
}

uint8_t BootSec::getBootSecCopy() {
    return bootSecCopy;
}

uint8_t* BootSec::getFileSystemTypeName() {
    return fileSystemTypeName;
}

void BootSec::setSb(const uint8_t& sb) {
    this->sb = sb;
}

void BootSec::setBootSecCopy(const uint8_t& bootSecCopy) {
    this->bootSecCopy = bootSecCopy;
}

void BootSec::setSc(const uint32_t& volSize) {
    // volSize tinh theo Mb
    if (volSize >= 32 && volSize < 64) {
        this->sc = 1;
    }
    else if (volSize >= 64 && volSize < 128) {
        this->sc = 2;
    }
    else if (volSize >= 128 && volSize < 256) {
        this->sc = 4;
    }
    else if (volSize >= 256 && volSize <= 1024) {
        this->sc = 8;
    }
}

void BootSec::setSv(const uint32_t& volSize) {
    // sb + nf * sf + x * sc = sv
    // 2 * 4 * x / 512 + x * sc = sv - sb -> x
    this->sv = static_cast<uint32_t>(volSize * pow(1024, 2) / 512);
}

void BootSec::setSectorByte(const uint16_t& sectorByte) {
    this->sectorByte = sectorByte;
}

void BootSec::setNf(const uint8_t& nf) {
    this->nf = nf;
}

void BootSec::setSf(const uint32_t& sf) {
    this->sf = static_cast<uint32_t>((this->sv - 2) / ((float)8 / 512 + this->sc) * 4 / 512);
}

void BootSec::setVolumePassword(const uint8_t& volumePassword) {
    this->volumePassword = volumePassword;
}

void BootSec::setRdetStartCluster(const uint8_t& rdetStartCluster) {
    this->rdetStartCluster = rdetStartCluster;
}

void BootSec::setFileSystemTypeName() {
    uint8_t s[4] = { 'M', 'y', 'F', 'S' };
    for (int i = 0; i < 4; ++i) {
        fileSystemTypeName[i] = s[i];
    }
}

// ghi 2 bảng bootsector
void BootSec::writeBootSector(Volume* vol, fstream &file) {
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<BootSec>(*this), sizeof(BootSec), 0, file);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<BootSec>(*this), sizeof(BootSec), bootSecCopy * 512, file);
}

BootSec* BootSec::readBootSector(Volume* vol, fstream &file) {
    BootSec bootSec;
    convertVectorByteToT(readBinaryFile(vol->getVolumeFileName(), sizeof(BootSec), 0, file), bootSec);
    return new BootSec(bootSec);
}

vector<uint32_t> ClusterTable::getElement() {
    return element.size() == 0 ? vector<uint32_t>() : element;
}

void ClusterTable::setDefaultElement(Volume* vol) {
    // table with sf 
    element.resize(vol->getBootSec()->getSf() * 512 / 4, 0);
    element[0] = EOC;
    element[1] = EOC;
    element[element.size() - 1] = EOC;
    vol->setNumEmptyCluster(static_cast<uint32_t>(vol->getBootSec()->getSf() * (float)512 / 4 - 3)); // - 3 vì có 3 cluster không xài
}

// Hàm tìm vị trí trong fat cho file mới
uint32_t ClusterTable::findPositionNewFileFat(uint32_t fileSize, Volume* vol, vector<uint32_t> &clusterStoreFile, const bool& numFind, const uint32_t& StartCluster) {
    // fileSize tính theo cluster
    fileSize = static_cast<uint32_t>(ceil(static_cast<float>(fileSize) / (512 * vol->getBootSec()->getSc())));
    if (fileSize > vol->getNumEmptyCluster()) {
        cout << "Khong du bo nho de luu file";
        return UINT32_MAX;
    }
    uint32_t startCluster;
    if (StartCluster >= 3) {
        startCluster = findAvailableCluster(StartCluster, this, numFind);
    } else {
        startCluster = findAvailableCluster(3, this, numFind);
    }
    uint32_t currentCluster = startCluster; // bắt đầu là 3; vì cluster số 2 lưu entry theo cách riêng
    uint32_t remainingSize = fileSize - 1;
    clusterStoreFile.push_back(currentCluster);
    while (remainingSize > 0) {
        uint32_t availableCluster = findAvailableCluster(currentCluster, this, numFind);
        element[currentCluster] = availableCluster;
        currentCluster = availableCluster;
        clusterStoreFile.push_back(currentCluster);
        remainingSize--;
    }
    element[currentCluster] = EOC; // Đánh dấu cluster cuối cùng
    vol->changeNumEmptyCluster(-1 * static_cast<uint32_t>(fileSize));
    return startCluster;
}

// Ghi bảng Fat khi mới tạo volume
void ClusterTable::writeNewFat(Volume* vol, fstream &file) {
    // Bảng 1
    uint32_t position = vol->getBootSec()->getSb() * 512;
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(element[0]), sizeof(uint32_t), position, file);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(element[1]), sizeof(uint32_t), position + 4, file);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(element[element.size() - 1]), sizeof(uint32_t), position + 4 * (element.size() - 1), file);
    // Bảng 2
    position += vol->getBootSec()->getSf() * 512;
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(element[0]), sizeof(uint32_t), position, file);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(element[1]), sizeof(uint32_t), position + 4, file);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(element[element.size() - 1]), sizeof(uint32_t), position + 4 * (element.size() - 1), file);
}

void ClusterTable::readAllFat(Volume* vol, fstream &file) {
    // vị trí đọc
    uint32_t position = vol->getBootSec()->getSb();
    element = convertVectorByteToVectorT<uint32_t>(readBinaryFile(vol->getVolumeFileName(), Sizeof(element), position * 512, file));
}

void BootSec::changeVolumePassword(const string& password, Volume* vol, fstream &file) {
    this->setVolumePassword(stringHash(password));
    // tìm vị trí volume password trong bootsector để ghi lại trong cả bootsector và phần backup bootsector
    uint32_t idx = 13;
    writeBinaryFile(vol->getVolumeFileName(), {this->getVolumePassword()}, 1, idx, file);
    writeBinaryFile(vol->getVolumeFileName(), {this->getVolumePassword()}, 1, idx + 512, file);
}

void ClusterTable::setElement(uint32_t idx, uint32_t value) {
    element[idx] = value;
}