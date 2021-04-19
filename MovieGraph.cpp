#include "MovieGraph.h"

void MovieGraph::GetInfo() {
    unordered_map<string, Movie*> tmpIdMap;
	ifstream istr;
	smatch matches;
	istr.open("../datasets/moviesWithRatings.csv");
	if (istr.is_open()) {
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
            regex_search(line, matches, regex("(\\d+),(.+),(\\d+),(.+),(\\d+)"));
            if(!matches.empty()) {
                Movie *newMovie = new Movie(stoi(matches[1]), matches[2], stoi(matches[3]), stod(matches[4]),
                                            stoi(matches[5]));
                movieMap.emplace(matches[2], newMovie);
                tmpIdMap.emplace(matches[1], newMovie);
            }
		}
	}
	istr.close();
    unordered_map<int, string> genreNames;
	istr.open("../datasets/genres.csv");
	if (istr.is_open()) {
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
		    istringstream i(line);
		    string id;
		    string name;
            getline(i, id, ',');
            getline(i, name, ',');
            genreNames.emplace(stoi(id), name);
            genreMap.emplace(name, vector<Movie*>{});
		}
	}
	istr.close();
    istr.open("../datasets/movie_genres.csv");
    if (istr.is_open()) {
        string line;
        getline(istr,line);
        while(getline(istr, line)) {
            istringstream i(line);
            string movieId;
            string genreId;
            getline(i, movieId, ',');
            getline(i, genreId, ',');
            string genreName = genreNames[stoi(genreId)];
            auto iter = tmpIdMap.find(movieId);
            if(iter != tmpIdMap.end()) {
                genreMap[genreName].push_back(iter->second);
                iter->second->addGenre(genreName);
            }
        }
    }
    istr.close();
	istr.open("../datasets/tagscores.csv");
	if (istr.is_open()) {
        string line;
        getline(istr, line);
        while (getline(istr, line)) {
            string tmpID;
            string tagID;
            string score;
            istringstream str(line);
            getline(str, tmpID, ',');
            getline(str, tagID, ',');
            getline(str, score, ',');
            auto movieIter = tmpIdMap.find(tmpID);
            if(movieIter != tmpIdMap.end()) {
                movieIter->second->addTag(stoi(tagID), stod(score));
                auto iter = tagMap.find(stoi(tagID));
                if (iter != tagMap.end()) {
                    iter->second.push_back(movieIter->second);
                } else {
                    vector<Movie *> vect{movieIter->second};
                    tagMap.emplace(stoi(tagID), vect);
                }
            }
        }
    }
	istr.close();
	istr.open("../datasets/links.csv");
	if(istr.is_open()) {
	    string line;
	    getline(istr,line);
	    while(getline(istr, line)) {
	        string movieID;
	        string imdbID;
	        istringstream i(line);
            getline(i, movieID, ',');
            getline(i, imdbID, ',');
            auto movieIter = tmpIdMap.find(movieID);
            if(movieIter != tmpIdMap.end()) {
                movieIter->second->addIMDB(imdbID);
            }
	    }
	}
}


