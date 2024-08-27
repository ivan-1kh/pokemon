#include "GuiThread.h"
#include "APIUtils.h"
#include "GuiUtils.h"
#include "FavoritesUtils.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include "FetchPokemonThread.h"
#include "FavoritePokemonMetadata.h"
#include "FetchImageThread.h"

//Function signatures
void RenderPokemonDetails(CommonPokemonObjects* common, PokemonData& rec, ImVec4 color);
void RenderHeartButton(PokemonData& rec);
void RenderPokemonImages(PokemonData& rec);

//Global variables used in GUI Thread routine
CommonPokemonObjects* common = NULL;

bool show_pokemon_not_found = false;
ImVec4 color;
ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red color
ImVec2 display_size;

int i;

static char buff[200];
PokemonData* queriedPokemon;

//Pokemon range to display (per page)
int startOfRange = 1;
int endOfRange = 400;

//Helper function to update Pokemon ID range depending on new page
void navigatePage()
{
	startOfRange = 1 + (common->pageNumber - 1) * 400;
	endOfRange = common->pageNumber * 400;
}

//Main window render function
void DrawAppWindow(void* common_ptr)
{
	//Populate global common variable
	if (common == NULL) common = (CommonPokemonObjects*)common_ptr;

	//Get display size
	ImGuiIO& io = ImGui::GetIO();
	display_size = io.DisplaySize;

	//Set the next window position to the top-left corner
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	//Set the next window size to fill the display
	ImGui::SetNextWindowSize(display_size);

	//Begin the window with full screen flags
	ImGui::Begin("Pokemon Pool", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("");
	//Set the width for the next item (search bar)
	ImGui::SetNextItemWidth(300.0f);  //Adjust the width to your preference

	//Push custom styles for a brighter appearance and larger height
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));  // Light background color
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));      // Dark text color
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));     // Increase the padding to enlarge the height

	//Create the search bar
	ImGui::InputTextWithHint("##search", "Search for Pokemon", buff, sizeof(buff));

	//Pop the custom styles to revert to the default
	ImGui::PopStyleVar(); //Revert the frame padding style change
	ImGui::PopStyleColor(2); // Revert color style changes

	ImGui::SameLine();

	//Search button
	if (ImGui::Button("I'm Feeling Lucky"))
	{
		queriedPokemon = FetchPokemonByQuery(*common, buff);
		
		if (queriedPokemon) //Pokemon found
		{
			queriedPokemon->showDetails = true;
			show_pokemon_not_found = false;
		}
		else //Search query returned no hits
		{
			show_pokemon_not_found = true;
		}
	}

	//Displays error message
	if (show_pokemon_not_found)
	{
		ImGui::SameLine();
		ImGui::TextColored(red, "Pokemon not found!");
	}

	ImGui::Text("");

	//Page indicator
	ImGui::Text(("PAGE <" + std::to_string(common->pageNumber) + ">").c_str());

	//Navigate to left page function
	if (ImGui::Button(" < "))
	{
		common->pageNumber = (common->pageNumber == 1) ? 3 : (common->pageNumber - 1);
		navigatePage();
	}

	ImGui::SameLine();

	//Navigate to right page function
	if (ImGui::Button(" > "))
	{
		common->pageNumber = (common->pageNumber == 3) ? 1 : (common->pageNumber + 1);
		navigatePage();
	}

	ImGui::Text("");

	//Favorite Pokemon page filter
	if (common->favoritesFilter)
	{
		if (ImGui::Button("Return to Feed"))
		{
			common->favoritesFilter = false;
		}
	}
	else
	{
		if (ImGui::Button("Show Favorites Only"))
		{
			common->favoritesFilter = true;
		}
	}

	ImGui::Text("");

	//Populates main feed when data has been set
	//Data ready MUTEX flag (displays only when feed fetched from API)
	if (common->data_ready)
	{
		//Displays Pokemon feed in a grid-like view
		if (ImGui::BeginTable("Pokemon", 5))
		{
			ImGui::TableSetupColumn("");
			ImGui::TableSetupColumn("");
			ImGui::TableSetupColumn("");
			ImGui::TableSetupColumn("");
			ImGui::TableHeadersRow();

			color = RandomDarkColor(); //generates color for cosmetic purposes

			i = 0;

			//Displays the feed (iterates over data received from API)
			for (auto& rec : common->pokemonData)
			{
				//Opens Pokemon's details window if flag is set
				if (rec.showDetails)
				{
					RenderPokemonDetails(common, rec, color);
				}

				//If favorites-only filter is toggled on, skips over non-fav entries
				if ((!rec.favMetadata.isFav) && common->favoritesFilter)
				{
					continue;
				}

				//Checks for page range & displays Pokemon accordingly
				//First 'if' makes sure it correctly displays favorites
				//regardless of current navigated page.
				if (rec.favMetadata.isFav && common->favoritesFilter)
				{
					//do nothing
				}
				else if (rec.pokemonID < startOfRange || rec.pokemonID > endOfRange)
				{
					continue;
				}

				ImGui::PushID(rec.name.c_str()); //Create a unique ID scope

				ImGui::TableNextColumn();
				ImGui::TableSetColumnIndex(i);

				//Renders favorite button
				RenderHeartButton(rec);

				ImGui::SameLine();

				ImGui::Text(rec.name.c_str()); //Pokemon name label

				//Displays Pokemon image in the homepage feed grid-view
				if (rec.fetchedImage)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Set background color to transparent
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Set hover color to transparent
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // Set hover color to transparent

					if (rec.hover)
					{
						//Creates button that opens the Pokemon's details (with shiny hover effect)
						if (ImGui::ImageButton((void*)rec.my_texture_hover, ImVec2(1.0f * rec.my_image_width + 70, 1.0f * rec.my_image_height + 70), ImVec2(0, 0), ImVec2(1, 1), 1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
						{
							rec.showDetails = true;
						}
					}
					else
					{
						if (rec.favMetadata.isFav) //if favorite, image flashes colors
						{
							ImGui::PopStyleColor(3);
							ImGui::PushStyleColor(ImGuiCol_Button, color);
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
						}

						//Creates button that opens the Pokemon's details
						if (ImGui::ImageButton((void*)rec.my_texture, ImVec2(1.0f * rec.my_image_width + 70, 1.0f * rec.my_image_height + 70), ImVec2(0, 0), ImVec2(1, 1), 1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
						{
							rec.showDetails = true;
						}
					}

					// Hover image
					if (ImGui::IsItemHovered())
					{
						rec.hover = true;
					}
					else
					{
						rec.hover = false;
					}

					ImGui::PopStyleColor(3);
				}
				else
				{
					if (ImGui::Button(rec.name.c_str()))
					{
						rec.showDetails = true;
					}
				}

				i++;

				if (i == 5)
					i = 0;

				ImGui::PopID(); // End the unique ID scope
			}

			ImGui::EndTable();
		}
	}

	ImGui::End();
}

//Modular function that renders the details window of the Pokemon.
void RenderPokemonDetails(CommonPokemonObjects* common, PokemonData& rec, ImVec4 color)
{
	//Fetches Pokemon details from API
	FetchPokemon(*common, &rec);

	//Create the details window with a title and control its visibility flag
	ImGui::Begin(rec.name.c_str(), &rec.showDetails);
	ImGui::SetWindowSize(ImVec2(650, 500)); // Set size only on first use

	ImGui::Text("");

	//Displays favorite heart button
	RenderHeartButton(rec);

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

	ImGui::Button(rec.name.c_str(), ImVec2(120, 40));

	//Renders the 4 Pokemon images in the Pokemon details window
	RenderPokemonImages(rec);

	//displays data here

	ImGui::Text("");
	ImGui::Text("Weight:");
	ImGui::SameLine();
	//Displays Pokemon weight information
	ImGui::Text(std::to_string(rec.weight).c_str());

	ImGui::Text("");
	ImGui::Text("Abilities:");

	//Displays Pokemon abilities information
	for (int k = 0; k < rec.abilities.size(); k++)
	{
		ImGui::SameLine();
		ImGui::Text(" ");
		if (k % 4 != 0) ImGui::SameLine();

		ImGui::Button(rec.abilities[k].name.c_str(), ImVec2(120, 40));
	}

	ImGui::Text("");
	ImGui::Text("Moves:");

	//Displays Pokemon moves information
	for (int k = 0; k < rec.moves.size(); k++)
	{
		ImGui::SameLine();
		ImGui::Text(" ");
		if (k % 4 != 0) ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f)); // Adjust padding as needed

		ImGui::Button(rec.moves[k].name.c_str());
		ImGui::PopStyleVar();
	}

	ImGui::Text("");
	ImGui::Text("Held Items:");

	//Displays Pokemon held items information
	for (int k = 0; k < rec.items.size(); k++)
	{
		ImGui::SameLine();
		ImGui::Text(" ");
		if (k % 4 != 0) ImGui::SameLine();

		ImGui::Button(rec.items[k].name.c_str(), ImVec2(120, 40));
	}

	ImGui::PopStyleColor(3);

	// End the details window
	ImGui::End();
}

