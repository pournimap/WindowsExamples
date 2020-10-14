#pragma once

#include <string>
#include "Obj_Loader.h"
#include "Read_Mtl.h"
#include "Arrange_Material.h"


GLuint gVbo_Model_Position, gVbo_Model_Normal, gVbo_Model_Texture, gVbo_Model_Tangents, gVbo_Model_BiTangents;

struct Model_Obj
{
	GLuint Vao;
	std::vector<float> model_vertices;
	std::vector<float> model_textures;
	std::vector<float> model_normals;
	
	std::vector<float> model_tangents;
	std::vector<float> model_bitangents;

	std::vector<material> model_material;
	std::vector<Mesh_Data> model_mesh_data;
	char model_materialFileName[256];
};

Model_Obj gModel_Nanosuit;



void LoadAllModels()
{

	/*****************Load Mahal Model*****************/
	std::string mahalModelFilePath = "nanosuit/nanosuit.obj";

	LoadMeshData(mahalModelFilePath.c_str(), gModel_Nanosuit.model_vertices, gModel_Nanosuit.model_textures, gModel_Nanosuit.model_normals,
		gModel_Nanosuit.model_tangents, gModel_Nanosuit.model_bitangents, gModel_Nanosuit.model_mesh_data, gModel_Nanosuit.model_materialFileName);

	LoadMaterialData(gModel_Nanosuit.model_materialFileName, gModel_Nanosuit.model_material);

	Rearrange_Material_Data(gModel_Nanosuit.model_mesh_data, gModel_Nanosuit.model_material);


	/*****************VAO For Model*****************/
	glGenVertexArrays(1, &gModel_Nanosuit.Vao);
	glBindVertexArray(gModel_Nanosuit.Vao);

	/*****************Model Position****************/
	glGenBuffers(1, &gVbo_Model_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Model_Position);
	glBufferData(GL_ARRAY_BUFFER, gModel_Nanosuit.model_vertices.size() * sizeof(float), &gModel_Nanosuit.model_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*****************Model Color****************/
	glGenBuffers(1, &gVbo_Model_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Model_Normal);
	glBufferData(GL_ARRAY_BUFFER, gModel_Nanosuit.model_normals.size() * sizeof(float), &gModel_Nanosuit.model_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*******************Texture******************/
	glGenBuffers(1, &gVbo_Model_Texture);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Model_Texture);
	glBufferData(GL_ARRAY_BUFFER, gModel_Nanosuit.model_textures.size() * sizeof(float), &gModel_Nanosuit.model_textures[0], GL_STATIC_DRAW);
	glVertexAttribPointer(MALATI_ATTRIBUTE_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(MALATI_ATTRIBUTE_TEXTURE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


}


void uninitializeAllModelData()
{
	/*****************Candle Model*****************/
	for (int i = 0; i < gModel_Nanosuit.model_mesh_data.size(); i++)
	{
		if (gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].gTexture_diffuse)
		{
			glDeleteTextures(1, &gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].gTexture_diffuse);
			gModel_Nanosuit.model_material[gModel_Nanosuit.model_mesh_data[i].material_index].gTexture_diffuse = 0;
		}
	}

	if (gModel_Nanosuit.Vao)
	{
		glDeleteVertexArrays(1, &gModel_Nanosuit.Vao);
		gModel_Nanosuit.Vao = 0;
	}

	if (gVbo_Model_Position)
	{
		glDeleteBuffers(1, &gVbo_Model_Position);
		gVbo_Model_Position = 0;
	}
	if (gVbo_Model_Normal)
	{
		glDeleteBuffers(1, &gVbo_Model_Normal);
		gVbo_Model_Normal = 0;
	}
	if (gVbo_Model_Texture)
	{
		glDeleteBuffers(1, &gVbo_Model_Texture);
		gVbo_Model_Texture = 0;
	}
	if (gVbo_Model_Tangents)
	{
		glDeleteBuffers(1, &gVbo_Model_Tangents);
		gVbo_Model_Tangents = 0;
	}
	if (gVbo_Model_BiTangents)
	{
		glDeleteBuffers(1, &gVbo_Model_BiTangents);
		gVbo_Model_BiTangents = 0;
	}
}
