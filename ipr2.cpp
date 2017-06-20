/**
 * Иерархия классов для организации виртуальной файловой системы.
 * Разработчик - Дубинин А. В. (http://dubinin.net)
 * 09.01.2017
 */

// Подключение файлов из стандартной библиотеки
#include <iostream>
#include <ctime>
#include <unistd.h>

// Открытие пространства имен std
using namespace std;

// Объявление констант
const string DRIVER_NATIVE    = "native";
const string DRIVER_BLOCK     = "block";
const string DRIVER_FTP       = "ftp";
const string ROOT_NATIVE      = "syscall";
const string ROOT_BLOCK       = "block.binary";
const string ROOT_FTP         = "/";
const string ENCODING_ASCII   = "ASCII";
const string ENCODING_LATIN_1 = "ISO 8859-1";
const string ENCODING_UTF_8   = "UTF-8";
const string DEFAULT_FILENAME = "new_file";
const string EMPTY_VALUE      = "";

const char MODE_WRITE   = 'a';
const char MODE_REWRITE = 'w';

const string BR              = "\n\n";
const string MSG_META        = "Информация о драйвере файловой системы";
const string MSG_DRIVER_NAME = "Название драйвера файловой системы: ";
const string MSG_ENCODING    = "Кодировка: ";
const string MSG_ROOT        = "Корневой элемент: ";
const string MSG_DIR         = "Директория: ";
const string MSG_FILE        = "Файл: ";
const string MSG_MOUNT_DATE  = "Дата монтирования: ";
const string MSG_CREATE_DATE = "Дата создания: ";
const string MSG_CHANGE_DATE = "Дата изменения: ";
const string MSG_FILESIZE    = "Размер файла: ";
const string MSG_APP_EXIT    = "Для выхода из программы нажмите любую клавишу";

/**
 * @interface IVFSObject
 * Описывает поведение записи виртуальной файловой системы.
 */
class IVFSObject
{
    public:
        /**
         * Статический метод getInstance() - возвращает экземпляр записи
         * виртуальной файловой системы.
         * @return IVFSObject
         */
        static IVFSObject* getInstance();
};

/**
 * @interface IVFS
 * Описывает методы доступа к метаинформации драйвера
 * виртульной файловой системы.
 */
class IVFS
{
    public:
        /**
         * Виртуальный метод getDriverName() - возвращает название
         * драйвера файловой системы.
         * @return string
         */
        virtual string getDriverName() = 0;

        /**
         * Виртуальный метод getFileNameEncoding() - возвращает используемую
         * кодировку в именах записей.
         * @return string
         */
        virtual string getFileNameEncoding() = 0;

        /**
         * Виртуальный метод getRootElement() - возвращает корневой
         * элемент файловой системы.
         * @return IVFSObject
         */
        virtual IVFSObject* getRootElement() = 0;
};

/**
 * @interface IDriver расширяет интерфейсы IVFSObject, IVFS.
 * Описывает поведение драйвера файловой системы.
 */
class IDriver: public IVFSObject, public IVFS
{
    public:
        /**
         * Статический метод getInstance() - возвращает драйвер
         * файловой системы с помощью шаблона Singleton.
         * @return IDriver
         */
        static IDriver* getInstance();
};

/**
 * @interface IFile расширяет интерфейс IVFSObject.
 * Описывает поведение записи типа 'файл'.
 */
class IFile: public IVFSObject
{
    public:
        /**
         * Виртуальный метод name() - возвращает название файла.
         * @return string
         */
        virtual string name() = 0;

        /**
         * Виртуальный метод path() - возвращает путь к файлу.
         * @return string
         */
        virtual string path() = 0;

        /**
         * Виртуальный метод fSize() - возвращает размер файла (в байтах).
         * @return size_t
         */
        virtual size_t fSize() = 0;

        /**
         * Виртуальный метод creationDate() - возвращает дату и время
         * создания файла.
         * @return string
         */
        virtual string creationDate() = 0;

