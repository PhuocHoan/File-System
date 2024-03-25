#include "volume.h"
#include <cctype>

void setClusterEntry(Volume* vol) {
    // Đọc fat xong trước rồi mới set
    set<uint32_t> clusterEntry;
    clusterEntry.insert(2);
    vol->getData()->assignClusterEntry(clusterEntry);
    for (int i = 2; i < vol->getClusterTable()->getElement().size(); ++i) {
        if (vol->getClusterTable()->getElement()[i] != EOC) {
            clusterEntry.insert(vol->getClusterTable()->getElement()[i]);
            i = vol->getClusterTable()->getElement()[i];
            vol->getData()->assignClusterEntry(clusterEntry);
        }
        else {
            return;
        }
    }
}

uint32_t getLastElementInSet(const set<uint32_t>& mySet) {
    return mySet.size() == 0 ? -1 : *mySet.rbegin();
}

pair<Time, Date> getDayTimeToday() {
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);

    // Chuyển đổi sang thời gian địa phương
    tm localTime;
    localtime_s(&localTime, &now_c);

    // Lấy các thành phần riêng lẻ
    Time time(localTime.tm_sec, localTime.tm_min, localTime.tm_hour);
    Date date(localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900 - 2024); // kết quả + 2024 = năm cần tìm

    return make_pair(time, date);
}

// Hàm để bật nhiều bit tại các vị trí cụ thể trong một byte
uint8_t setBits(uint8_t byte, const vector<uint32_t>& positions) {
    // Duyệt qua các vị trí cần bật bit và thực hiện phép OR bit để kết hợp giá trị của chúng
    for (int position : positions) {
        byte |= (1 << position);
    }
    return byte;
}

// tính theo byte
uint32_t getFileSize(const string &fileName) {
    fstream file(fileName, ios::in | ios::binary | ios::ate);
    if (!file) {
        return UINT32_MAX;
    }
    // Lấy vị trí hiện tại (kích thước của tập tin)
    streamsize file_size = file.tellg();
    file.close();

    // Kiểm tra nếu kích thước tập tin vượt quá giới hạn của uint32_t
    if (file_size > UINT32_MAX) {
        cout << "File size exceeds the limit of uint32_t\n";
        return UINT32_MAX;
    }

    // Trả về kích thước của tập tin dưới dạng uint32_t
    return static_cast<uint32_t>(file_size);
}

// check xem file đã tồn tại chưa
bool findExistedFile(const string& fileName) {
    uint32_t size = getFileSize(fileName);
    return size < UINT32_MAX && size > 0 ? true : false;
}

// Hàm đổi tên file nếu bị trùng tên file
string convertFileName(const string& fileName) {
    string base, ext;
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != string::npos) {
        base = fileName.substr(0, dotPos);
        ext = fileName.substr(dotPos);
    }
    int counter = 1;
    string newFilename = fileName;
    while (findExistedFile(newFilename)) {
        ostringstream oss;
        oss << base << counter << ext;
        newFilename = oss.str();
        counter++;
    }
    return newFilename;
}

uint8_t Time::getSecond() {
    return second;
}

uint8_t Time::getMinute() {
    return minute;
}

uint8_t Time::getHour() {
    return hour;
}

uint8_t Date::getDay() {
    return day;
}

uint8_t Date::getMonth() {
    return month;
}

uint8_t Date::getYear() {
    return year;
}

uint8_t ShortEntry::getReserve() {
    return reserve;
}

uint8_t* ShortEntry::getName() {
    return name;
}

uint8_t ShortEntry::getAttribute() {
    return attribute;
}

uint8_t ShortEntry::getPassword() {
    return password;
}

void ShortEntry::setToNull() {
    for (int i = 0; i < 9; ++i) {
        name[i] = 0xE5;
    }
    attribute = 0xE5;
    password = 0xE5;
    createTime.setHour(0xE5);
    createTime.setMinute(0xE5);
    createTime.setSecond(0xE5);
    createDay.setDay(0xE5);
    createDay.setMonth(0xE5);
    createDay.setYear(0xE5);
    accessTime.setHour(0xE5);
    accessTime.setMinute(0xE5);
    accessTime.setSecond(0xE5);
    accessDay.setDay(0xE5);
    accessDay.setMonth(0xE5);
    accessDay.setYear(0xE5);
    startCluster = 0xE5;
    fileSize = 0xE5;
    reserve = 0xE5;
}

