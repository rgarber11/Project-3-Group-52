#include <termox/termox.hpp>
#include <filesystem>
#include <termox/widget/widgets/button_list.hpp>
#include "MovieGraph.h"
#include "parser.h"
using namespace ox;


struct personalButtonList : public layout::Horizontal<> { //Button_List doesn't work, but this hybrid approach does for some reason
    struct personalButtonListInterior : public layout::Vertical<> {}; //Other buttons (empty to start)
    personalButtonListInterior& interior = this->make_child<personalButtonListInterior>();
    sl::Signal<void(const std::wstring &name)> button_pressed; //signal that allows for this to be useful

    auto add_button(std::wstring const &name) -> Button & { //Basically the code from the include file, but made to fit with this class
        auto &btn = this->interior.make_child<Button>(name) |
                    pipe::on_press([this, name] { button_pressed(name); });
        pipe::fixed_height(1);
        return btn;
    }
    Scrollbar<layout::Vertical<>>& scroll = this->make_child<Scrollbar<layout::Vertical<>>>();

    personalButtonList() {
        link(scroll, interior);
    } //If I remove the include, this breaks
};


struct MovieDispCentered : public layout::Horizontal<> { //Main class for seeing film data


    struct MovieDisp : public layout::Vertical<> { //Central display that contains our sauce
        Text_display &named = this->make_child<Text_display>();
        Text_display &padded = this->make_child<Text_display>(); //Padding for our design purposes
        Text_display &rated = this->make_child<Text_display>(); //Rating
        Text_display &popularity = this->make_child<Text_display>();
        Text_display &genreLabel = this->make_child<Text_display>(L"Genre:"); //Label for our button list
        personalButtonList &genres = this->make_child<personalButtonList>(); //Dynamic list of things to click on
        Text_display &padded2 = this->make_child<Text_display>(); //More padding
        Button& linker = this->make_child<Button>(L"Open this Movie on IMDB"); //This button will always lead
        Text_display& padded3 = this->make_child<Text_display>();
        Button& recommender = this->make_child<Button>(L"Click Here for Similar Movies"); //Our Greedy Algorithm

        MovieDisp() {
            named | Trait::Standout | pipe::fixed_height(1); //the name is a heading,
            padded | pipe::fixed_height(1);
            padded2 | pipe::fixed_height(1);
            padded3 | pipe::fixed_height(1);
            rated | pipe::fixed_height(1);
            popularity | pipe::fixed_height(1);
            genreLabel | pipe::fixed_height(1);
            linker | Trait::Standout | pipe::fixed_height(1); //These buttons should also standout
            recommender | Trait::Standout | pipe::fixed_height(1);
        };
    };

    Text_display &padding = this->make_child<Text_display>(); //Centering horizontally
    MovieDisp &content = this->make_child<MovieDisp>();
    Text_display &padding2 = this->make_child<Text_display>();


};
class App : public layout::Vertical<> { //The main attraction. Data is read in and out with this, and in general the forms are fairly consistent

    struct Search_bar : public layout::Horizontal<> { //For instance there's only one of these, though it could be used more
        Text_display& title = this->make_child<Text_display>(L"Search: "); //Heading
        Text_display& separator = this->make_child<Text_display>(L" ");
        Line_edit& selector = this->make_child<Line_edit>(L"Movie Title");
        sl::Signal<void(const std::string&)>& movied = selector.edit_finished; //Alias for signal

        Search_bar() {
            title | Trait::Standout | pipe::fixed_width(7);
            separator | pipe::fixed_width(1) ;
            selector | pipe::name("selector");
        }
};

    struct Number_bar : public layout::Horizontal<> { //For choosing how much the greedy algorithm will run
        Text_display& title = this->make_child<Text_display>(L"# of steps: "); //0 will be just the src vertex itself, others will be more.
        Text_display& separator = this->make_child<Text_display>(L" ");
        Number_edit<unsigned int>& recommender = this->make_child<Number_edit<unsigned int>>(); //unsigned int hopefully prevents - signs
        Number_bar() {
            title | Trait::Standout | pipe::fixed_width(12);
            separator | pipe::fixed_width(1);
        }
    };