        /**
         * Виртуальный метод modificationDate() - возвращает дату и время
         * последнего изменения файла.
         * @return string
         */
        virtual string modificationDate() = 0;

        /**
         * Виртуальный метод read() - возвращает содержимое файла.
         * @return string
         */
        virtual string read() = 0;

        /**
         * Виртуальный метод write() - записывает данные в файл.
         * @param string - данные
         * @param char - режим записи
         * @return void
         */
        virtual void write(string, char const = MODE_REWRITE) = 0;

        /**
         * Виртуальный метод moveTo() - перемещает файл.
         * @param string - новый путь к файлу
         * @return void
         */
        virtual void moveTo(string) = 0;

        /**
         * Виртуальный метод rename() - переименовывает файл.
         * @param string - новое название файла
         * @return void
         */
        virtual void rename(string) = 0;
};

/**
 * @interface IDirectory расширяет интерфейс IVFSObject.
 * Описывает поведение записи типа 'директория'.
 */
class IDirectory: public IVFSObject
{
    public:
        /**
         * Виртуальный метод path() - возвращает адрес директории.
         * @return string
         */
        virtual string path() = 0;

        /**
         * Виртуальный метод creationDate() - возвращает дату и время
         * создания директории.
         * @return string
         */
        virtual string creationDate() = 0;

        /**
         * Виртуальный метод moveTo() - перемещает директорию.
         * @param string - новый адрес директории
         * @return void
         */
        virtual void moveTo(string) = 0;
};

/**
 * @interface IVFSObjectTime
 * Описывает работу с датой и временем (создание, изменение)
 * записей виртуальной файловой системы.
 */
class IVFSObjectTime
{
    public:
        /**
         * Статический метод initCurrentDate() - возвращает текущую
         * системную дату и время.
         * @return string
         */
        static string initCurrentDate();
};

/**
 * @class VFSObjectTime реализует интерфейс IVFSObjectTime.
 * Класс для работы с датой и временем.
 */
class VFSObjectTime: public IVFSObjectTime
{
    public:
        static string initCurrentDate()
        {
            time_t seconds = time(NULL);
            tm* timeinfo = localtime(&seconds);
            return asctime(timeinfo);
        }
};

/**
 * @class File реализует интерфейс IFile.
 * Класс записи типа 'файл' файловой системы.
 */
class File: public IFile
{
    private:
        string _path;             // путь к файлу
        string _name;             // название файла
        string _creationDate;     // дата и время создания файла
        string _modificationDate; // дата и время последнего изменения файла
        string data;              // содержимое файла

        /**
         * Конструктор File
         * @param string fPath - путь к файлу
         * @param string fName - название файла
         */
        File(string fPath, string fName)
        {
            this->_path             = fPath;
            this->_name             = fName;
            this->_creationDate     = VFSObjectTime::initCurrentDate();
            this->_modificationDate = this->_creationDate;
            this->data              = EMPTY_VALUE;
        }
    public:
        static IFile* getInstance(
            string fPath,
            string fName = DEFAULT_FILENAME
        ) {
            return new File(fPath, fName);
        }
        string path()
        {
            return this->_path;
        }
        string name()
        {
            return this->_name;
        }
        size_t fSize()
        {
            return data.length();
        }
        string creationDate()
        {
            return this->_creationDate;
        }
        string modificationDate()
        {
            return this->_modificationDate;
        }
        string read()
        {
            return this->data;
        }

        /**
         * Метод write() - записывает данные в файл и изменяет дату и время
         * последней модификации файла.
         * В зависимости от значения переметра mode данные могут быть:
         * - перезаписаны в файле, если mode = MODE_REWRITE
         * - дописаны в конец файла, если mode = MODE_WRITE
         *
         * @param string - данные
         * @param char - режим записи
         * @return void
         */
        void write(string newData, char const mode = MODE_REWRITE)
        {
            switch (mode) {
                case MODE_REWRITE:
                    this->data = newData;
                    break;
                case MODE_WRITE:
                    this->data += newData;
                    break;
                default:
                    return;
            }
            this->_modificationDate = VFSObjectTime::initCurrentDate();
            return;
        }
        void moveTo(string newPath)
        {
            this->_path = newPath;
            return;
        }
        void rename(string newName)
        {
            this->_name = newName;
            return;
        }
};