Time ShortEntry::getCreateTime() {
    return createTime;
}

Time ShortEntry::getAccessTime() {
    return accessTime;
}

Date ShortEntry::getCreateDay() {
    return createDay;
}

Date ShortEntry::getAccessDay() {
    return accessDay;
}

uint32_t ShortEntry::getStartCluster() {
    return startCluster;
}

uint32_t ShortEntry::getFileSize() {
    return fileSize;
}

uint8_t LongEntry::getIndex() {
    return index;
}

uint8_t* LongEntry::getFileName() {
    return fileName;
}

void LongEntry::setToNull() {
    index = 0xE5;
    for (int i = 0; i < 30; ++i) {
        fileName[i] = 0xE5;
    }
    reserve = 0xE5;
}

vector<pair<ShortEntry, vector<LongEntry>>> Data::getEntries() {
    return entries;
}

void ShortEntry::setAttribute() {
    cout << "Enter attribute\n";
    vector<uint32_t> attribute;
    bool ok = 0;
    cout << "Readonly(Y=1|N=0): ";
    cin >> ok;
    if (ok) {
        attribute.push_back(0);
        ok = 0;
    }
    cout << "Hidden(Y=1|N=0): ";
    cin >> ok;
    if (ok) {
        attribute.push_back(1);
        ok = 0;
    }
    cout << "Archive(Y=1|N=0): ";
    cin >> ok;
    if (ok) {
        attribute.push_back(5);
    }
    this->attribute = setBits(0, attribute);
}

void ShortEntry::setPassword(const string& password) {
    this->password = stringHash(password);
}

void ShortEntry::setCreateTime(const Time& createTime) {
    this->createTime = createTime;
}

void ShortEntry::setAccessTime(const Time& accessTime) {
    this->accessTime = accessTime;
}

void ShortEntry::setCreateDay(const Date& createDay) {
    this->createDay = createDay;
}

void ShortEntry::setAccessDay(const Date& accessDay) {
    this->accessDay = accessDay;
}

void ShortEntry::setStartCluster(const uint32_t& startCluster) {
    this->startCluster = startCluster;
}

void ShortEntry::setFileSize(const uint32_t& fileSize) {
    this->fileSize = fileSize;
}

void ShortEntry::setName(uint8_t* name) {
    for (int i = 0; i < 9; ++i) {
        this->name[i] = name[i];
    }
}

void ShortEntry::setReserve(const uint8_t& c) {
    reserve = c;
}

void Time::setSecond(const uint8_t& second) {
    this->second = second;
}

void Time::setMinute(const uint8_t& minute) {
    this->minute = minute;
}

void Time::setHour(const uint8_t& hour) {
    this->hour = hour;
}

void Date::setDay(const uint8_t& day) {
    this->day = day;
}

void Date::setMonth(const uint8_t& month) {
    this->month = month;
}

void Date::setYear(const uint8_t& year) {
    this->year = year;
}

void LongEntry::setIndex(const uint8_t& index) {
    this->index = index;
}

void LongEntry::setFileName(uint8_t* fileName) {
    for (int i = 0; i < 30; ++i) {
        this->fileName[i] = fileName[i];
    }
}

void LongEntry::setReserve(const uint8_t& c) {
    reserve = c;
}

vector<uint8_t> Data::importFile(const string& fileName) {
    fstream file(fileName, ios::in | ios::binary);
    if (!file)
    {
        cout << "Fail to open file\n";
        return vector<uint8_t>();
    }
    return readBinaryFile(fileName, getFileSize(fileName), 0, file);
}

