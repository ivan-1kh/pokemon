#include "APIUtils.h"
#include "PokemonData.h"
#include <iostream>
#include "FetchPokemonThread.h"
#include <thread>
#include "FetchImageThread.h"

//Creates new thread to fetch pokemon details from the API
void FetchPokemon(CommonPokemonObjects& common, PokemonData* pokemonData)
{
	//Redundancy check
	if (pokemonData->fetchedDetails == true)
		return;

	std::cout << "Fetching Pokemon " + pokemonData->name + "'s data" << std::endl;

	FetchPokemonThread pokemonDetailsDown;

	//Sets data for thread to handle
	common.current_pokemon = pokemonData;
	pokemonDetailsDown.SetUrl("/api/v2/pokemon/" + pokemonData->name);

	auto pokemonDetailsDown_th = std::jthread([&] {pokemonDetailsDown(common); });
}

//Creates new thread to fetch pokemon front image from the API
void FetchPokemonImage(CommonPokemonObjects& common, PokemonData* pokemonData)
{
	//Redundancy check
	if (pokemonData->fetchedImage)
		return;

	std::cout << "Fetching Image of " << pokemonData->name << std::endl;

	FetchImageThread pokemonImageThread;

	//Sets data for thread to handle
	common.current_pokemon = pokemonData;
	pokemonImageThread.SetUrl(pokemonData->img);

	auto pokemonImageThread_th = std::jthread([&] {pokemonImageThread(common, false); });
}

//Creates new thread to fetch pokemon back image from the API
void FetchPokemonImageBack(CommonPokemonObjects& common, PokemonData* pokemonData)
{
	if (pokemonData->fetchedImageBack)
		return;

	std::cout << "Fetching Image of " << pokemonData->name << std::endl;

	FetchImageThread pokemonImageThread;

	//Sets data for thread to handle
	common.current_pokemon = pokemonData;
	pokemonImageThread.SetUrl(pokemonData->img_back); //img_back = indicating to API back image

	auto pokemonImageThread_th = std::jthread([&] {pokemonImageThread(common, true); });
}

//Helper function to pass pokemon from search query and call to fetch its details
PokemonData* FetchPokemonByQuery(CommonPokemonObjects& common, std::string buffer)
{
	PokemonData* pokemonData;

	//iterates over Pokemon to find hit from search query
	for (auto& rec : common.pokemonData)
	{
		if (_strcmpi(rec.name.c_str(), buffer.c_str()) == 0)
		{
			//Pokemon found
			pokemonData = &rec;

			if (!pokemonData->fetchedDetails)
				FetchPokemon(common, pokemonData); //Makes the API call
			
			return pokemonData;
		}
	}

	return NULL;
}
