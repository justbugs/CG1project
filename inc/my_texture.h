#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//Do not use image bigger than 30kB. Spuare image is better
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here you need to fill the loadTexture function, so that you can use it to load texture in main.             //
// width and height must be pow of 2                                                                           //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data){

		GLenum form;
		if (nrComponents == 1)
			form = GL_RED;
		else if (nrComponents == 3)
			form = GL_RGB;
		else if (nrComponents == 4)
			form = GL_RGBA;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D,textureID);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,0,form,width,height,0,form,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else
	{
    	std::cout << "Failed to load texture" << std::endl;
	}
	return textureID;
}