uint8_t* Data::convertFileNameToShortName(string& fileName) {
    const int size = 9;
    uint8_t* name = new uint8_t[size]{ '\0' };
    string base, ext;
    size_t dotPos = fileName.find_last_of('.');
    int counter = 1;
    if (findExistedFileEntry(fileName)) {
        // thêm số vào tên file nếu trùng tên file
        if (dotPos != string::npos) {
            base = fileName.substr(0, dotPos);
            ext = fileName.substr(dotPos);
        }
        string newFilename = fileName;
        while (findExistedFileEntry(newFilename)) {
            ostringstream oss;
            oss << base << counter << ext;
            newFilename = oss.str();
            counter++;
        }
        fileName = newFilename;
        cout << "Because fileName have existed, fileName change to: " << fileName << '\n';
    }
    fileNames.insert({ fileName, static_cast<uint32_t>(entries.size()) });
    counter = 0;
    // Tìm số file trùng tên đã tồn tại
    dotPos = fileName.find_last_of('.');
    if (dotPos != string::npos) {
        base = fileName.substr(0, dotPos);
        ext = fileName.substr(dotPos + 1);
    }
    if (fileName.size() > 9) {
        // Nếu fileName.size() > 9, chuyển đổi thành tên ngắn
        for (auto it : fileNames) {
            size_t found = it.first.find(base);
            if (found != string::npos && it.first.substr(found + base.length()) == ext) {
                counter++;
            }
        }
        ostringstream oss;
        oss << base.substr(0, 4 - counter / 10) << "~" << counter << ext;
        fileName = oss.str();
        for (int i = 0; i < fileName.size(); ++i) {
            name[i] = toupper(fileName[i]);
        }
    }
    else {
        for (int i = 0; i < ext.size(); ++i) {
            name[size - 1 - i] = toupper(ext[ext.size() - i - 1]);
        }
        for (int i = 0; i < base.size(); ++i) {
            name[i] = toupper(fileName[i]);
        }
        for (int i = base.size(); i < fileName.size() - ext.size(); ++i) {
            name[i] = '\0';
        }
    }
    return name;
}

// Kiểm tra xem filename này đã có trong volume chưa
bool Data::findExistedFileEntry(const string& fileName) {
    return fileNames.find(fileName) != fileNames.end() ? true : false;
}

void Data::readAllEntry(const string& fileName, Volume* vol, fstream &file) {
    // Đọc fat trước
    uint32_t timeToRead1Cluster = vol->getBootSec()->getSc() * 512 / 32; // đọc 1 lần 32 byte trong 1 cluster
    for (auto it : clusterEntry) {
        uint32_t idx = (vol->getBootSec()->getSb() + vol->getBootSec()->getNf() * vol->getBootSec()->getSf() + it * vol->getBootSec()->getSc()) * 512;
        for (uint32_t i = 0; i < timeToRead1Cluster; ++i) { // Chuyển đổi kiểu dữ liệu của biến i thành uint32_t
            pair<ShortEntry, vector<LongEntry>> entry;
            convertVectorByteToT<ShortEntry>(readBinaryFile(fileName, 32, idx, file), entry.first);
            if (entry.first.getStartCluster() == '\0') {
                return;
            }
            // Kiểm tra nếu các entry tiếp theo đều có kí tự 0xE5 thì vẫn cho vào 1 entry thôi
            if (entry.first.getAttribute() == 0xE5) {
                while (true) {
                    idx += 32;
                    LongEntry Long;
                    convertVectorByteToT<LongEntry>(readBinaryFile(fileName, 32, idx, file), Long);
                    entry.second.push_back(Long);
                    ++i;
                    if (Long.getIndex() != 0xE5) {
                        break;
                    }
                }
                entries.push_back(entry);
                continue;
            }
            while (true) {
                idx += 32;
                LongEntry Long;
                convertVectorByteToT<LongEntry>(readBinaryFile(fileName, 32, idx, file), Long);
                entry.second.push_back(Long);
                if (checkTwoHighestBits(Long.getIndex())) {
                    // đây là entry phụ cuối cùng
                    break;
                }
                ++i;
            }
            entries.push_back(entry);
            idx += 32;
            // Cần check xem là khi xóa vĩnh viễn thì tất cả entry của tập tin đó lưu thành '0xE5' vì vậy nên không lưu entry bị xóa vĩnh viễn vào entries
        }
    }
    updateFileNames();
}

