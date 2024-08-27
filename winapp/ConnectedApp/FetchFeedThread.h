#pragma once
#include "PokemonData.h"

class FetchFeedThread
{

public:
	void operator()(CommonPokemonObjects& common);
	void SetUrl(std::string_view new_url);

private:
	std::string _download_url;

};

