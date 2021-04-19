#include "Graph.h"

void Graph::GetInfo() {
	ifstream istr;
	istr.open("datasets/movies.csv");
	if (istr.is_open()) {
		cout << "Opened." << endl;
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
			string tmpId;
			string name;
			string genres;
			
			istringstream str(line);
			getline(str, tmpId, ',');
			if (line.find('"') == string::npos) {
				getline(str, name, ',');
				getline(str, genres, ',');
			}
			else {
				name = line.substr(line.find('\"'), line.find_last_of('\"') - tmpId.size());
				genres = line.substr(line.find_last_of(',') + 1);
			}
			int Id = stoi(tmpId);
			vector<double> tmp;

			Movie newMovie(Id, name, genres);
			movieMap.emplace(Id, newMovie);
			nameToID.emplace(name, Id);
			ratings.emplace(Id, tmp);
		}
	}
	else {
		cout << "Not Opened." << endl;
	}
	istr.close();

	istr.open("datasets/updatedRatings.csv");
	if (istr.is_open()) {
		cout << "Opened." << endl;
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
			string tmpID;
			string rating;

			istringstream str(line);
			getline(str, tmpID, ',');
			getline(str, rating, ',');
			int ID = stoi(tmpID);
			double rate = stod(rating);
			
			movieMap[ID].SetRating(rate);
		}
	}
	else {
		cout << "Not Opened." << endl;
	}
	istr.close();

	istr.open("datasets/tags.csv");
	if (istr.is_open()) {
		cout << "Opened." << endl;
		string line;
		getline(istr, line);

		while (getline(istr, line)) {
			string user;
			string tmpID;
			string tag;

			istringstream str(line);
			getline(str, user, ',');
			getline(str, tmpID, ',');
			getline(str, tag, ',');
			int ID = stoi(tmpID);

			movieMap[ID].tags.push_back(tag);
		}
	}
	else {
		cout << "Not Opened." << endl;
	}
	istr.close();
}

void Graph::PrintMovies() {
	for (unsigned int i = 0; i < movieMap.size(); i++) {
		movieMap[i].Print();
	}
}