vector<uint32_t> Data::writeEntry(string fileName, Volume* vol, const string& password, fstream &file, uint32_t fileSizeWrite) {
    pair<ShortEntry, vector<LongEntry>> entry;
    string fileName1 = fileName;
    entry.first.setName(convertFileNameToShortName(fileName));
    entry.first.setAttribute();
    entry.first.setPassword(password);
    pair<Time, Date> dataTime = getDayTimeToday();
    entry.first.setCreateTime(dataTime.first);
    entry.first.setAccessTime(dataTime.first);
    entry.first.setCreateDay(dataTime.second);
    entry.first.setAccessDay(dataTime.second);
    entry.first.setFileSize(fileSizeWrite);
    entry.first.setStartCluster(0);
    entry.first.setReserve('\0');
    vector<uint32_t> clusterStoreFile;
    uint32_t position = vol->getClusterTable()->findPositionNewFileFat(fileSizeWrite, vol, clusterStoreFile, 0, 0);
    if (position != UINT32_MAX) {
        entry.first.setStartCluster(position);
    }
    else {
        cout << "Volume storage is not enough to store new file";
        return vector<uint32_t>();
    }
    size_t fileNameSize = fileName1.size();
    uint8_t idx = 1;
    if (fileNameSize > 10) {
        while (fileNameSize >= 0) {
            uint8_t fName[30]{ '\0' };
            LongEntry lEntry;
            lEntry.setIndex(idx);
            lEntry.setReserve('\0');
            if (fileNameSize <= 30) {
                for (int j = 0; j < fileNameSize; ++j) {
                    fName[j] = fileName1[j];
                }
                lEntry.setFileName(fName);
                entry.second.push_back(lEntry);
                break;
            }
            for (int j = 0; j < 30; ++j) {
                fName[j] = fileName1[j + 30 * (idx - 1)];
            }
            fileNameSize -= 30;
            lEntry.setFileName(fName);
            entry.second.push_back(lEntry);
            ++idx;
        }
    }
    else {
        LongEntry lEntry;
        lEntry.setIndex(idx);
        uint8_t fName[30]{ '\0' };
        for (int i = 0; i < fileNameSize; ++i) {
            fName[i] = fileName1[i];
        }
        lEntry.setFileName(fName);
        entry.second.push_back(lEntry);
    }
    idx = setBits(idx, { 6 });
    // set byte đầu của entry phụ cuối cùng có 2 bit cao nhất là: 01......
    entry.second[entry.second.size() - 1].setIndex(idx);
    entries.push_back(entry);
    // Tìm vị trí trong volume để lưu entry
    // bắt đầu tại cluster 2
    // check xem còn đủ chỗ để lưu entry tiếp theo không, không thì tìm cluster khác.
    uint32_t NumOfClusterNeedToFind = static_cast<uint32_t>(floor((float)Sizeof(entries) / (512 * vol->getBootSec()->getSc())) + 1 - clusterEntry.size()),
        lastElementInSet = getLastElementInSet(clusterEntry);
    // set cluster cuối của entry = EOC
    if (vol->getClusterTable()->getElement()[lastElementInSet] != EOC) {
        vol->getClusterTable()->setElement(lastElementInSet, EOC);
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[lastElementInSet]), sizeof(uint32_t), vol->getBootSec()->getSb() * 512 + lastElementInSet * 4, file);
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[lastElementInSet]), sizeof(uint32_t), (vol->getBootSec()->getSb() + vol->getBootSec()->getSf()) * 512 + lastElementInSet * 4, file);
    }
    if (NumOfClusterNeedToFind == 1) {
        uint32_t clusterToStore = findAvailableCluster(lastElementInSet + 1, vol->getClusterTable(), 0);
        vol->getClusterTable()->setElement(lastElementInSet, EOC);
        clusterEntry.insert(clusterToStore);
        // ghi vào volume vị trí bảng fat, vị trí cluster lưu entry đã thay đổi.
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[lastElementInSet]), sizeof(uint32_t), vol->getBootSec()->getSb() * 512 + lastElementInSet * 4, file);
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[lastElementInSet]), sizeof(uint32_t), (vol->getBootSec()->getSb() + vol->getBootSec()->getSf()) * 512 + lastElementInSet * 4, file);
        vol->changeNumEmptyCluster(-1);
        lastElementInSet = clusterToStore;
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[lastElementInSet]), sizeof(uint32_t), vol->getBootSec()->getSb() * 512 + lastElementInSet * 4, file);
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[lastElementInSet]), sizeof(uint32_t), (vol->getBootSec()->getSb() + vol->getBootSec()->getSf()) * 512 + lastElementInSet * 4, file);
    }
    // viết entry vào cluster mới tìm
    uint32_t bytePositionToWrite = (vol->getBootSec()->getSb() + vol->getBootSec()->getNf() * vol->getBootSec()->getSf() + lastElementInSet * vol->getBootSec()->getSc()) * 512 + Sizeof(entries) - Sizeof(entry);
    file.seekp(bytePositionToWrite, ios::beg);
    file.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
    for (uint32_t i = 1; i <= entry.second.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&entry.second[i - 1]), sizeof(entry.second[i - 1]));
    }
    return clusterStoreFile;
}