    MovieGraph movieData; //Our data, read into some unordered maps
    string movieName; //JIC string that stores our

    Titlebar& title_ = this->make_child<Titlebar>(L"Battleship Cinema"); //This stays permanently on screen
    Text_display& instructions = this->make_child<Text_display>(L"Please input the Name of a Movie to Begin"); //This also stays permanently on screen
    Search_bar& searchBar = this->make_child<Search_bar>(); //For entering movie titles
    personalButtonList& queryResults = this->make_child<personalButtonList>(); //For the results of any algorithm/just genres
    MovieDispCentered& info = this->make_child<MovieDispCentered>(); //The permovie info
    Number_bar& recommender = this->make_child<Number_bar>(); //Deciding how long the greedy algorithm will run
    Text_display& padding2 = this->make_child<Text_display>(L" "); //Padding keeps thins centered
    sl::Signal<void(const std::string&)>& entered = searchBar.selector.edit_finished; //Alias for enter on movie input
public:
    auto key_press_event(Key k) -> bool override { //Simple code that allows escape to bring us home
        switch(k) {
            case Key::Escape:
                info.disable(); //We're not looking at a specific movie
                queryResults.disable(); //Or at query results,
                recommender.disable(); //Or deciding how long algorithms will run
                recommender | pipe::fixed_height(0);  //these
                info | pipe::fixed_height(0);  //shouldn't
                queryResults | pipe::fixed_height(0); //take up space on screen
                searchBar.enable(); //allow entering a movie
                searchBar | pipe::fixed_height(3);
                instructions.clear(); //get rid of current instruction text
                instructions.append("Please input the Name of a Movie to Begin"); //New instructions
                break;
            default: break; //any other key does nothing
        }
        return Widget::key_press_event(k);
    }
    App() { //main loop of our app

        info.disable(); //by default, we want to be inputting our movie title, not seeing blank data structures
        info | pipe::fixed_height(0);
        recommender.disable(); //at first, we're not deciding how long our algorithm will run
        recommender | pipe::fixed_height(0);
        queryResults.disable(); //We're not looking at any query results yes
        queryResults | pipe::fixed_height(0);
        this->focus_policy = Focus_policy::Direct;
        instructions | Trait::Bold; //Let instructions pop
        searchBar | pipe::bordered() | pipe::fixed_height(3); //the search bar should be visible
        recommender | pipe::bordered(); //Some flair
        info | pipe::bordered();
        movieData.GetInfo(); //Get our data!


        entered.connect([this](const std::string s){ //Code run once we enter some data
            const Movie* movieSelected = movieData.GetMovie(s); //Get what our movie is
            if(movieSelected) { //If it's a movie, not just some random jumble of letters
                searchBar.disable(); //Turn off our search
                searchBar | pipe::fixed_height(0);
                info.enable(); //Turn on movie specific info
                info | pipe::ignored_height();
                info.content.named.clear(); //remove any old data
                movieName = movieSelected->name; //get the new stuff
                Glyph_string named = movieSelected->name + " (" + std::to_string(movieSelected->year) + ")"; //Formatting
                info.content.named.append(named);
                info.content.rated.clear();
                Glyph_string ratingString = "Rated: " + std::to_string(movieSelected->rating) + "/5.0 stars.";
                info.content.rated.append(ratingString);
                info.content.popularity.clear();
                Glyph_string popularityString = "Viewed by " + std::to_string(movieSelected->popularity) + " people";
                info.content.popularity.append(popularityString);
                Glyph_string instructions_text = "Information about: " + s; //change instruction text too.
                instructions.clear();
                instructions.append(instructions_text);
                System::set_focus(*this); //Keep focus here for that escape function though
                int height = 0; //Perfect height
                info.content.genres.interior.delete_all_children(); //Remove old buttons
                for(string genre : movieSelected->genres) {
                    std::wstring genreString(genre.begin(), genre.end()); //wstring for the buttons
                    info.content.genres.add_button(genreString);
                    height++;
                }
                info.content.genres | pipe::fixed_height(height); //Perfect height
            }
        });


        info.content.genres.button_pressed.connect([this](const wstring& w) { //Clicking on a genre when viewing a movie
            string genreResult (w.begin(), w.end()); //what Genre is it?
            info.disable(); //turn off plain viewing
            info | pipe::fixed_height(0);
            queryResults.enable(); //turn on list viewing
            queryResults | pipe::ignored_height();
            queryResults.interior.delete_all_children(); //remove old list results
            for(Movie* movie : movieData.getGenreMovies(genreResult)) { //get what movies are in this genre.
                wstring movieName (movie->name.begin(),movie->name.end()); //Create all the buttons
                queryResults.add_button(movieName);
            }
            instructions.clear();
            Glyph_string instruct = genreResult + " Films ordered by Adjusted Rating (desc.)"; //New instructions
            instructions.append(instruct);
        });


        queryResults.button_pressed.connect([this](const wstring& w) { //Picking a new movie from the list
            std::string s (w.begin(), w.end());
            const Movie* movieSelected = movieData.GetMovie(s); //hash table ftw
            if(movieSelected) {
                queryResults.disable(); //Get rid of the list if someone chose one
                queryResults | pipe::fixed_height(0);
                info.enable(); //give us info
                info | pipe::ignored_height();
                info.content.named.clear();
                movieName = movieSelected->name;
                Glyph_string named = movieSelected->name + " (" + std::to_string(movieSelected->year) + ")"; //Set up the info
                info.content.named.append(named);
                info.content.rated.clear();
                Glyph_string ratingString = "Rated: " + std::to_string(movieSelected->rating) + "/5.0 stars.";
                info.content.rated.append(ratingString);
                info.content.popularity.clear();
                Glyph_string popularityString = "Viewed by " + std::to_string(movieSelected->popularity) + " people";
                info.content.popularity.append(popularityString);
                Glyph_string instructions_text = "Information about: " + s;
                instructions.clear();
                instructions.append(instructions_text);
                System::set_focus(*this); //Keep the focus on us, since then escape will  work
                int height = 0;
                info.content.genres.interior.delete_all_children(); //Janky button list
                for (string genre : movieSelected->genres) {
                    std::wstring genreString(genre.begin(), genre.end());
                    info.content.genres.add_button(genreString);
                    height++;
                }
                info.content.genres | pipe::fixed_height(height);
            }
        });


        info.content.recommender.pressed.connect([this]() { //Allows people to choose the amount of steps
            info.disable();
            info | pipe::fixed_height(0);
            recommender.enable();
            recommender | pipe::fixed_height(3);
            instructions.clear();
            instructions.append(L"Enter the number (n > -1) of steps to take through the Graph:");
        });


        recommender.recommender.edit_finished.connect([this](string s){ //actually runs recommendation algorithm
            vector<Movie*> recommended = movieData.recommendationAlgorithm(movieName, stoi(s)); //the meat
            recommender.disable(); //turn of number_edit
            recommender | pipe::fixed_height(0);
            queryResults.enable(); //show the list
            queryResults | pipe::ignored_height();
            queryResults.interior.delete_all_children();
            for(const Movie* movie : recommended ){
                wstring movieName (movie->name.begin(),movie->name.end());
                queryResults.add_button(movieName);
            }
            instructions.clear();
            Glyph_string instruct = "Recommended films for those who liked " + movieName +  " after " + s + " steps:";
            instructions.append(instruct);
            System::set_focus(*this);
        });


        info.content.linker.pressed.connect([this]() {
            string command = "xdg-open https:://www.imdb.com/title/tt" + movieData.GetMovie(movieName)->imdb; //Linux system call to open imdb
            system(command.c_str()); //xdg doesn't work on windows. We'd need preprocessor directives to go cross platform.
        });
    };
};
int main() {

    if(!std::filesystem::exists("../datasets/moviesWithRatings.csv")) { //If the files have been parsed, there is no need to parse them.
        filecreator(); //Pre-parser
    }
    return ox::System{}.run<App>(); //Start up the TUI


}