
#include <windef.h>
#include <glad/glad.h>  
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <iostream>
#include "../3rdLibs/glm/glm/glm.hpp"
#include "../3rdLibs/glm/glm/gtc/matrix_transform.hpp"
#include "../3rdLibs/glm/glm/gtc/type_ptr.hpp"


#include "../inc/my_texture.h"
#include "../inc/shader_m.h"
#include "tiny_obj_loader.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The .obj .mtl and images are in Dir "model".                                                                  //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*-----------------------------------------------------------------------*/
//Here are some mouse and keyboard function. You can change that.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float degreeX = (360 * lastX / 400);
float degreeY = (360 * lastY / 300);
bool firstMouse = true;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float OX = 0;//should be update to a new coordinate
float OY = 0;
float OZ = 0;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
float currentFrame;
float yaw = -90.0;
float pitch = 0;


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	glm::vec3 front;//why not in global 
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
	//std::cout << yaw << " " << pitch << std::endl;
}

void processInput(GLFWwindow* window)
{
	/*currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;*/
	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void initPMV()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, SCR_WIDTH / SCR_HEIGHT, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt
	(
		3, 3, 3,
		0, 0, 0,
		0, 1, 0
	);

}

void changePMV()
{


}
/*-----------------------------------------------------------------------*/