void Data::updateFileNames() {
    for (int i = 0; i < entries.size(); ++i) {
        if (entries[i].first.getAttribute() == 0xE5) {
            // file đã bị xóa vĩnh viễn
            continue;
        }
        string fileName = "";
        for (auto j : entries[i].second) {
            uint8_t* name = j.getFileName();
            for (int k = 0; k < 30; ++k) {
                if (name[k] >= 21 && name[k] <= 126) {
                    fileName += name[k];
                }
                else {
                    break;
                }
            }
        }
        fileNames.insert({ fileName, i });
    }
}

void Data::writeFile(string fileName, Volume* vol, const string& password, fstream &file) {
    // Import file
    // check nếu file đưa vào đã tồn tại thì có thể đổi tên file và đưa vào tiếp
    vector<uint8_t> content = importFile(fileName);
    uint32_t dataSize = content.size();
    if (fileNames.find(fileName) != fileNames.end()) {
        string base, ext;
        size_t dotPos = fileName.find_last_of('.');
        if (dotPos != string::npos) {
            base = fileName.substr(0, dotPos);
            ext = fileName.substr(dotPos);
        }
        int counter = 1;
        string newFilename = fileName;
        while (fileNames.find(newFilename) != fileNames.end()) {
            ostringstream oss;
            oss << base << counter << ext;
            newFilename = oss.str();
            counter++;
        }
        fileName = newFilename;
        cout << "File system name has existed\n"
            << "File system name after change: " << fileName << '\n';
    }
    // Viết entry
    vector<uint32_t> clusterStoreFile = writeEntry(fileName, vol, password, file, dataSize);
    // Xác định kích thước của dữ liệu
    // Ghi nội dung tập tin vào volume
    for (auto i : clusterStoreFile) {
        uint32_t idx = (vol->getBootSec()->getSb() + vol->getBootSec()->getNf() * vol->getBootSec()->getSf() + i * vol->getBootSec()->getSc()) * 512;
        writeBinaryFile(vol->getVolumeFileName(), content, dataSize, idx, file);
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[i]), sizeof(uint32_t), vol->getBootSec()->getSb() * 512 + i * 4, file);
        writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[i]), sizeof(uint32_t), (vol->getBootSec()->getSb() + vol->getBootSec()->getSf()) * 512 + i * 4, file);
    }
}

void Data::exportFile(const string& fileNameIn, Volume* vol, const string& fileNameOut, fstream &file) {
    // vị trí để lấy file trong volume
    // fileNameIn là file name đã tồn tại trong volume mà mình muốn xuất ra
    // fileNameOut là file name mình muốn xuất ra dưới dạng cái tên gì. 
    if (fileNames.find(fileNameIn) != fileNames.end()) { // check xem có tồn tại file trong volume không
        // tìm các cluster lưu trữ file trong fat
        if (checkFileDeleted(fileNameIn)) {
            cout << "File đã bị xóa tạm thời\n";
            return;
        }
        if (checkFilePassword(fileNameIn)) {
            vector<uint32_t> cluster;
            cluster.push_back(entries[fileNames[fileNameIn]].first.getStartCluster());
            for (int i = entries[fileNames[fileNameIn]].first.getStartCluster(); i < vol->getClusterTable()->getElement().size() - 1; ++i) {
                if (vol->getClusterTable()->getElement()[i] == EOC) {
                    // Khi đến cluster kết thúc file thì ngừng
                    break;
                }
                cluster.push_back(vol->getClusterTable()->getElement()[i]);
                i = vol->getClusterTable()->getElement()[i] - 1;
            }
            // Thay đổi thời gian truy cập file
            changeFileDateTime(vol, fileNameIn, file);
            fstream file1(fileNameOut, ios::out | ios::binary | ios::ate);
            if (!file1)
            {
                cout << "Fail to open file\n";
                return;
            }
            vector<uint8_t> content;
            uint32_t size1Cluster = vol->getBootSec()->getSc() * 512;
            uint32_t dataSize = entries[fileNames[fileNameIn]].first.getFileSize();
            for (int i = 0; i < cluster.size(); ++i) {
                if (i == cluster.size() - 1) {
                    vector<uint8_t> tmp = readBinaryFile(vol->getVolumeFileName(), dataSize, (vol->getBootSec()->getSb() + vol->getBootSec()->getNf() * vol->getBootSec()->getSf() + cluster[i] * vol->getBootSec()->getSc()) * 512, file);
                    content.insert(content.end(), tmp.begin(), tmp.end());
                    break;
                }
                dataSize -= size1Cluster;
                vector<uint8_t> tmp = readBinaryFile(vol->getVolumeFileName(), size1Cluster, (vol->getBootSec()->getSb() + vol->getBootSec()->getNf() * vol->getBootSec()->getSf() + cluster[i] * vol->getBootSec()->getSc()) * 512, file);
                content.insert(content.end(), tmp.begin(), tmp.end());
            }
            writeBinaryFile(fileNameOut, content, content.size(), 0, file1);
            file1.close();
        }
        else {
            cout << "Password is incorrect!\n";
        }
    }
    else {
        cout << "File does not exist\n";
    }
}

