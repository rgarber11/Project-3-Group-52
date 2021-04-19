#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

class Movie {
private:
    string name;
    int movieID = 0;
    double rating = 0;
public:
    vector<string> genres;
    vector<string> tags;
    Movie() {}
    Movie(int ID, string nm, string genresToSort) {
        movieID = ID;
        name = nm;
        istringstream str(genresToSort);
        string tmp;
        while (!getline(str, tmp, '|').eof()) {
            genres.push_back(tmp);
        }
        genres.push_back(tmp); //Needs to occur twice due to nature of eof: does not enter loop for last genre, or if there's only 1.
    }
    Movie(const Movie& m1) { 
        movieID = m1.movieID;
        name = m1.name;
        rating = m1.rating;
        genres = m1.genres;
    }
    void SetRating(double rated) { rating = rated; }
    string GetName() { return name; }
    int GetID() { return movieID; }
    void Print() {
        if (movieID != 0) {
            cout << movieID << "|" << name << "|" << rating << "|";
            for (unsigned int i = 0; i < genres.size(); i++) {
                if (i == genres.size() - 1) {
                    cout << genres[i];
                }
                else {
                    cout << genres[i] << ", ";
                }
            }
            cout << endl;
        }
    }
};
