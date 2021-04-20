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
		return (iter == movieMap.end()) ? nullptr : iter->second; //Simple to see if the movie exists.
	}
	const vector<Movie*> getGenreMovies(string genre) { //returns all movies in one genre
	    vector<Movie*> genreVector = genreMap[genre];

        return genreVector;

	}


	vector<Movie*>  recommendationAlgorithm(string name, int steps) {
	    if(steps == 0) {
	        return vector<Movie*>{movieMap[name]}; //No steps no go
	    } else {
	        Movie* og = movieMap[name]; //Start with the source
	        unordered_set<string> traversed; //Where have we been
	        traversed.insert(name);
            unordered_map<string, double> rater; //How close is everybody
            vector<Movie*> recommended; //All the movies
            for(auto iter : og->tags) { //Do this for all tags
                vector<Movie*> tagged = tagMap[iter.first]; //find all the tagged movies
                for(Movie* similar : tagged) { //Go through them
                    if(traversed.find(similar->name) == traversed.end()) { //if we've used them for traversing, we don't need them
                        double addition = similar->popularity > 1500 ? 1 : .9; //Super unpopular movies are rated lower
                        addition *= (.5) / (abs(similar->tags[iter.first] - iter.second) + .1); //algorithm where similar weights in tags are proritized.
                        auto raterIter = rater.find(similar->name); //Have we dealt with this movie befor?
                        if (raterIter != rater.end()) { //If we have, then we don't need to re add it to the vector
                            raterIter->second += addition;
                        } else {
                            addition *= similar->rating; //If we haven't seen it, we need it's rating
                            recommended.push_back(similar);
                            rater.emplace(similar->name, addition);
                        }
                    }
                }
            }
            std::sort(recommended.begin(), recommended.end(), [&](const Movie* lhs, const Movie* rhs){ //lambda comparator with unordered map
                double lrater = rater[lhs->name];
                double rrater = rater[rhs->name];
                return lrater > rrater;
            });

            for(int i = 1; i < steps; i++) {
                og = recommended[i-1]; //take then next lowest number
                int offset = 1;
                while(traversed.find(og->name) != traversed.end()) { //make sure we haven't somehow been there
                    og = recommended[i-1+offset];
                    offset++;
                }
                traversed.insert(og->name); //Do the last thing all over again
                for(auto iter : og->tags) {
                    vector<Movie*> tagged = tagMap[iter.first];
                    for(Movie* similar : tagged) {
                        if (traversed.find(similar->name) == traversed.end()) {
                            double addition = similar->popularity > 1500 ? 1.0 / (i + 1) : .9 / (i + 1); //Since we're further away these are worth less. Quasi Zipf's law
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
                std::sort(recommended.begin(), recommended.end(), [&](const Movie* lhs, const Movie* rhs){ //Resort for next time
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