void Data::listAllFile() {
    if (entries.size()) {
        cout << setw(3) << left << "Attribute"
            << setw(22) << right << "CreateTime"
            << setw(25) << right << "LastAccessTime"
            << setw(12) << right << "Length"
            << setw(20) << right << "Name" << '\n';
            
        for (auto i : fileNames) {
            if (checkFileDeleted(i.first)) {
                continue;
            }
            // Attribute
            bitset<8> bits(entries[i.second].first.getAttribute());
            for (int i = 7; i >= 0; --i) {
                if (i == 0 && bits[i]) {
                    // readonly
                    cout << "r";
                }
                else if (i == 1 && bits[i]) {
                    // hidden
                    cout << "h";
                }
                else if (i == 5 && bits[i]) {
                    // archive
                    cout << "a";
                }
                else {
                    cout << "|";
                }
            }
            // CreateTime
            uint32_t tmp = 0;
            cout << setw(10) << right
                << tmp + entries[i.second].first.getCreateDay().getDay() << "/"
                << tmp + entries[i.second].first.getCreateDay().getMonth() << "/"
                << tmp + entries[i.second].first.getCreateDay().getYear() + 2024 << " "
                << tmp + entries[i.second].first.getCreateTime().getHour() << ":"
                << tmp + entries[i.second].first.getCreateTime().getMinute() << ":"
                << tmp + entries[i.second].first.getCreateTime().getSecond();
            // LastAccessTime
            cout << setw(7) << right
                << tmp + entries[i.second].first.getAccessDay().getDay() << "/"
                << tmp + entries[i.second].first.getAccessDay().getMonth() << "/"
                << tmp + entries[i.second].first.getAccessDay().getYear() + 2024 << " "
                << tmp + entries[i.second].first.getAccessTime().getHour() << ":"
                << tmp + entries[i.second].first.getAccessTime().getMinute() << ":"
                << tmp + entries[i.second].first.getAccessTime().getSecond();
            // Length
            cout << setw(10) << right
                << entries[i.second].first.getFileSize();
            // Name
            cout << setw(30) << right << i.first << '\n';
        }
    }
    else {
        cout << "Volume has no file\n";
    }
}

bool Data::checkFilePassword(const string& fileName) {
    if (fileNames.find(fileName) != fileNames.end()) { // check xem có tồn tại file trong volume không
        // tìm các cluster lưu trữ file trong fat
        uint8_t fileKeyPassword = entries[fileNames[fileName]].first.getPassword();
        if (fileKeyPassword) {
            // file có password
            string password;
            cout << "File password: ";
            cin >> password;
            // check xem có đúng file password không
            if (stringHash(password) == fileKeyPassword) {
                // đúng password
                return true;
            }
            else {
                cout << "Password is incorrect\n";
                return false;
            }
        }
        else {
            // file không có password
            return true;
        }
    }
    return false;
}

void Data::changeFilePassword(Volume* vol, const string& fileName, fstream &file) {
    if (fileNames.find(fileName) != fileNames.end()) {
        if (checkFileDeleted(fileName)) {
            cout << "File đã bị xóa tạm thời\n";
            return;
        }
        
        if (vol->getData()->checkFilePassword(fileName)) {
            string password;
            cout << "New Password: ";
            cin >> password;
            entries[fileNames[fileName]].first.setPassword(password);
            uint32_t idx = getPositionEntry(vol, fileName) + 10;
            writeBinaryFile(vol->getVolumeFileName(), {entries[fileNames[fileName]].first.getPassword()}, 1, idx, file);
            changeFileDateTime(vol, fileName, file);
            return;
        }
        // Không đúng password
        cout << "Password is incorrect!\n";
    } else {
        cout << "File does not exist!\n";
    }
}

