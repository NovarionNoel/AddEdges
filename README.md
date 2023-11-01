#AddEdges

This program takes graph input from various files and converts it into a chordal graph by adding edges until the graph is chordal.

##How To Run

Compile using c++17 or higher:

`g++ --std=c++17 AddEdges.cpp -o AddEdges`

Run by supplying a file and optionally entering the number of input lines to skip at the start of the file.
Arguments must be supplied in order.

Example:

`./AddEdges -f [filename] [OPTIONAL -s [LinesToSkip]]`
