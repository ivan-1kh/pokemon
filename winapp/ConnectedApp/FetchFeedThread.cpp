#include "FetchFeedThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

#include <GuiMain.h>
#include "../../shared/ImGuiSrc/imgui.h"

//Defines object structure for the JSON parser to populate
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PokemonData, name, url)

//Helper function to load Pokemon sprites into RAM
void LoadImages(CommonPokemonObjects& common)
{
	//DirectX 11 texture data structure for reading images into
	ID3D11ShaderResourceView* my_texture = NULL;

	bool ret; //texture load success flag

	//Loads the favorite symbols (full heart, empty heart icons)
	ret = LoadTextureFromFile("sprites/heart_empty.png", &my_texture, &common.heartImageWidth, &common.heartImageHeight);
	common.heartEmptyTexture = my_texture;
	ret = LoadTextureFromFile("sprites/heart_full.png", &my_texture, &common.heartImageWidth, &common.heartImageHeight);
	common.heartFullTexture = my_texture;

	//temporary helper variables
	std::string temp;
	std::string original_string;

	int pokemon_number;
	int characters_to_remove = 34;

	//Iterates over every pokemon in memory to fetch corresponding sprite
	for (auto& rec : common.pokemonData)
	{
		//Extracts pokemon unique ID to fetch associated sprite PNG
		original_string = rec.url.substr(characters_to_remove);
		original_string = original_string.substr(0, original_string.length() - 1);

		pokemon_number = std::stoi(original_string);

		rec.pokemonID = pokemon_number; //unique identifier

		//Loads texture of the default Pokemon image
		temp = "sprites/" + std::to_string(pokemon_number) + ".png";
		ret = LoadTextureFromFile(temp.c_str(), &my_texture, &rec.my_image_width, &rec.my_image_height);
		rec.my_texture = my_texture;

		//Loads texture of the shiny Pokemon image (for the hover feature)
		temp = "sprites_shiny/" + std::to_string(pokemon_number) + ".png";
		ret = LoadTextureFromFile(temp.c_str(), &my_texture, &rec.my_image_width, &rec.my_image_height);
		rec.my_texture_hover = my_texture;

		//check if success
		if (ret) rec.fetchedImage = true;
	}
}

//Entry point for the thread when creating the class FetchPokemonThread
void FetchFeedThread::operator()(CommonPokemonObjects& common)
{
	//Sets the domain hostname of the API
	httplib::Client cli("https://pokeapi.co");

	//Creates an HTTP GET request to the API
	auto res = cli.Get(_download_url);

	if (res->status == 200) //if response is OK
	{
		//Uses the library 'json' to parse the response contents into C++ readable object
		auto json_result = nlohmann::json::parse(res->body);
		common.pokemonData = json_result["results"];

		//Loads the images locally (./sprites and ./sprites_shiny)
		LoadImages(common);

		if (common.pokemonData.size() > 0)
			common.data_ready = true;
	}
}

void FetchFeedThread::SetUrl(std::string_view new_url)
{
	_download_url = new_url;
}
