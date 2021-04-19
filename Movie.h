#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

struct Movie {
    string name;
    int movieID = 0;
    double rating = 0;
    int popularity;
    int year;
    string imdb;
    vector<string> genres;
    unordered_map<int, double> tags;
    Movie() {}
    Movie(int ID, string nm, int _year, double _rating, int _popularity) {
        movieID = ID;
        name = nm;
        year = _year;
        rating = _rating;
        popularity = _popularity;
    }
    Movie(const Movie& m1) { 
        movieID = m1.movieID;
        name = m1.name;
        rating = m1.rating;
        genres = m1.genres;
        tags = m1.tags;
    }
    void addGenre(string genre) {genres.push_back(genre);}
    void addTag(int tag, double weight) {tags.emplace(tag, weight);}
    void addIMDB(string s) {imdb = s;};
};
