#include <unordered_map>
#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>
#include <cstring>
#include <vector>
#include <unordered_set>

using namespace std;
/*
Adding edges
attempt elimination order
any nodes in elimination order can be skipped
calculate clustering coefficient
(n*(n-1))/2 - e
fill-in
attempt elimination order
finding minimum fill in is NP complete (go cry about it)
*/
// file types
#define TXT 11
#define MTX 12
#define EDGES 13
#define NODES 14
// function signatures
void fillInEdges(unordered_map<int, unordered_map<int, int>> &AdjacencyList, int node);
int findLowestClusteringCoefficient(unordered_map<int, unordered_map<int, int>> &AdjacencyList, unordered_map<int, bool> &eliminationOrder, unordered_map<int, float> &clusteringCoefficients);

unordered_map<int, unordered_map<int, int>> createAdjacencyList(fstream *file, int type, int skip)
{
   unordered_map<int, unordered_map<int, int>> AdjacencyList;
   string eatLines;
   int x = 0, y = 0, w = 0;

   switch (type)
   {
   case TXT:
      for (int i = 0; i < skip; i++)
      {
         getline(*file, eatLines);
      }
      // read edge pairs
      // use first coord as first vertex
      // use second coord as second vertex
      // third coord is weight

      while (*file >> x >> y)
      {

         AdjacencyList[x].insert(make_pair(y, w));
      }
      break;
   case MTX:
      // do stuff
      for (int i = 0; i < skip; i++)
      {
         getline(*file, eatLines);
      }
      // unweighted mtx
      while (*file >> x >> y)
      {
         AdjacencyList[x].insert(make_pair(y, w));
      }
      break;
   case EDGES:
      for (int i = 0; i < skip; i++)
      {
         getline(*file, eatLines);
      }

      while (*file >> x >> y)
      {
         AdjacencyList[x].insert(make_pair(y, w));
      }
      break;
   default:
      // give me a supported file type.
      cout << "File type not supported. Try again." << endl;
      exit(2);
      break;
   }
   return AdjacencyList;
}

bool isEliminationOrder(unordered_map<int, bool> &eliminationOrder)
{
   for (const auto &[node, eliminated] : eliminationOrder)
   {
      if (!eliminated)
      {
         return false;
      }
   }
   return true;
}

bool inEliminationOrder(unordered_map<int, unordered_map<int, int>> &AdjacencyList, int nodeCheck)
{
   unordered_set<int> eliminatedNodes;
   for (int i = 0; i < AdjacencyList.size(); i++)
   {
      int minDegreeNode = -1;
      int minDegree = numeric_limits<int>::max();
      for (const auto &nodeSet : AdjacencyList)
      {
         int node = nodeSet.first;
         if (eliminatedNodes.find(node) == eliminatedNodes.end())
         {
            int degree = nodeSet.second.size();
            if (degree < minDegree)
            {
               minDegree == degree;
               minDegreeNode = node;
            }
         }
      }
      if (minDegreeNode == -1)
      {
         return false;
      }
      eliminatedNodes.insert(minDegreeNode);
      if (nodeCheck == minDegreeNode)
      {
         return true;
      }
   }
   return false;
}

void addToEliminationOrder(unordered_map<int, unordered_map<int, int>> &AdjacencyList, unordered_map<int, bool> &eliminationOrder)
{
   for (const auto &[node, edgeSet] : AdjacencyList)
   {
      eliminationOrder[node] = inEliminationOrder(AdjacencyList, node);
   }
}

float clusteringCoefficient(unordered_map<int, unordered_map<int, int>> &AdjacencyList, int node)
{
   unordered_map<int, int> &neighbors = AdjacencyList.at(node);
   int edges = 0;
   int possibleEdges = neighbors.size() * (neighbors.size() - 1);
   if (possibleEdges == 0)
   {
      return 0.0;
   }
   for (const auto &pair : neighbors)
   {
      int neighbor = pair.first;
      for (const auto &otherPair : neighbors)
      {
         int anotherNeighbor = otherPair.first;
         if (neighbor != anotherNeighbor && AdjacencyList.find(neighbor) != AdjacencyList.end() && AdjacencyList.at(neighbor).count(anotherNeighbor))
         {
            edges++;
         }
      }
   }
   return static_cast<float>((edges) / static_cast<float>(possibleEdges));
}
string getCurrentDateTime(const string &format)
{
   auto now = chrono::system_clock::now();
   auto in_time_t = chrono::system_clock::to_time_t(now);

   stringstream ss;
   ss << put_time(localtime(&in_time_t), format.c_str());
   return ss.str();
}

void logError(const string &message)
{
   cerr << message << endl;
   fstream logFile("error_log_" + getCurrentDateTime("%Y%m%d_%H%M%S") + ".log", ios::out);
   if (logFile.is_open())
   {
      logFile << getCurrentDateTime("%Y-%m-%d %H:%M:%S") << ": " << message << endl;
   }
}

