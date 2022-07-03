#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <queue>

struct EdgeWeight
{
    int _weight;
    bool _exists;
    EdgeWeight() : _weight(INT_MAX), _exists(false) {}
    EdgeWeight(int weight, bool exists) : _weight(weight), _exists(exists) {}
};

class Graph
{
private:
    const int _numOfVertices;
    int _numOfEdges;
    EdgeWeight** adjMatrix;
    std::map <int, int> convert; //нужно для перевода числа имени вершины в индекс массива для матрицы смежности

public:
    Graph() : _numOfVertices(10), _numOfEdges(0) //дефолтный размер графа
    {
        adjMatrix = new EdgeWeight * [_numOfVertices];
        for (int i = 0; i < _numOfVertices; i++)
        {
            adjMatrix[i] = new EdgeWeight[_numOfVertices];
        }
    }
    Graph(int numOfVert) : _numOfVertices(numOfVert), _numOfEdges(0)
    {
        adjMatrix = new EdgeWeight * [_numOfVertices];
        for (int i = 0; i < _numOfVertices; i++)
        {
            adjMatrix[i] = new EdgeWeight[_numOfVertices];
        }
    }
    Graph(const Graph& other) : _numOfEdges(0), _numOfVertices(other._numOfVertices), convert(other.convert)
    {
        adjMatrix = new EdgeWeight * [_numOfVertices];
        for (int i = 0; i < _numOfVertices; i++)
        {
            adjMatrix[i] = new EdgeWeight[_numOfVertices];
        }

        for (int i = 0; i < _numOfVertices; i++)
        {
            for (int j = 0; j < _numOfVertices; j++)
            {
                adjMatrix[i][j] = other.adjMatrix[i][j];
            }
        }
    }
    ~Graph()
    {
        for (int i = 0; i < _numOfVertices; i++)
        {
            delete[] adjMatrix[i];
        }
        delete[] adjMatrix;
    }

    //Добавление имени вершины (для словаря convert). Возвращает true, если добавление удалось, и false в ином случае
    bool addVertex(int vert)
    {
        if (convert.size() < _numOfVertices)
        {
            convert[vert] = convert.size();
            return true;
        }
        else return false;
    }

    //Добавление ребер в матрицу смежности. Возвращает true, если добавление удалось, и false в ином случае
    bool addEdge(int vert1, int vert2, int weight)
    {
        if ((weight <= INT_MAX / 2) && convert.contains(vert1) && convert.contains(vert2)) //Вес не может быть больше INT_MAX/2, а вершины vert1 и vert2 должны существовать в графе
        {
            adjMatrix[convert[vert1]][convert[vert2]]._weight = weight;
            adjMatrix[convert[vert1]][convert[vert2]]._exists = true;
            ++_numOfEdges;
            return true;
        }
        else return false;
    }

    void BFS(const int vert1, const int vert2)
    {
        if ((convert.count(vert1) == 1) && (convert.count(vert2) == 1) && (adjMatrix[convert[vert1]][convert[vert2]]._exists == true))
        {
            std::queue<std::pair<int, int>> Queue; //очередь для накопителя
            std::map<std::pair<int, int>, int> edges; //если дуга содержится в этой карте, то она уже встречалось при обходе
            std::pair<int, int> edge;

            Queue.push({ vert1, vert2 });
            std::cout << "------" << std::endl;
            while (!Queue.empty())
            {
                edge = Queue.front(); Queue.pop(); //извлекаем из очереди дугу
                edges[edge] = true; //метка, дуга встретилось
                for (auto& it : convert)
                {
                    if ((adjMatrix[convert[edge.second]][it.second]._exists == true) && !(edges.contains({edge.second, it.first}))) //смотрим дуги исходящие из конца дуги edge
                    {
                        Queue.push({ edge.second, it.first });
                    }
                }
                std::cout << edge.first << " -> " << edge.second << std::endl;
            }
            std::cout << "------" << std::endl;
        }
        else std::cout << "В графе нет заданных вершин или нет дуги из первой вершины во вторую" << std::endl;

    }

