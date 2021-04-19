#pragma once
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <map>
#include "Movie.h"

class MovieGraph {
private:
	unordered_map<string, Movie*> movieMap;
	unordered_map<string, vector<Movie*>> genreMap;
	unordered_map<int, vector<Movie*>> tagMap;
	unordered_map<int, vector<double>> ratings;
	struct MovieGreater {
	    bool operator()(Movie* lhs, Movie* rhs) {
	        return lhs->rating > rhs->rating;
	    }
	};
public:
	void GetInfo(); //Parses data, fills unordered maps.
	const Movie* GetMovie(string name) {
		auto iter = movieMap.find(name);
		return (iter == movieMap.end()) ? nullptr : iter->second;
	}
	const vector<Movie*> getGenreMovies(string genre) {
	    vector<Movie*> genreVector = genreMap[genre];
	    std::sort(genreVector.begin(), genreVector.end(), [](Movie* lhs, Movie* rhs) {
	        double lAdjRating = lhs->popularity > 1500 ? 1 : .75;
	        double rAdjRating = rhs->popularity > 1500 ? 1 : .75;
	        lAdjRating*=lhs->rating;
	        rAdjRating*=rhs->rating;
	        return lAdjRating>rAdjRating;
	    });
        return genreVector;

	}
	vector<Movie*>  recommendationAlgorithm(string name, int steps) {
	    if(steps == 0) {
	        return vector<Movie*>{movieMap[name]};
	    } else {
	        Movie* og = movieMap[name];
	        unordered_set<string> traversed;
	        traversed.insert(name);
            unordered_map<string, double> rater;
            vector<Movie*> recommended;
            for(auto iter : og->tags) {
                vector<Movie*> tagged = tagMap[iter.first];
                for(Movie* similar : tagged) {
                    if(traversed.find(similar->name) == traversed.end()) {
                        double addition = similar->popularity > 1500 ? 1 : .9;
                        addition *= (.5) / (abs(similar->tags[iter.first] - iter.second) + .1);
                        auto raterIter = rater.find(similar->name);
                        if (raterIter != rater.end()) {
                            raterIter->second += addition;
                        } else {
                            addition *= similar->rating;
                            recommended.push_back(similar);
                            rater.emplace(similar->name, addition);
                        }
                    }
                }
            }
            std::sort(recommended.begin(), recommended.end(), [&](const Movie* lhs, const Movie* rhs){
                double lrater = rater[lhs->name];
                double rrater = rater[rhs->name];
                return lrater > rrater;
            });
            for(int i = 1; i < steps; i++) {
                og = recommended[i-1];
                int offset = 1;
                while(traversed.find(og->name) != traversed.end()) {
                    og = recommended[i-1+offset];
                    offset++;
                }
                traversed.insert(og->name);
                for(auto iter : og->tags) {
                    vector<Movie*> tagged = tagMap[iter.first];
                    for(Movie* similar : tagged) {
                        if (traversed.find(similar->name) == traversed.end()) {
                            double addition = similar->popularity > 1500 ? 1.0 / (i + 1) : .9 / (i + 1);
                            addition *= (.5) / (abs(similar->tags[iter.first] - iter.second) + .1);
                            addition *= similar->rating;
                            auto raterIter = rater.find(similar->name);
                            if (raterIter != rater.end()) {
                                raterIter->second += addition;
                            } else {
                                recommended.push_back(similar);
                                rater.emplace(similar->name, addition);
                            }
                        }
                    }
                }
                std::sort(recommended.begin(), recommended.end(), [&](const Movie* lhs, const Movie* rhs){
                    double lrater = rater[lhs->name];
                    double rrater = rater[rhs->name];
                    return lrater > rrater;
                });
            }
            return recommended;
	    }
	}
	~MovieGraph() {
	    for(auto i : movieMap) {
	        delete i.second;
	    }
	}
};
