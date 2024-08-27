#include <iostream>
#include <thread>
#include "PokemonData.h"
#include "GuiThread.h"
#include "FetchFeedThread.h"
#include "FavoritesUtils.h"

//Main function (Program begins here)
int main()
{
    CommonPokemonObjects common;
    GuiThread draw;

    //Starts new thread and calls the Draw routine in it
    auto draw_th = std::jthread([&] {draw(common); });

    FetchFeedThread down;

    //URL to fetch the Pokemon feed for the homepage
    down.SetUrl("/api/v2/pokemon?limit=1025");

    //Starts new thread and calls the Download (fetch feed) routine in it
    auto down_th = std::jthread([&] {down(common); });

    std::cout << "running...\n";
    
    down_th.join();

    if (!common.favoritesLoaded && common.pokemonData.size() > 0)
        LoadUserFavorites(common, "UserFavoritesDatabase.txt");

    draw_th.join(); //program ends when main draw (GUI) thread stops
}
