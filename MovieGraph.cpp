#include "MovieGraph.h"

void MovieGraph::GetInfo() { //Parse post preparser
    unordered_map<string, Movie*> tmpIdMap; //maps are technically O(n) worst case
	ifstream istr;
	smatch matches;
	istr.open("../datasets/moviesWithRatings.csv"); //first we go through m lines load movies, years, ratings, pop
	if (istr.is_open()) {
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
            regex_search(line, matches, regex("(\\d+),(.+),(\\d+),(.+),(\\d+)")); //this comparison is the length of a line i. O(m*i)
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
	istr.open("../datasets/genres.csv"); //THis is tiny, so matters less genre id numbers if database were used
	if (istr.is_open()) {
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
		    std::regex_search(line, matches, regex("(\\d+),(.+)"));
            genreNames.emplace(stoi(matches[1]), matches[2]);
            genreMap.emplace(matches[2], vector<Movie*>{});
		}
	}
	istr.close();

    istr.open("../datasets/movie_genres.csv"); //Fairly large worst case O(g*m) All genres that movies are
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
            auto iter = tmpIdMap.find(movieId); //O(m) technically, so O(g*m^2)
            if(iter != tmpIdMap.end()) {
                genreMap[genreName].push_back(iter->second);
                iter->second->addGenre(genreName);
            }
        }
    }
    istr.close();

	istr.open("../datasets/tagscores.csv"); //Pretty large O(t*m) all tags that movies are plus how much
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
            auto movieIter = tmpIdMap.find(tmpID); //O(m) O(t*m^2)
            if(movieIter != tmpIdMap.end()) {
                movieIter->second->addTag(stoi(tagID), stod(score));
                auto iter = tagMap.find(stoi(tagID)); //O(t) worstcase so O(t^2*m^2)
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

	istr.open("../datasets/links.csv"); //O(m) //IMDB number
	if(istr.is_open()) {
	    string line;
	    getline(istr,line);
	    while(getline(istr, line)) {
	        string movieID;
	        string imdbID;
	        istringstream i(line);
            getline(i, movieID, ',');
            getline(i, imdbID, ',');
            auto movieIter = tmpIdMap.find(movieID); //O(m) so O(m^2(
            if(movieIter != tmpIdMap.end()) {
                movieIter->second->addIMDB(imdbID);
            }
	    }
	}
	for(auto iter : genreMap) { //O(n*log n) //sort all genre in advance
        std::sort(iter.second.begin(), iter.second.end(), [](Movie* lhs, Movie* rhs) {
            double lAdjRating = lhs->popularity > 1500 ? 1 : .75;
            double rAdjRating = rhs->popularity > 1500 ? 1 : .75;
            lAdjRating*=lhs->rating;
            rAdjRating*=rhs->rating;
            return lAdjRating>rAdjRating;
        });
	}
}