//Renders the heart icon button (for AddFavorite and RemoveFavorite)
void RenderHeartButton(PokemonData& rec)
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); // Set background color to transparent
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Set hover color to transparent
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0)); // Set hover color to transparent

	//Displays heart button to favorite or unfavorite Pokemon
	if (rec.favMetadata.isFav)
	{
		//In case Pokemon already in favorites, displays full heart button
		if (ImGui::ImageButton((void*)common->heartFullTexture, ImVec2(40, 35), ImVec2(0, 0), ImVec2(1, 1), 1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
		{
			RemoveFavoritePokemon(*common, "UserFavoritesDatabase.txt", &rec);
		}
	}
	else
	{
		//In case Pokemon already in favorites, displays empty heart button
		if (ImGui::ImageButton((void*)common->heartEmptyTexture, ImVec2(40, 35), ImVec2(0, 0), ImVec2(1, 1), 1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
		{
			AddFavoritePokemon(*common, "UserFavoritesDatabase.txt", &rec);
		}
	}

	ImGui::PopStyleColor(3);
}

//Renders the Pokemon's sprites in the details window
void RenderPokemonImages(PokemonData& rec)
{
	//Fetches Pokemon's sprites from the API
	FetchPokemonImage(*common, &rec);

	if (rec.fetchedImage)
	{
		ImGui::Image((void*)rec.my_texture, ImVec2(1.0f * rec.my_image_width + 70, 1.0f * rec.my_image_height + 70));
	}

	//Fetches the Pokemon's back sprite from the API
	FetchPokemonImageBack(*common, &rec);

	if (rec.fetchedImageBack)
	{
		ImGui::SameLine();
		ImGui::Image((void*)rec.my_texture_back, ImVec2(1.0f * rec.my_image_width + 70, 1.0f * rec.my_image_height + 70));
	}

	if (rec.fetchedImage)
	{
		ImGui::Image((void*)rec.my_texture_hover, ImVec2(1.0f * rec.my_image_width + 70, 1.0f * rec.my_image_height + 70));
	}

	if (rec.fetchedImageBack)
	{
		ImGui::SameLine();
		ImGui::Image((void*)rec.my_texture_shiny_back, ImVec2(1.0f * rec.my_image_width + 70, 1.0f * rec.my_image_height + 70));
	}
}

void GuiThread::operator()(CommonPokemonObjects& common)
{
	//GuiMain([] { DrawAppWindow(); });
	GuiMain(DrawAppWindow, &common);
	common.exit_flag = true;
}