void cal_tang(std::vector<glm::vec3> pos,std::vector<glm::vec2> uv,std::vector<glm::vec3> &tangant,std::vector<glm::vec3> &bitangant){
	glm::vec3 e1,e2,tan,bitan;
	glm::vec2 duv1,duv2;
	
	for (int i=0;i<pos.size();i+=3){
		e1 = pos[i+1]-pos[i];
		e2 = pos[i+2]-pos[i];
		duv1 = uv[i+1]-uv[i];
		duv2 = uv[i+2]-uv[i];
		if ((duv1.x*duv2.y-duv2.x*duv1.y)==0){
			tangant.push_back(glm::normalize(e1));
			bitangant.push_back(glm::cross(glm::normalize(e1),glm::cross(e1,e2)));
		}
		else {
			float f = 1.0/(duv1.x*duv2.y-duv2.x*duv1.y) ;
			tan = f*glm::vec3(duv2.y*e1.x-duv1.y*e2.x,duv2.y*e1.y-duv1.y*e2.y,duv2.y*e1.z-duv1.y*e2.z);
			tan = glm::normalize(tan);
			tangant.push_back(tan);

			bitan = f*glm::vec3(-duv2.x*e1.x+duv1.x*e2.x,-duv2.x*e1.y+duv1.x*e2.y,-duv2.x*e1.z+duv1.x*e2.z);
			bitan = glm::normalize(bitan);
			bitangant.push_back(bitan);
		}

	}
	return;
}
//This an function to get v, vt and vn. 
bool make_face(std::vector<float> v, std::vector<float> vt, std::vector<float> vn, std::vector<unsigned int> f,
	std::vector<glm::vec3>& points, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs)
{
	if (f.size() % 3 != 0)
		return false;
	for (int i = 0; i < f.size()/3; i += 1)
	{
		int k = i * 3;
		for (int j = 0; j < 3; j++)
		{
			points.push_back(glm::vec3(v[f[k + j] * 3], v[f[k + j] * 3 + 1], v[f[k + j] * 3 + 2]));
			normals.push_back(glm::vec3(vn[f[k + j] * 3], vn[f[k + j] * 3 + 1], vn[f[k + j] * 3 + 2]));
			uvs.push_back(glm::vec2(vt[f[k + j] * 2], vt[f[k + j] * 2 + 1]));
		}		
	}
}
void get_vec3(std::vector<float> list, std::vector<glm::vec3> &vec)
{
	int n = list.size() / 3;
	for (int i = 0; i < n; i++)
	{
		vec.push_back(glm::vec3(list[i], list[i + 1], list[i + 2]));
	}
}
void get_vec2(std::vector<float> list, std::vector<glm::vec2>& vec)
{
	int n = list.size() / 2;
	for (int i = 0; i < n; i++)
	{
		vec.push_back(glm::vec2(list[i], list[i + 1]));
	}
}



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	
    gladLoadGL();  
    
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Here you need to fill construct function of class Shader. And you need to understand other funtions in Shader.//
	// Then, write code in shader_m.vs, shader_m.fs and shader_m.gs to finish the tasks.                             //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Shader my_shader(
		"../src/shader_m.vs", 
		"../src/shader_m.fs"
	);
	//A shader for light visiable source
	Shader lampShader("../src/lamp.vs", "../src/lamp.fs");
	


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// tiny::LoadObj is a function to load obj file. The output is shape_t and material_t.                         //
	// "shape.mesh" is a mesh struct. "mesh.positions", "mesh.normals", "mesh.texcoords" corresponds to v,vn,vt.   //
	// For example:                                                                                                //
	// positions[0],positions[1],positions[2] -> v 0,0,1                                                           //
	// positions[3],positions[4],positions[5] -> v 0,1,0                                                           //
	// "mesh.indice" corresponds to f, but it is different from f. Each element is an index for all of v,vn,vt.    //
	// positions[0],positions[1],positions[2] -> v 0,0,1  positions[0],positions[1],positions[2] -> v 0,0,1        //
	// You can read tiny_obj_loader.h to get more specific information.                                            //
	//                                                                                                             //
	// I have write make_face for you.  It will return v, vt, vn in vec form (each element if for one point).      //
	// These informations can help you to do normal mapping.  (You can calculate tangent here)                     //
	// Since i did not assign uv for noraml map, you just need use vt as uv for normal map, but you will find it is//
	//  ugly. So please render a box to show a nice normal mapping. (Do normal mapping on obj and box)             //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string if_load_succeed = tinyobj::LoadObj(shapes, materials,
		"../src/p.obj"
	);
	std::vector<unsigned int> obj_VBO_l, obj_VAO_l;
	for (int i = 0; i < shapes.size(); i++)
	{
		
		std::vector < glm::vec3 > out_vertices;
		std::vector < glm::vec2 > out_uvs;
		std::vector < glm::vec3 > out_normals;

		// out_vertices, out_uvs, out_normals will get v, vt and vn.
		make_face(shapes[i].mesh.positions, shapes[i].mesh.texcoords, shapes[i].mesh.normals, shapes[i].mesh.indices,
			out_vertices, out_normals, out_uvs);
		std::vector <glm::vec3> out_tangant,out_bitangant;
		cal_tang(out_vertices,out_uvs,out_tangant,out_bitangant);
		int scale = shapes[i].mesh.indices.size();
		float pikachu[scale*14];
		for (int id =0;id<scale;id++){
			int k = id*14;
			int j = id/3;
			pikachu[k]=out_vertices[id].x;
			pikachu[k+1]=out_vertices[id].y;
			pikachu[k+2]=out_vertices[id].z;
			pikachu[k+3]=out_normals[id].x;
			pikachu[k+4]=out_normals[id].y;
			pikachu[k+5]=out_normals[id].z;
			pikachu[k+6]=out_uvs[id].x;
			pikachu[k+7]=out_uvs[id].y;
			pikachu[k+8]=out_tangant[j].x;
			pikachu[k+9]=out_tangant[j].y;
			pikachu[k+10]=out_tangant[j].z;
			pikachu[k+11]=out_bitangant[j].x;
			pikachu[k+12]=out_bitangant[j].y;
			pikachu[k+13]=out_bitangant[j].z;
		}
		unsigned int A,B;
		glGenBuffers(1, &B);
		glBindBuffer(GL_ARRAY_BUFFER, B);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pikachu), pikachu, GL_STATIC_DRAW);
		glGenVertexArrays(1, &A);
		glBindVertexArray(A);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3*sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6*sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8*sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11*sizeof(float)));
		glEnableVertexAttribArray(4);
		glBindVertexArray(0);
		obj_VAO_l.push_back(A);
		obj_VBO_l.push_back(B);
		
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render a box to show nice normal mapping.                                                                   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float vertices_cube_0[] = {

		// positions          // normals           // texture coords

		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

	};
	std::vector<glm::vec3> pos,norm,tangant,bitangant;
	std::vector<glm::vec2> uv;

	for (int i = 0;i<36;i++){
		int k =i*8;
		pos.push_back(glm::vec3(vertices_cube_0[k],vertices_cube_0[k+1],vertices_cube_0[k+2]));
		norm.push_back(glm::vec3(vertices_cube_0[k+3],vertices_cube_0[k+4],vertices_cube_0[k+5]));
		uv.push_back(glm::vec2(vertices_cube_0[k+6],vertices_cube_0[k+7]));
	}
	cal_tang(pos,uv,tangant,bitangant);
	float cube_vertex[504];
	for (int i=0;i<36;i++){
		int k = i*14;
		int j = i/3;
		cube_vertex[k]=pos[i].x;
		cube_vertex[k+1]=pos[i].y;
		cube_vertex[k+2]=pos[i].z;
		cube_vertex[k+3]=norm[i].x;
		cube_vertex[k+4]=norm[i].y;
		cube_vertex[k+5]=norm[i].z;
		cube_vertex[k+6]=uv[i].x;
		cube_vertex[k+7]=uv[i].y;
		cube_vertex[k+8]=tangant[j].x;
		cube_vertex[k+9]=tangant[j].y;
		cube_vertex[k+10]=tangant[j].z;
		cube_vertex[k+11]=bitangant[j].x;
		cube_vertex[k+12]=bitangant[j].y;
		cube_vertex[k+13]=bitangant[j].z;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// You need to fill this function which is defined in my_texture.h. The parameter is the path of your image.   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	unsigned int texture1 = loadTexture("../src/container.jpg");
	unsigned int texture2 = loadTexture("../src/normal_map.jpg");
	unsigned int texture3 = loadTexture("../src/p_r.jpg");
	




	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Here we defined pointlights in shader and passed some parameter for you. You can take this as an example.   //
	// Or you can change it if you like.                                                                           //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glm::vec3 pointLightPositions[] = {
		glm::vec3(5.7f,  5.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	my_shader.use();
	my_shader.setVec3("dirLight.direction", glm::vec3(1.01f, 1.01f, 1.01f));
	my_shader.setVec3("dirLight.ambient", glm::vec3(0.01f, 0.01f, 0.02f));
	my_shader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	my_shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	// point light 1
	my_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	my_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	my_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[0].constant", 1.0f);
	my_shader.setFloat("pointLights[0].linear", 0.09);
	my_shader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	my_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	my_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	my_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[1].constant", 1.0f);
	my_shader.setFloat("pointLights[1].linear", 0.09);
	my_shader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	my_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	my_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[2].diffuse", 0.6f, 0.1f, 0.8f);
	my_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[2].constant", 1.0f);
	my_shader.setFloat("pointLights[2].linear", 0.09);
	my_shader.setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	my_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	my_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	my_shader.setVec3("pointLights[3].diffuse", 0.1f, 1.1f, 0.8f);
	my_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	my_shader.setFloat("pointLights[3].constant", 1.0f);
	my_shader.setFloat("pointLights[3].linear", 0.09);
	my_shader.setFloat("pointLights[3].quadratic", 0.032);

	my_shader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
	my_shader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
	my_shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	my_shader.setFloat("material.shininess", 2.0f);
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex), cube_vertex, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8*sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11*sizeof(float)));
	glEnableVertexAttribArray(4);
	GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

	

	
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
		//Update Camera Matrix
		
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		//Update Camera Matrix
		my_shader.use();
		glm::mat4 model = glm::mat4(1.0f);

		my_shader.setMat4("model",model);

		glm::mat4 projection = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f); 
		my_shader.setMat4("projection", projection);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		my_shader.setMat4("view", view);
		my_shader.setVec3("viewPos",cameraPos);
		glFlush();
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO);
		glUniform1i(glGetUniformLocation(my_shader.ID, "ourTexture"), 0); 
	    glUniform1i(glGetUniformLocation(my_shader.ID, "normalMap"), 1);
		my_shader.setBool("nm",false);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		for (int i = 0; i < shapes.size(); i++)
		{
			my_shader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
			my_shader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
			my_shader.setVec3("material.specular", 1.0f, 1.5f, 1.5f);
			my_shader.setFloat("material.shininess", 2.0f);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D,texture3);
			glUniform1i(glGetUniformLocation(my_shader.ID, "ourTexture"), 3);
			glBindVertexArray(obj_VAO_l[i]);
			my_shader.setBool("nm",false);
			glDrawArrays(GL_TRIANGLES,0,shapes[i].mesh.indices.size());

		}
		lampShader.use();
        model = glm::mat4(1.0f);

		lampShader.setMat4("model",model);

		projection = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f); 
		lampShader.setMat4("projection", projection);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		lampShader.setMat4("view", view);
        
        // We now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightVAO);
        for (GLuint i = 0; i < 4; i++)
        {
            model = glm::mat4();
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(1.0f)); 
            lampShader.setMat4("model",model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Render an object using texture and normal map.                                                             //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		







		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Render the object in .obj file. You need to set materials and wrap texture for objects.                    //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		








		
		/////////////////////////////////////////////////////////////////////
		
		/////////////////////////////end/////////////////////////////////////



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