/**
 * @class Directory реализует интерфейс IDirectory.
 * Класс записи типа 'директория' файловой системы.
 */
class Directory: public IDirectory
{
    private:
        string _path;         // адрес директории
        string _creationDate; // дата и время создания директории

        /**
         * Конструктор Directory
         * @param string dPath - адрес директории
         */
        Directory(string dPath)
        {
            this->_path         = dPath;
            this->_creationDate = VFSObjectTime::initCurrentDate();
        }
    public:
        static IDirectory* getInstance(string dPath)
        {
            return new Directory(dPath);
        }
        string path()
        {
            return this->_path;
        }
        string creationDate()
        {
            return this->_creationDate;
        }
        void moveTo(string newPath)
        {
            this->_path = newPath;
            return;
        }
};

/**
 * @interface IMountPoint расширяет интерфейс IVFSObject.
 * Описывает поведение точки монтирования.
 */
class IMountPoint: public IVFSObject
{
    public:
        /**
         * Виртуальный метод mount() - монтирование файловой системы.
         * @return void
         */
        virtual void mount() = 0;

        /**
         * Виртуальный метод unmount() - демонтирование файловой системы.
         * @return void
         */
        virtual void unmount() = 0;
};

/**
 * @class AbstractDriver реализует интерфейсы IDriver, IMountPoint.
 * Абстрактный класс драйвера файловой системы.
 */
class AbstractDriver: public IDriver, public IMountPoint
{
    protected:
        static IDriver* instance; // экземпляр драйвера файловой системы
        string driverName;        // название драйвера файловой системы
        string fileNameEncoding;  // используемая кодировка имен записей
        IVFSObject* rootElement;  // корневой элемент файловой системы
    public:
        string getDriverName()
        {
            return this->driverName;
        }
        string getFileNameEncoding()
        {
            return this->fileNameEncoding;
        }
        void mount()
        {
            return;
        }
        void unmount()
        {
            return;
        }
};

/**
 * Первичная инициализация экземпляра абстрактного
 * драйвера файловой системы.
 */
IDriver* AbstractDriver::instance = NULL;

/**
 * @class DriverNative расширяет абстрактный класс AbstractDriver.
 * Класс драйвера файловой системы native.
 */
class DriverNative: public AbstractDriver
{
    protected:
        IFile* rootElement;
        DriverNative()
        {
            this->driverName       = DRIVER_NATIVE;
            this->fileNameEncoding = ENCODING_ASCII;
            this->rootElement      = File::getInstance(EMPTY_VALUE, ROOT_NATIVE);
        }
    public:
        static IDriver* getInstance()
        {
            if (instance == NULL) {
                instance = new DriverNative();
            }
            return instance;
        }
        IFile* getRootElement()
        {
            return this->rootElement;
        }
};

/**
 * @class DriverBlock расширяет абстрактный класс AbstractDriver.
 * Класс драйвера файловой системы block.
 */
class DriverBlock: public AbstractDriver
{
    private:
        IFile* rootElement;
        DriverBlock()
        {
            this->driverName       = DRIVER_BLOCK;
            this->fileNameEncoding = ENCODING_LATIN_1;
            this->rootElement      = File::getInstance(EMPTY_VALUE, ROOT_BLOCK);
        }
    public:
        static IDriver* getInstance()
        {
            if (instance == NULL) {
                instance = new DriverBlock();
            }
            return instance;
        }
        IFile* getRootElement()
        {
            return this->rootElement;
        }
};

/**
 * @class DriverFtp расширяет абстрактный класс AbstractDriver.
 * Класс драйвера файловой системы ftp.
 */