// Lấy vị trí byte của file entry trong volume
uint32_t Data::getPositionEntry(Volume* vol, const string& fileName) {
    // đi tới cluster 2 trước
    uint32_t sizeof1Cluster = 512 * vol->getBootSec()->getSc();
    bool ok = 0;
    uint32_t i = 0;
    for (auto x : clusterEntry) {
        uint32_t bytes = 0,
        idx = (vol->getBootSec()->getSb() + vol->getBootSec()->getNf() * vol->getBootSec()->getSf() + x * vol->getBootSec()->getSc()) * 512;
        while (bytes < sizeof1Cluster && i <= fileNames[fileName]) {
            bytes += Sizeof(entries[i]);
            ++i;
            if (bytes > sizeof1Cluster) {
                // Vì ngay tại đây entry đã cộng byte của chính nó và kích thước của byte entry đó kể từ cluster x vượt quá 1 cluster nên ta sẽ
                ok = 1;
            }
        }
        if (!ok) {
            idx += bytes - Sizeof(entries[i - 1]);
            return idx;
        }
    }
    return 0;
};

void Data::changeFileDateTime(Volume* vol, const string& fileName, fstream &file) {
    pair<Time, Date> dataTime = getDayTimeToday();
    entries[fileNames[fileName]].first.setAccessTime(dataTime.first);
    entries[fileNames[fileName]].first.setAccessDay(dataTime.second);
    uint32_t idx = getPositionEntry(vol, fileName);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<Time>(entries[fileNames[fileName]].first.getAccessTime()), 3, idx + 17, file);
    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<Date>(entries[fileNames[fileName]].first.getAccessDay()), 3, idx + 20, file);
}

set<uint32_t> Data::getClusterEntry() {
    return clusterEntry;
}

void Data::assignClusterEntry(const set<uint32_t>& clusterEntry) {
    this->clusterEntry = clusterEntry;
}

map<string, uint32_t> Data::getFileNames() {
    return fileNames;
}

void Data::deleteFileCanRestore(const string &fileName, Volume* vol, fstream &file) {
    // kiểm tra mật khẩu trước
    if (fileNames.find(fileName) != fileNames.end()) { // check xem có tồn tại file trong volume không
        // tìm các cluster lưu trữ file trong fat
        if (checkFilePassword(fileName)) {
            // ghi entry trước
            // lấy vị trí entry trước
            changeFileDateTime(vol, fileName, file);
            uint32_t idx = getPositionEntry(vol, fileName) + 31;
            writeBinaryFile(vol->getVolumeFileName(), {0xE5}, 1, idx, file);
            entries[fileNames[fileName]].first.setReserve(0xE5);
            idx += 32;
            for (auto &i : entries[fileNames[fileName]].second) {
                writeBinaryFile(vol->getVolumeFileName(), {0xE5}, 1, idx, file);
                i.setReserve(0xE5);
                idx += 32;
            }
            // ghi vào 2 bảng table
            idx = vol->getBootSec()->getSb() * 512;
            uint32_t j;
            for (uint32_t i = entries[fileNames[fileName]].first.getStartCluster(); i < vol->getClusterTable()->getElement().size(); ++i) {
                writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(RESERVED), 4, idx + i * 4, file);
                writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(RESERVED), 4, vol->getBootSec()->getSf() * 512 + idx + i * 4, file);
                if (vol->getClusterTable()->getElement()[i] == EOC) {
                    vol->getClusterTable()->setElement(i, RESERVED);
                    break;
                }
                j = i;
                i = vol->getClusterTable()->getElement()[i];
                vol->getClusterTable()->setElement(j, RESERVED);
            }
        }
    } else {
        cout << "File does not exist\n";
    }
}

