#include "FetchPokemonThread.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

//Defines object structure for the JSON parser to populate
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PokemonData, name, img, img_back, weight)

//Entry point for the thread when creating the class FetchPokemonThread
void FetchPokemonThread::operator()(CommonPokemonObjects& common)
{
	//Sets the domain hostname of the API
	httplib::Client cli("https://pokeapi.co");

	//Creates an HTTP GET request to the API
	auto res = cli.Get(_download_url);

	if (res->status == 200) //if response is OK
	{
		//Uses the library 'json' to parse the response contents into C++ readable object
		auto json_result = nlohmann::json::parse(res->body);

		//Initialize data vectors
		common.current_pokemon->abilities = std::vector<AbilityData>(json_result["abilities"].size());
		common.current_pokemon->items = std::vector<ItemData>(json_result["held_items"].size());
		common.current_pokemon->moves = std::vector<MoveData>(json_result["moves"].size());

		//Populates weight data
		common.current_pokemon->weight = json_result["weight"];

		//Populates ability data
		for (int i = 0; i < json_result["abilities"].size(); i++)
		{
			common.current_pokemon->abilities[i].name = json_result["abilities"][i]["ability"]["name"];
			common.current_pokemon->abilities[i].url = json_result["abilities"][i]["ability"]["url"];
		}

		//Populates held item data
		for (int i = 0; i < json_result["held_items"].size(); i++)
		{
			common.current_pokemon->items[i].name = json_result["held_items"][i]["item"]["name"];
			common.current_pokemon->items[i].url = json_result["held_items"][i]["item"]["url"];
		}

		//Populates move data
		for (int i = 0; i < json_result["moves"].size(); i++)
		{
			common.current_pokemon->moves[i].name = json_result["moves"][i]["move"]["name"];
			common.current_pokemon->moves[i].url = json_result["moves"][i]["move"]["url"];
		}

		//Passes the Pokemon's raw image URLs (for fetching later)
		common.current_pokemon->img = json_result["sprites"]["front_default"];
		common.current_pokemon->img_back = json_result["sprites"]["back_default"];

		common.current_pokemon->fetchedDetails = true; //Pokemon details populated flag
	}
}

void FetchPokemonThread::SetUrl(std::string_view new_url)
{
	_download_url = new_url;
}
