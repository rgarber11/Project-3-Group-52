#pragma once
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class Movie {
private:
    string name;
    int movieID = 0;
    int releaseYear = 0;
    int rating = 0;
    int popularity = 0;
    vector<int> genres;

public:
    Movie() {}
    Movie(int ID, string nm, int release, int rate, int pop) {
        movieID = ID;
        name = nm;
        releaseYear = release;
        rating = rate;
        popularity = pop;
    }
    Movie(const Movie& m1) { 
        movieID = m1.movieID;
        name = m1.name;
        releaseYear = m1.releaseYear;
        rating = m1.rating;
        popularity = m1.popularity;
        genres = m1.genres;
    }
    void SetGenres(vector<int> _genres);
    vector<int> GetGenres();
    string GetName() { return name; }
    int GetID() { return movieID; }
};