fstream singleOpen(const string &filename)
{
   const int maxAttempts = 3;
   int attempts = 0;
   fstream file;

   while (attempts < maxAttempts && !file.is_open())
   {
      file.open(filename, ios::in);
      if (!file.is_open())
      {
         cout << "Failed to open file (attempt " << (attempts + 1) << "). Retrying..." << endl;
         this_thread::sleep_for(chrono::seconds(1));
         attempts++;
      }
      else
      {
         cout << filename << " opened successfully \n";
      }
   }

   if (!file.is_open())
   {
      string errorMessage = "Error: Failed to open file after " + to_string(maxAttempts) + " attempts.";
      logError(errorMessage);
      exit(1); // or throw an exception
   }

   return file;
}

void printAdjList(unordered_map<int, unordered_map<int, int>> adjList)
{
   for (auto const &[node, edgeSet] : adjList)
   {
      cout << "NODE: " << node << "\n";
      cout << "EDGES: ";
      for (auto const &[edge, weight] : edgeSet)
      {
         cout << edge << " w: " << weight << ", ";
      }
      cout << "\n";
   }
}

void edgeAddingAlgorithm(unordered_map<int, unordered_map<int, int>> &AdjacencyList, unordered_map<int, bool> &eliminationOrder, unordered_map<int, float> &clusteringCoefficients)
{

   // first, add to elimination order
   addToEliminationOrder(AdjacencyList, eliminationOrder);

   // then, check if ordering is valid
   if (isEliminationOrder(eliminationOrder))
   {
      return;
   }

   // select next node by clustering coefficient
   int nextNode = findLowestClusteringCoefficient(AdjacencyList, eliminationOrder, clusteringCoefficients);

   // as long as a non-zero result was found, continue algorithm
   if (nextNode != -1)
   {
      fillInEdges(AdjacencyList, nextNode);
   }

   // recursively call until complete
   edgeAddingAlgorithm(AdjacencyList, eliminationOrder, clusteringCoefficients);
}

void fillInEdges(unordered_map<int, unordered_map<int, int>> &AdjacencyList, int node)
{
   unordered_map<int, int> neighbors = AdjacencyList.at(node);
   for (const auto &pair : neighbors)
   {
      int neighbor = pair.first;
      for (const auto &otherPair : neighbors)
      {
         int anotherNeighbor = otherPair.first;
         if (neighbor != anotherNeighbor && AdjacencyList[neighbor].count(anotherNeighbor) == 0)
         {
            AdjacencyList[node][anotherNeighbor] = 0;
            AdjacencyList[anotherNeighbor][node] = 0;
         }
      }
   }
}

int findLowestClusteringCoefficient(unordered_map<int, unordered_map<int, int>> &AdjacencyList, unordered_map<int, bool> &eliminationOrder, unordered_map<int, float> &clusteringCoefficients)
{
   int lowestCC = 2;
   int lowestNode = -1;
   for (const auto &[node, edgeSet] : AdjacencyList)
   {
      if (eliminationOrder[node] == false)
      {
         clusteringCoefficients[node] = clusteringCoefficient(AdjacencyList, node);
         if (lowestCC > clusteringCoefficients[node] && clusteringCoefficients[node] != 0)
         {
            lowestCC = clusteringCoefficients[node];
            lowestNode = node;
         }
      }
   }
   return lowestNode;
}

int main(int argc, char *argv[])
{
   int skip = 0;
   if (argc >= 3)
   {
      unordered_map<int, unordered_map<int, int>> AdjacencyList;
      if (strcmp(argv[1], "-f") == 0)
      {
         // open file
         fstream combinedGraph = singleOpen(argv[2]);
         int type = 0;

         string extension(argv[2]);

         extension = (extension.substr(extension.find(".") + 1));

         if (extension.compare("txt") == 0)
         {
            type = TXT;
         }

         else if (extension.compare("edges") == 0)
         {
            type = EDGES;
         }

         else if (extension.compare("mtx") == 0)
         {
            type = MTX;
         }

         if (argc == 5 && strcmp(argv[3], "-s") == 0)
         {
            skip = stoi(argv[4]);
         }

         AdjacencyList = createAdjacencyList(&combinedGraph, type, skip);
         cout << "ORIGINAL LIST\n";
         printAdjList(AdjacencyList);

         // create elimination order
         unordered_map<int, bool> eliminationOrder;
         unordered_map<int, float> clusteringCoefficients;
         for (auto const &nodeSet : AdjacencyList)
         {
            eliminationOrder[nodeSet.first] = false;
            clusteringCoefficients[nodeSet.first] = 0;
         }
         edgeAddingAlgorithm(AdjacencyList, eliminationOrder, clusteringCoefficients);
         cout << "CHORDAL LIST\n";
         printAdjList(AdjacencyList);
      }
   }
   else
   {
      cout << "Invalid arguments. Please supply either a file with both nodes and edges." << endl;
      cout << "USAGE: ./AddEdges.exe -f filename  [OPTIONAL-s (number of lines to skip)]" << endl;
      exit(1);
   }
   return 0;
}