void Data::deleteFileForever(const string &fileName, Volume* vol, fstream &file) {
    // xóa hết nội dung file cần xóa
    // Tìm vị trí nội dung file
    // kiểm tra mật khẩu trước
    if (fileNames.find(fileName) != fileNames.end()) { // check xem có tồn tại file trong volume không
        // tìm các cluster lưu trữ file trong fat
        if (checkFilePassword(fileName)) {
            uint32_t idx = vol->getBootSec()->getSb() + vol->getBootSec()->getSf() * 2;
            uint32_t bytesLeft = entries[fileNames[fileName]].first.getFileSize();
            uint32_t sizeOf1Cluster = vol->getBootSec()->getSc() * 512;
            vector<uint8_t> emptyBytes(bytesLeft, '\0');
            uint32_t j;
            vol->changeNumEmptyCluster(static_cast<uint32_t>(1 * ceil(float(bytesLeft) / (vol->getBootSec()->getSc() * vol->getBootSec()->getSectorByte()))));
            for (uint32_t i = entries[fileNames[fileName]].first.getStartCluster(); i < vol->getClusterTable()->getElement().size(); ++i) {
                if (bytesLeft <= sizeOf1Cluster) {
                    // ghi kí tự '\0' vào nội dung file
                    writeBinaryFile(vol->getVolumeFileName(), emptyBytes, bytesLeft, (idx + i * vol->getBootSec()->getSc()) * 512, file);
                    // ghi kí tự '\0' vào 2 bảng table
                    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>('\0'), 4, vol->getBootSec()->getSb() * 512 + i * 4, file);
                    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>('\0'), 4, (vol->getBootSec()->getSb() + 2 * vol->getBootSec()->getSf()) * 512 + i * 4, file);
                    vol->getClusterTable()->setElement(i, '\0');
                    break;
                }
                else {
                    // ghi kí tự '\0' vào nội dung file
                    writeBinaryFile(vol->getVolumeFileName(), emptyBytes, sizeOf1Cluster, (idx + i * vol->getBootSec()->getSc()) * 512, file);
                    // ghi kí tự '\0' vào 2 bảng table
                    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>('\0'), 4, vol->getBootSec()->getSb() * 512 + i * 4, file);
                    writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>('\0'), 4, (vol->getBootSec()->getSb() + 2 * vol->getBootSec()->getSf()) * 512 + i * 4, file);
                    bytesLeft -= sizeOf1Cluster;
                    j = i;
                    i = vol->getClusterTable()->getElement()[i];
                    vol->getClusterTable()->setElement(j, vol->getClusterTable()->getElement()[j]);
                }
            }
            // Tìm vị trí entry và chuyển tất cả các byte trong entry thành "0xE5"
            idx = getPositionEntry(vol, fileName);
            vector<uint8_t> e5Bytes(32, 0xE5);
            writeBinaryFile(vol->getVolumeFileName(), e5Bytes, 32, idx, file);
            entries[fileNames[fileName]].first.setToNull();
            idx += 32;
            for (auto &i : entries[fileNames[fileName]].second) {
                writeBinaryFile(vol->getVolumeFileName(), e5Bytes, 32, idx, file);
                i.setToNull();
                idx += 32;
            }
        }
    } else {
        cout << "File does not exist!\n";
    }
}

bool Data::checkFileDeleted(const string &fileName) {
    return entries[fileNames[fileName]].first.getReserve() == 0xE5 ? true : false;
}

void Data::restoreFile(const string &fileName, Volume* vol, fstream &file) {
    // Sửa entry trước
    // kiểm tra mật khẩu trước
    if (fileNames.find(fileName) != fileNames.end()) { // check xem có tồn tại file trong volume không
        // tìm các cluster lưu trữ file trong fat
        changeFileDateTime(vol, fileName, file);
        uint32_t idx = getPositionEntry(vol, fileName) + 31;
        writeBinaryFile(vol->getVolumeFileName(), {'\0'}, 1, idx, file);
        entries[fileNames[fileName]].first.setReserve('\0');
        idx += 32;
        for (auto &i : entries[fileNames[fileName]].second) {
            writeBinaryFile(vol->getVolumeFileName(), {'\0'}, 1, idx, file);
            i.setReserve('\0');
            idx += 32;
        }
        // Sửa 2 bảng table
        vector<uint32_t> clusterStoreFile;
        uint32_t startCluster = vol->getClusterTable()->findPositionNewFileFat(entries[fileNames[fileName]].first.getFileSize(), vol, clusterStoreFile, 1, vol->getData()->getEntries()[vol->getData()->getFileNames()[fileName]].first.getStartCluster());
        idx = vol->getBootSec()->getSb() * 512;
        for (uint32_t i = 0; i < clusterStoreFile.size(); ++i) {
            writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[i]), 4, idx + clusterStoreFile[i] * 4, file);
            writeBinaryFile(vol->getVolumeFileName(), convertTtoVectorByte<uint32_t>(vol->getClusterTable()->getElement()[i]), 4, vol->getBootSec()->getSf() * 512 + idx + clusterStoreFile[i] * 4, file);
        }
    } else {
        cout << "File does not exist!\n";
    }
}