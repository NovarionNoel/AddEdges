#include <unordered_map>
#include <fstream>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>
#include <cstring>
#define TXT 11
#define MTX 12
#define EDGES 13
#define NODES 14

using namespace std;
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
