#include "FetchImageThread.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "nlohmann/json.hpp"

#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"

//Defines object structure for the JSON parser to populate
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PokemonData, name)

//Entry point for the thread when creating the class FetchImageThread
void FetchImageThread::operator()(CommonPokemonObjects& common, bool back)
{
	//Sets the domain hostname of the API
	httplib::Client cli("https://raw.githubusercontent.com");

	//Creates an HTTP GET request to the API
	auto res = cli.Get(_download_url);

	if (res->status == 200) //if response is OK
	{
		//Initializes texture variable (to map the images into textures)
		ID3D11ShaderResourceView* my_texture = NULL;

		//Loads fetched Pokemon thread (in memory) into a texture in the program
		bool ret = LoadTextureFromMemory(res->body.c_str(), res->body.size(), &my_texture, &common.current_pokemon->my_image_width, &common.current_pokemon->my_image_height);

		//Checks if back of the Pokemon images requested
		if (back)
		{
			//Sets the texture data for the back image of the Pokemon
			common.current_pokemon->my_texture_back = my_texture;

			//Creates the URL for the shiny version of the images
			std::string back_shiny_url = _download_url.substr(0, 78) + "shiny/" + _download_url.substr(78, _download_url.length());

			//Second HTTP GET call to fetch the shiny version of the back image
			res = cli.Get(back_shiny_url);

			if (res->status == 200) // if response is OK
			{
				//Loads second texture & sends data to Pokemon
				ret = LoadTextureFromMemory(res->body.c_str(), res->body.size(), &my_texture, &common.current_pokemon->my_image_width, &common.current_pokemon->my_image_height);
				common.current_pokemon->my_texture_shiny_back = my_texture;
			}

			common.current_pokemon->fetchedImageBack = true;
		}
		else //Else, front image is the _download_url
		{
			common.current_pokemon->my_texture = my_texture;
			common.current_pokemon->fetchedImage = true;
		}
	}
}

void FetchImageThread::SetUrl(std::string_view new_url)
{
	_download_url = new_url;
}
