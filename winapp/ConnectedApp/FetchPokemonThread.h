#pragma once
#include "PokemonData.h"

class FetchPokemonThread
{

public:
	void operator()(CommonPokemonObjects& common);
	void SetUrl(std::string_view new_url);

private:
	std::string _download_url;

};

