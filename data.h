#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <chrono>
#include <set>
#include <map>
#include <bitset>
#include <iomanip>
#include <sstream>
using namespace std;
#pragma pack(push, 1)

class Time;
class Date;
class ShortEntry;
class LongEntry;
class Data;

void setClusterEntry(Volume* vol);
uint32_t getLastElementInSet(const set<uint32_t>& mySet);
pair<Time, Date> getDayTimeToday();
uint8_t setBits(uint8_t byte, const vector<uint32_t>& positions);
uint32_t getFileSize(const string& fileName);
bool findExistedFile(const string& fileName);
string convertFileName(const string& fileName);

class Time { // 3 byte
private:
    uint8_t second; // 1 byte
    uint8_t minute; // 1 byte
    uint8_t hour; // 1 byte
public:
    Time() = default;
    Time(const uint8_t& second, const uint8_t& minute, const uint8_t& hour) : second(second), minute(minute), hour(hour) {}
    uint8_t getSecond();
    uint8_t getMinute();
    uint8_t getHour();
    void setSecond(const uint8_t& second);
    void setMinute(const uint8_t& minute);
    void setHour(const uint8_t& hour);
};

class Date { // 1 byte
private:
    uint8_t day; // 1 byte
    uint8_t month; // 1 byte
    uint8_t year; // 1 byte; year = year - 2024
public:
    Date() = default;
    Date(const uint8_t& day, const uint8_t& month, const uint8_t& year) : day(day), month(month), year(year) {}
    uint8_t getDay();
    uint8_t getMonth();
    uint8_t getYear();
    void setDay(const uint8_t& day);
    void setMonth(const uint8_t& month);
    void setYear(const uint8_t& year);
};

class ShortEntry {
private:
    uint8_t name[9]{ '\0' }; // tên ngắn (6 byte) + tên mở rộng (3 byte)
    uint8_t attribute;
    uint8_t password;
    Time createTime;
    Date createDay;
    Time accessTime;
    Date accessDay;
    uint32_t startCluster;
    uint32_t fileSize;
    uint8_t reserve; // lưu kí tự '0xE5' nếu đã bị xóa, lưu kí tự '\0' nếu chưa bị xóa
public:
    uint8_t getAttribute();
    uint8_t getPassword();
    Time getCreateTime();
    Time getAccessTime();
    Date getCreateDay();
    Date getAccessDay();
    uint32_t getStartCluster();
    uint32_t getFileSize();
    uint8_t* getName();
    uint8_t getReserve();
    void setAttribute();
    void setPassword(const string& password);
    void setCreateTime(const Time& createTime);
    void setAccessTime(const Time& accessTime);
    void setCreateDay(const Date& createDay);
    void setAccessDay(const Date& accessDay);
    void setStartCluster(const uint32_t& startCluster);
    void setFileSize(const uint32_t& fileSize);
    void setName(uint8_t* name);
    void setReserve(const uint8_t& c); // set last byte to "E5" or "\0"
    void setToNull(); // set tất cả các byte của ShortEntry to NULL
};

class LongEntry {
private:
    uint8_t index;
    uint8_t fileName[30]{ '\0' }; // tên đầy đủ
    uint8_t reserve; // lưu kí tự '0xE5' nếu đã bị xóa, lưu kí tự '\0' nếu chưa bị xóa
public:
    uint8_t getIndex();
    uint8_t* getFileName();
    void setIndex(const uint8_t& index);
    void setFileName(uint8_t* fileName);
    void setReserve(const uint8_t& c); // set last byte to "E5" or "\0"
    void setToNull(); // set tất cả các byte của ShortEntry to NULL
};

class Data {
private:
    vector<pair<ShortEntry, vector<LongEntry>>> entries; // entry của 1 file
    map<string, uint32_t> fileNames; // fileName và vị trí của nó trong entries
    set<uint32_t> clusterEntry; // những cluster lưu entry
public:
    vector<pair<ShortEntry, vector<LongEntry>>> getEntries();
    vector<uint8_t> importFile(const string& fileName);
    void writeFile(string fileName, Volume* vol, const string& password, fstream &file);
    vector<uint32_t> writeEntry(string fileName, Volume* vol, const string& password, fstream &file, uint32_t fileSizeWrite);
    void readAllEntry(const string& fileName, Volume* vol, fstream &file);
    bool findExistedFileEntry(const string& fileName);
    uint8_t* convertFileNameToShortName(string& fileName);
    void exportFile(const string& fileNameIn, Volume* vol, const string& fileNameOut, fstream &file);
    void updateFileNames(); // cập nhật fileNames khi đọc từ ổ đĩa
    void listAllFile();
    bool checkFilePassword(const string& fileName);
    void changeFilePassword(Volume* vol, const string& fileName, fstream &file);
    void changeFileDateTime(Volume* vol, const string& fileName, fstream &file);
    uint32_t getPositionEntry(Volume* vol, const string& fileName);
    set<uint32_t> getClusterEntry();
    void assignClusterEntry(const set<uint32_t>& clusterEntry);
    map<string, uint32_t> getFileNames();
    void deleteFileCanRestore(const string &fileName, Volume* vol, fstream &file);
    void deleteFileForever(const string &fileName, Volume* vol, fstream &file);
    void restoreFile(const string &fileName, Volume* vol, fstream &file);
    bool checkFileDeleted(const string &fileName);
};

#pragma pack(pop)