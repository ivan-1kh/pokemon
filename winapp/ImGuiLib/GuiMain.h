#pragma once
#include <d3d11.h>

using drawcallback = void(void*);

bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

int GuiMain(drawcallback drawfunction, void* obj_ptr);
