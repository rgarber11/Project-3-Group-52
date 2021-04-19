//
// Created by rgarber11 on 4/19/21.
//

#pragma once
#include "mpreal.h"
#include<vector>
#include <unordered_map>
#include <fstream>
#include <filesystem>
using mpfr::mpreal;
void filecreator() {
    std::ifstream movies;
    std::ofstream MOVIES ("../datasets/moviesNoRatings.csv");
    std::ofstream genre("../datasets/genres.csv");
    MOVIES << "id,title,year,rating\n";
    std::unordered_map<std::string,int> genres;
    genres.emplace(std::make_pair("Action", 0));
    genres.emplace(std::make_pair("Adventure", 1));
    genres.emplace(std::make_pair("Animation", 2));
    genres.emplace(std::make_pair("Children's", 3));
    genres.emplace(std::make_pair("Comedy", 4));
    genres.emplace(std::make_pair("Crime", 5));
    genres.emplace(std::make_pair("Documentary", 6));
    genres.emplace(std::make_pair("Drama", 7));
    genres.emplace(std::make_pair("Fantasy", 8));
    genres.emplace(std::make_pair("Film-Noir", 9));
    genres.emplace(std::make_pair("Horror", 10));
    genres.emplace(std::make_pair("Musical", 11));
    genres.emplace(std::make_pair("Mystery", 12));
    genres.emplace(std::make_pair("Romance", 13));
    genres.emplace(std::make_pair("Sci-Fi", 14));
    genres.emplace(std::make_pair("Thriller", 15));
    genres.emplace(std::make_pair("War", 16));
    genres.emplace(std::make_pair("Western", 17));
    genre << "id,genre\n";
    for(auto i : genres) {
        genre << i.second << "," << i.first << "\n";
    }
    genre.close();
    movies.open("../ml-25m/movies.csv");
    std::string input;
    getline(movies, input);
    std::smatch matches;
    std::regex movieRegex("(\\d+),(.+) \\((\\d{4})\\),(.+)");
    std::ofstream movie_genres("movie_genres.csv");
    movie_genres << "movie,genre\n";
    while(getline(movies, input)) {
        std::regex_search(input, matches, movieRegex);
        if (!matches.empty()) {
            MOVIES << matches[1] << "," << matches[2] << "," << matches[3] << ",\n";
            std::istringstream istringstream(matches[4].str());
            std::string genre;
            while (getline(istringstream, genre, '|')) {
                auto iter = genres.find(genre);
                if (iter != genres.end()) {
                    movie_genres << matches[1] << "," << iter->second << "\n";
                }
            }
        }
    }
    movies.close();
    movie_genres.close();
    std::ifstream ratings("../ml-25m/ratings.csv");
    std::unordered_map<int,std::vector<mpreal>> ratingAvg;
    std::regex ratingRegex("\\d+,(\\d+),(.+),.+");
    std::string line;
    getline(ratings, line);
    while(getline(ratings, line)) {
        std::regex_search(line,matches,ratingRegex);
        if(!matches.empty()) {
            auto iter = ratingAvg.find(stoi(matches[1]));
            if(iter == ratingAvg.end())  {
                mpreal val = stod(matches[2]);
                std::vector<mpreal> vect;
                vect.push_back(val);
                ratingAvg.emplace(std::make_pair(stoi(matches[1]), vect));
            } else {
                mpreal val = stod(matches[2]);
                iter->second.push_back(val);
            }
        }
    }
    ratings.close();
    std::ifstream movieOG("../datasets/moviesNoRatings.csv");
    std::ofstream movieNew("../datasets/moviesWithRatings.csv");
    getline(movieOG,line);
    movieNew << "id,title,year,rating,popularity\n";
    std::regex finalMovieRegex ("(\\d+),.+");
    while(getline(movieOG,line)) {
        std::regex_search(line, matches, finalMovieRegex);
        auto iter = ratingAvg.find(stoi(matches[1].str()));
        if(iter == ratingAvg.end()) {
            movieNew << line << "\n";
        } else {
            mpreal i = 1.0;
            for (mpreal j : iter->second) {
                i*=j;
            }
            mpreal geometric_mean;
            geometric_mean = pow(i, (mpreal) 1.0/iter->second.size());
            movieNew << line << geometric_mean << "," << iter->second.size() << "\n";
        }
    }
    movieNew.close();
    movieOG.close();
    std::ifstream tags("../ml-25m/tags.csv");
    std::ifstream hash("../ml-25m/genome-tags.csv");
    std::unordered_map<int, std::unordered_set<int>> tagdb;
    std::unordered_map<std::string, int> tagToID;
    getline(hash, line);
    std:: regex  genometag ("(\\d+),(.+)");
    while(getline(hash, line)) {
        std::regex_search(line, matches, genometag);
        tagToID.emplace(std::make_pair(matches[2], stoi(matches[1])));
    }
    hash.close();
    std::regex tagsCSV ("\\d+,(\\d+),(.+),.+");
    getline(tags, line);
    while(getline(tags,line)) {
        std::regex_search(line, matches, tagsCSV);
        int tagid = tagToID[matches[2]];
        auto iter = tagdb.find(stoi(matches[1]));
        if(iter == tagdb.end()) {
            std::unordered_set<int> tags;
            tags.insert(tagid);
            tagdb.emplace(std::make_pair(stoi(matches[1]),tags));
        } else {
            iter->second.insert(tagid);
        }
    }
    tags.close();
    std::ifstream scores("../ml-25m/genome-scores.csv");
    std::regex scorer ("(\\d+),(\\d+),(.+)");
    std::ofstream tagscores("tagscores.csv");
    getline(scores,line);
    tagscores << "movieid,tagid,score\n";
    auto iter = tagdb.begin();
    while(getline(scores,line)) {
        std::regex_search(line, matches, scorer);
        auto iter = tagdb.find(stoi(matches[1]));
        if (iter != tagdb.end()) {
            if (iter->second.find(stoi(matches[2])) != iter->second.end()) {
                tagscores << matches[0] << "\n";
            }
        }
    }
    scores.close();
    tagscores.close();
    std::filesystem::copy("../ml-25m/genome-tags.csv", "../datasets/genome-tags.csv");
    std::filesystem::copy("../ml-25m/links.csv", "../datasets/links.csv");
};
