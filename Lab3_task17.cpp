/*  Кулаков Роман Андреевич ПС-22
* Visual Studio 2022
*   17. Информация  о  файлах  на  жестких  дисках   компьютера
записана с помощью дерева. Листьям соответствуют файлы. Удалить
все копии одного и того же файла, оставив самую позднюю.  Если
папка оказывается пустой, то она тоже должна быть удалена (10).
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <Windows.h>

struct Time
{
    int day;
    int month;
    int year;
};

struct Tree
{
    std::string name;
    Time time;
    int level;
    Tree* father = nullptr;
    std::vector<Tree*> sons;
};

int ReadFromFile(std::ifstream& inFile, Tree*& root);
void PrintTree(Tree* root, int level);
void PrintMenu();
void DeleteTree(Tree*& root);
int ModeInput();
int MenuInput(Tree*& root);
int ChangeLocalization();

int main(int argc, char* argv[])
{   
    ChangeLocalization();
    if (argc != 2)
    {
        std::cout << "\nНедостаточно параметров";
        return 1;
    }
    std::ifstream Fin(argv[1]);
    if (!Fin)
    {
        std::cout << "\nНевозможно открыть файл %s" << argv[1];
        return 1;
    }
    Tree* root = NULL;
    ReadFromFile(Fin, root);
    while (true)
    {
        PrintMenu();
        MenuInput(root);
    }
}

int ChangeLocalization()
{
    setlocale(LC_CTYPE, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    return 0;
}

Time ReadTime(std::string time)
{   
    std::string date;
    int iter;
    Time nodeTime;

    nodeTime.day = 0;
    nodeTime.month = 0;
    nodeTime.year = 0;

    iter = 0;

    while(iter < time.length())
    {   
        if (time[iter] == '.')
        {
            iter++;
            nodeTime.day = std::stoi(date);
            date = "";
            break;
        }


        date += time[iter];
        iter++;
    }

    while (iter < time.length())
    {
        if (time[iter] == '.')
        {
            iter++;
            nodeTime.month = std::stoi(date);
            date = "";
            break;
        }

        date += time[iter];
        iter++;
    }

    while (iter < time.length())
    {
        date += time[iter];
        iter++;
    }

    nodeTime.year = std::stoi(date);

    return nodeTime;
}

bool TimeEqual(Time time1, Time time2)
{
    return time1.day == time2.day && time1.month == time2.month && time1.year == time2.year;
}

bool CompareTime(Time time1, Time time2)
{
    if (time1.year != time2.year) 
    {
        return time1.year > time2.year;
    }

    if (time1.month != time2.month) 
    {
        return time1.month > time2.month;
    }

    return time1.day > time2.day;
}

int ReadFromFile(std::ifstream& inFile, Tree*& root)
{   
    std::string buffer, time;
    int j, k, fatherLevel, nameLength;
    Tree *newNode, *prevFather, *father;
    fatherLevel = -1;
    father = NULL;
    while (std::getline(inFile, buffer))
    {
        nameLength = buffer.size();
        if (nameLength == 0) continue;
        k = 0;
        while (buffer[k] == '.')
        {
            k = k + 1;
        }
        newNode = new Tree;
        j = k;
        while (j < nameLength && buffer[j] != ' ')
        {
            newNode->name += buffer[j];
            j++;
        }
        while (j < nameLength)
        {   
            if (buffer[j] != ' ')
            {
                time += buffer[j];
            }
            j++;
        }
        if (!time.empty())
        {   
            Time nodeTime = ReadTime(time);
            newNode->time = nodeTime;
        }
        else
        {
            newNode->time.day = 0;
            newNode->time.month = 0;
            newNode->time.year = 0;
        }
        time.erase();
        newNode->level = k;
        if (k == 0)
        {   
            if (father == NULL)
            {
                root = newNode;
                father = root;
                fatherLevel = 0;
                continue;
            }
        }
        if (k - fatherLevel == 1)
        {   
            father->sons.push_back(newNode);
            newNode->father = father;
            fatherLevel = k;
            father = newNode;
        }
        else if (k == fatherLevel)
        {
            prevFather = father->father;
            prevFather->sons.push_back(newNode);
            newNode->father = prevFather;
            fatherLevel = k;
            father = newNode;
        }
        else if(k < fatherLevel && k > 0)
        {   
            prevFather = father;
            for (int i = 0; i <= fatherLevel - k; i++) prevFather = prevFather->father;
            prevFather->sons.push_back(newNode);
            newNode->father = prevFather;
            fatherLevel = k;
            father = newNode;
        }
        else
        {   
            std::cout << "Узел " << newNode->name << " не был включен в дерево, так как не подходит по параметрам" << std::endl;
            newNode = nullptr;
            delete newNode;
        }
    }
    return 0;
}

void PrintTree(Tree* root, int level)
{   
    if (root == NULL)
    {   
        std::cout << "Дерево пустое" << std::endl;
        return;
    }

    for (int i = 0; i < level; i++) std::cout << '.';
    std::cout << root->name;
    if (root->time.day != 0)
    {
        std::cout << ' ' << root->time.day << '.' << root->time.month << '.' << root->time.year;
    }
    std::cout << std::endl;
    for (int i = 0; i < root->sons.size(); i++)
        PrintTree(root->sons[i], level + 1);
}

Tree* FindFirstOccurrence(Tree* root, std::string& name) 
{
    if (!root) return nullptr;

    if (root->name == name) 
    {
        return root;
    }

    for (Tree* son: root->sons) {
        Tree* found = FindFirstOccurrence(son, name);
        if (found) 
        {
            return found;
        }
    }

    return nullptr;
}

Tree* FindMostNewFile(Tree*& root, std::string& name, Time& mostNewTime)
{   
    if (!root) return nullptr;

    for (Tree* son: root->sons) 
    {
        Tree* mostNewFile = FindMostNewFile(son, name, mostNewTime);
        if (mostNewFile->name == name && CompareTime(mostNewFile->time, mostNewTime)) 
        {   
            mostNewTime = mostNewFile->time;
        }
    }

    return root;
}

void DeleteFilesCopy(Tree*& root, std::string name, Time& mostNewTime)
{   
    if (root == NULL)
    {   
        std::cout << "Дерево пустое" << std::endl;
        return;
    }

    for (int i = root->sons.size() - 1; i >= 0; i--)
    {
        DeleteFilesCopy(root->sons[i], name, mostNewTime);
    }
    
    if (root->name == name && CompareTime(mostNewTime, root->time))
    {   
        Tree* fatherCopy = root->father;

        for(int i = 0; i < fatherCopy->sons.size(); i++)
        {
            if (fatherCopy->sons[i] == root)
            {
                delete root;
                root = nullptr;

                auto iter = fatherCopy->sons.begin();
                fatherCopy->sons.erase(iter + i);

                fatherCopy = nullptr;
                delete fatherCopy;

                return;
            }
        }
    }
    else if (root->name.find('.') == std::string::npos && root->father)
    {
        Tree* fatherCopy = root->father;

        for (int i = fatherCopy->sons.size() - 1; i >= 0; i--)
        {
            if (fatherCopy->sons[i] == root && root->sons.empty())
            {
                delete root;

                auto iter = fatherCopy->sons.begin();
                fatherCopy->sons.erase(iter + i);

                fatherCopy = nullptr;
                delete fatherCopy;

                return;
            }
        }
    }
}

void DeleteTree(Tree*& root)
{
    if (root == NULL)
    {
        std::cout << "Дерево пустое" << std::endl;
        return;
    }

    for (int i = root->sons.size() - 1; i >= 0; i--)
    {
        DeleteTree(root->sons[i]);
    }

    if (root->father)
    {
        Tree* fatherCopy = root->father;

        for (int i = fatherCopy->sons.size() - 1; i >= 0; i--)
        {
            root = nullptr;
            delete root;

            auto iter = fatherCopy->sons.begin();
            fatherCopy->sons.erase(iter + i);

            fatherCopy = nullptr;
            delete fatherCopy;

            return;
        }
    }
    else
    {   
        root = nullptr;
    }

}

void PrintMenu()
{
    std::cout << "1. Вывести дерево" << std::endl;
    std::cout << "2. Удалить копии файла" << std::endl;
    std::cout << "3. Удалить дерево" << std::endl;
}

int ModeInput()
{   
    int mode;
    std::cin >> mode;
    return mode;
}

int MenuInput(Tree*& root)
{
    int mode = ModeInput();
    if (mode == 1)
    {   
        PrintTree(root, 0);
    }
    else if (mode == 2)
    {   
        std::string nodeName;
        std::cout << "Введите имя файла для удаления" << std::endl;
        std::cin >> nodeName;
        Time fileTime;
        fileTime.day = 0;
        fileTime.month = 0;
        fileTime.year = 0;
        fileTime = FindFirstOccurrence(root, nodeName)->time;

        FindMostNewFile(root, nodeName, fileTime);

        DeleteFilesCopy(root, nodeName, fileTime);
    }
    else if (mode == 3)
    {
        DeleteTree(root);
    }
    else
    {
        std::cout << "Неверный ввод, повторите попытку" << std::endl;
    }

    return 0;

}