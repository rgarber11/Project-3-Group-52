#pragma once
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <map>
#include "Movie.h"

class Graph {
private:
	unordered_map<string, int> nameToID;
	unordered_map<int, Movie> movieMap;
	unordered_map<int, vector<double>> ratings;
public:
	void GetInfo(); //Parses data, fills unordered maps.
	void PrintMovies();
	Movie GetMovie(string name) {
		try {
			int tmp = nameToID.at(name);
			return movieMap.at(tmp);
		}
		catch (out_of_range& excpt) {
			cout << "Movie not found." << endl;
		}
	}
	Movie GetMovie(int ID) {
		try {
			return movieMap.at(ID);
		}
		catch (out_of_range& excpt) {
			cout << "Movie not found." << endl;
		}
	}
};