    //Вспомогательная функция
    void printMatrix(EdgeWeight** matrix)
    {
        std::cout << std::setw(3) << "\t";
        for (auto& it : convert)
        {
            std::cout << std::setw(3) << it.first << " ";
        }
        std::cout << std::endl << std::endl;
        for (auto& it1 : convert)
        {
            std::cout << std::setw(3) << it1.first << "\t";
            for (auto& it2 : convert)
            {
                if (matrix[it1.second][it2.second]._exists)
                {
                    std::cout << std::setw(3) << matrix[it1.second][it2.second]._weight << " ";
                }
                else std::cout << "inf ";
            }
            std::cout << std::endl;
        }
    }

    //Печать графа (в виде матрицы смежности)
    void print()
    {
        std::cout << "-----------------------" << std::endl;
        printMatrix(adjMatrix);
        std::cout << "-----------------------" << std::endl;
    }

    //Печать матрицы кратчайших путей (по алгоритму Флойда)
    void FloydAlgorithmPrint()
    {
        EdgeWeight** matrix = FloydAlgorithm();
        std::cout << "-----------------------" << std::endl;
        printMatrix(matrix);
        std::cout << "-----------------------" << std::endl;

        for (int i = 0; i < _numOfVertices; i++)
        {
            delete[] matrix[i];
        }
        delete[] matrix;
    }

    int getNumOfVert()
    {
        return _numOfVertices;
    }

private:
    //Алгоритм Флойда, возвращает матрицу кратчайших путей
    EdgeWeight** FloydAlgorithm()
    {
        //создаем и заполняем матрицу смежности (копируем ту, что на которой построен граф и изменяем главную диагональ на нули) для работы с алгоритмом
        EdgeWeight** temp = new EdgeWeight * [_numOfVertices];
        for (int i = 0; i < _numOfVertices; i++)
        {
            temp[i] = new EdgeWeight[_numOfVertices];
        }
        for (int i = 0; i < _numOfVertices; i++)
        {
            for (int j = 0; j < _numOfVertices; j++)
            {
                temp[i][j] = adjMatrix[i][j];
            }
        }
        for (int i = 0; i < _numOfVertices; i++)
        {
            temp[i][i] = EdgeWeight(0, true);
        }

        //сам алгоритм
        for (int k = 0; k < _numOfVertices; k++)
            for (int i = 0; i < _numOfVertices; i++)
                for (int j = 0; j < _numOfVertices; j++)
                {
                    if ((temp[i][k]._exists) && (temp[k][j]._exists) &&
                        (temp[i][j]._weight >= temp[i][k]._weight + temp[k][j]._weight))
                    {
                        temp[i][j]._weight = temp[i][k]._weight + temp[k][j]._weight;
                        temp[i][j]._exists = true;
                    }
                }

        return temp;
    }
};

void split(std::string s, char delim, std::vector<std::string>& data)
{
    while (!s.empty())
    {
        auto word_end = s.find(delim);
        if (word_end == -1)
        {
            data.push_back(s);
            s.clear();
        }
        else
        {
            data.push_back(s.substr(0, word_end));
            s.erase(0, word_end + 1);
        }
    }
}

//Заполнение графа из файла input
Graph fillGraphfrom(std::ifstream& input)
{
    std::string temp;
    std::vector<std::string> numVertAndEdge;
    std::vector<std::string> vertices;
    std::vector<std::string> edgesWithWeights;
    std::getline(input, temp);
    split(temp, ' ', numVertAndEdge);
    Graph graph(stoi(numVertAndEdge[0]));
    std::getline(input, temp);
    split(temp, ' ', vertices);
    for (int i = 0; i < graph.getNumOfVert(); i++)
    {
        graph.addVertex(stoi(vertices[i]));
    }
    for (int i = 0; i < stoi(numVertAndEdge[1]); i++)
    {
        std::getline(input, temp);
        split(temp, ' ', edgesWithWeights);
        graph.addEdge(stoi(edgesWithWeights[0]), stoi(edgesWithWeights[1]), stoi(edgesWithWeights[2]));
        edgesWithWeights.clear();
    }
    return graph;
}

int main()
{
    setlocale(LC_ALL, "rus");
    std::ifstream input("inputVerticesAndEdges.txt");
    Graph graph = fillGraphfrom(input);
    graph.print();
    graph.FloydAlgorithmPrint();
    graph.print();
    graph.FloydAlgorithmPrint();
    graph.print();
    graph.BFS(4, 6);
}