class DriverFtp: public AbstractDriver
{
    private:
        IDirectory* rootElement;
        DriverFtp()
        {
            this->driverName       = DRIVER_FTP;
            this->fileNameEncoding = ENCODING_UTF_8;
            this->rootElement      = Directory::getInstance(ROOT_FTP);
        }
    public:
        static IDriver* getInstance()
        {
            if (instance == NULL) {
                instance = new DriverFtp();
            }
            return instance;
        }
        IDirectory* getRootElement()
        {
            return this->rootElement;
        }
};

/**
 * @interface IDriverFactory
 * Описывает поведение фабрики классов драйверов представления файловой системы.
 */
class IDriverFactory
{
    public:
        /**
         * Статический метод create() - создает драйвер файловой системы,
         * если объявлен класс для такого драйвера.
         * @param string
         * @return IDriver|NULL
         */
        static IDriver* create(string);
};

/**
 * @class DriverFactory реализует интерфейс IDriverFactory.
 * Класс фабрики драйверов.
 */
class DriverFactory: public IDriverFactory
{
    public:
        /**
         * Возвращает объект драйвера по его названию,
         * либо NULL, если класс не объявлен.
         * @param string driverName
         * @return IDriver|NULL
         */
        static IDriver* create(string driverName)
        {
            if (driverName == DRIVER_NATIVE) return DriverNative::getInstance();
            if (driverName == DRIVER_BLOCK) return DriverBlock::getInstance();
            if (driverName == DRIVER_FTP) return DriverFtp::getInstance();
            return NULL;
        }
};

/**
 * @class Application
 */
class Application
{
    public:
        /**
         * Статический метод run() - запускает приложение.
         * @return void
         */
        static void run()
        {
            /**
             * Объявление и инициализация переменных для проверки
             * работы приложения:
             *
             * - exampleDriver - название драйвера файловой системы (string);
             * получение значения из внешних источников: I/O, DB, config, etc.
             * - exampleDir - адрес директории (string)
             * - exampleFile - название файла (string)
             */
            string exampleDriver = DRIVER_FTP;
            string exampleDir    = "/httpdocs/";
            string exampleFile   = "index.php";

            /**
             * Объявление переменной driver типа IDriver.
             * Инициализация объекта реализуется с помощью
             * фабрики классов по названию драйвера файловой системы.
             */
            IDriver* driver = DriverFactory::create(exampleDriver);
            // Получение корневого элемента файловой системы
            IDirectory* rootElement = (IDirectory*) driver->getRootElement();

            /**
             * Вывод на экран метаинформации драйвера файловой системы.
             */
            cout << MSG_META << BR;
            cout << MSG_DRIVER_NAME << driver->getDriverName() << endl;
            cout << MSG_ENCODING << driver->getFileNameEncoding() << endl;
            cout << MSG_ROOT << rootElement->path() << endl;
            cout << MSG_MOUNT_DATE << rootElement->creationDate() << endl;
            cout << "----------------------------------------" << BR;

            // Ожидание 8 сек после монтирования файловой системы
            sleep(8);
            // Создание директории
            IDirectory* newDir = Directory::getInstance(exampleDir);
            // Ожидание 3 сек после создания директории
            sleep(3);
            // Создание файла
            IFile* newFile = File::getInstance(newDir->path(), exampleFile);
            // Ожидание 5 сек после создания файла
            sleep(5);
            // Модицикация файла
            newFile->write("some information");

            /**
             * Вывод на экран информации о созданных записях файловой системы.
             */
            cout << MSG_DIR << newDir->path() << endl;
            cout << MSG_CREATE_DATE << newDir->creationDate();
            cout << "----------------------------------------" << BR;
            cout << MSG_FILE << newFile->path() + newFile->name() << endl;
            cout << MSG_FILESIZE << newFile->fSize() << endl;
            cout << MSG_CREATE_DATE << newFile->creationDate();
            cout << MSG_CHANGE_DATE << newFile->modificationDate();
            cout << "----------------------------------------" << BR;

            // Освобождение ресурсов
            delete newFile;
            delete newDir;
            delete driver;

            // Вывод на экран диалога выхода из программы
            cout << MSG_APP_EXIT;
            cin.get();
            return;
        }
};

int main()
{
    // Запуск приложения
    Application::run();
}
