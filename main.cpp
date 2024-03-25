#include "volume.h"
using namespace std;
class UserInterface
{
private:
    Volume* v;
public:
    void StartProgram() {
        int n, choice;
        string volumeFileName, password;
        while (true) {
            do
            {
                cout << "Set MyFS Menu:\n"
                    << "1. Create new volume\n"
                    << "2. Access existed volume\n"
                    << "3. Exit\n"
                    << "Your choice: ";
                cin >> n;
            } while (n < 1 || n > 3);
            switch (n) {
            case 1: {
                v = new Volume;
                cout << "Successfully create new volume\n";
                // Tạo volume xong phải đăng nhập lại
                break;
            }
            case 2: {
                cout << "File system name: ";
                cin >> volumeFileName;
                cout << "Volume Password: ";
                cin >> password;
                if (getFileSize(volumeFileName) != UINT32_MAX) {
                    // có tồn tại file system
                    fstream file(volumeFileName, ios::in | ios::out | ios::binary);
                    if (!file)
                    {
                        cout << "Fail to open file\n";
                        break;
                    }
                    v = new Volume(volumeFileName, password, file);
                    if (stringHash(password) == v->getBootSec()->getVolumePassword()) {
                        cout << "Password is correct\n";
                        // đọc hết bảng fat, entry
                        v->setData(new Data);
                        v->setClusterTable(new ClusterTable, v);
                        v->getClusterTable()->readAllFat(v, file);
                        setClusterEntry(v);
                        v->getData()->readAllEntry(volumeFileName, v, file);
                        v->getData()->updateFileNames();
                        while (true) {
                            choice = showMenu();
                            if (processChoice(choice, file)) { // processChoice(choice) = 1 thì exit ra Set MyFS Menu
                                break;
                            }
                        }
                        file.close();
                    }
                    else {
                        cout << "Password is incorrect\n";
                        break;
                    }
                }
                else {
                    cout << "File System does not exist\n";
                }
                break;
            }
            case 3: {
                cout << "Program end!\n";
                return;
            }
            }
        }
    }
    int showMenu()
    {
        int choice;
        do
        {
            cout << "MyFS Management Menu:\n"
                << "1. change Volume Password\n"
                << "2. List Files\n"
                << "3. Change File Password\n"
                << "4. Import File\n"
                << "5. Export File\n"
                << "6. Delete File temporarily\n"
                << "7. Delete File permanently\n"
                << "8. Recover File\n"
                << "9. Show volume left\n" // xem kích thước volume còn lại bao nhiêu
                << "0. Exit\n"
                << "Your choice: ";
            cin >> choice;
        } while (choice < 0 || choice > 9);
        return choice;
    }
    bool processChoice(int choice, fstream &file)
    {
        string password, fileName;
        bool flag = 0;
        switch (choice)
        {
        case 1: {
            cout << "New Password: ";
            cin >> password;
            v->getBootSec()->changeVolumePassword(password, v, file);
            cout << "change Volume Password successfully\n";
            break;
        }
        case 2: {
            v->getData()->listAllFile();
            cout << '\n';
            break;
        }
        case 3: {
            cout << "File name to change password: ";
            cin >> fileName;
            v->getData()->changeFilePassword(v, fileName, file);
            break;
        }
        case 4: {
            cout << "File name to import: ";
            cin >> fileName;
            bool ok = 0;
            cout << "File Password (Yes=1|No=0): ";
            cin >> ok;
            if (ok) {
                cout << "Password: ";
                cin >> password;
                v->getData()->writeFile(fileName, v, password, file);
            }
            else {
                v->getData()->writeFile(fileName, v, "", file);
            }
            break;
        }
        case 5: {
            cout << "File name to export: ";
            cin >> fileName;
            cout << "File name to export to: ";
            string fileNameOut;
            cin >> fileNameOut;
            v->getData()->exportFile(fileName, v, fileNameOut, file);
            break;
        }
        case 6: {
            cout << "File name to delete temporarily: ";
            cin >> fileName;
            v->getData()->deleteFileCanRestore(fileName, v, file);
            break;
        }
        case 7: {
            cout << "File name to delete permanently: ";
            cin >> fileName;
            v->getData()->deleteFileForever(fileName, v, file);
            break;
        }
        case 8: {
            cout << "File name to recover: ";
            cin >> fileName;
            v->getData()->restoreFile(fileName, v, file);
            break;
        }
        case 9: {
            cout << "Volume left: " << v->getNumEmptyCluster() * v->getBootSec()->getSc() * v->getBootSec()->getSectorByte() << " bytes" << '\n';
            break;
        }
        case 0:
            cout << "Exit.\n";
            flag = 1;
            break;
        }
        return flag;
    }
};

int main(int argc, char const* argv[])
{
    UserInterface* ui = new UserInterface();
    ui->StartProgram();
    delete ui;
    return 0;
}