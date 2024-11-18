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
#include <stack>
#include <queue>

struct Tree
{
    std::string name;
    int level;
    Tree* father = nullptr;
    std::vector<Tree*> sons;
};

int ReadFromFile(std::ifstream& inFile, Tree*& root);
void PrintTree(Tree* root, int level);
void PrintMenu();
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


int ReadFromFile(std::ifstream& inFile, Tree*& root)
{   
    std::string buffer;
    int j, k, fatherLevel, nameLength;
    Tree *newNode, *prevFather, *father;
    fatherLevel = -1;
    father = NULL;
    while (inFile >> buffer)
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
        while (j < nameLength)
        {
            newNode->name += buffer[j];
            j++;
        }
        newNode->level = k;
        if (k == 0)
        {
            root = newNode;
            father = root;
            fatherLevel = 0;
            continue;
        }
        if (k > fatherLevel)
        {
            father->sons.push_back(newNode);
            newNode->father = father;
        }
        else if (k == fatherLevel)
        {
            prevFather = father->father;
            prevFather->sons.push_back(newNode);
            newNode->father = prevFather;
        }
        else
        {   
            prevFather = father;
            for (int i = 0; i <= fatherLevel - k; i++) prevFather = prevFather->father;
            prevFather->sons.push_back(newNode);
            newNode->father = prevFather;
        }
        fatherLevel = k;
        father = newNode;
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
    std::cout << root->name << std::endl;
    for (int i = 0; i < root->sons.size(); i++)
        PrintTree(root->sons[i], level + 1);
}

void DeleteFilesCopy(Tree*& root, std::string name, bool& isCopy)
{   
    if (root == NULL)
    {   
        std::cout << "Дерево пустое" << std::endl;
        return;
    }

    for (int i = root->sons.size() - 1; i >= 0; i--)
    {
        DeleteFilesCopy(root->sons[i], name, isCopy);
    }
    
    if (root->name == name)
    {
        if (isCopy == false)
        {
            isCopy = true;
        }
        else
        {   
            Tree* fatherCopy = root->father;

            for (int i = fatherCopy->sons.size() - 1; i >= 0; i--)
            {   
                if (fatherCopy->sons[i] == root)
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

void PrintMenu()
{
    std::cout << "1. Вывести дерево" << std::endl;
    std::cout << "2. Удалить копии файла" << std::endl;
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
        bool isCopyExist = false;

        DeleteFilesCopy(root, nodeName, isCopyExist);
    }
    else
    {
        std::cout << "Неверный ввод, повторите попытку" << std::endl;
    }

    return 0;

}