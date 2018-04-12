#include "Init.h"
#include"Camera.h"
#include "shader.h"
#include"Input.h"
#include "verticesData.h"
#include"Color.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define window_width 1000
#define window_height 1000

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadTexture(char const* path);

void spotLightSwitch(GLFWwindow* window);
bool light_switch = false;

int main() {
	initialize();
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenGL_Light", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	stbi_set_flip_vertically_on_load(true);

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	int objTexture_diffuse = loadTexture("container2.png");
	int objTexture_specular = loadTexture("container2_specular.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, objTexture_diffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, objTexture_specular);

	unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

	Camera camera(glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 projection(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)window_width / window_height, 0.1f, 100.0f);

	Shader objectShader("vertexShader.shader", "fragmentShader.shader");

	Shader lightShader("lampShader_v.shader", "lampShader_f.shader");

	glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
	while (!glfwWindowShouldClose(window)) {
		processInput(window, camera);
		setBackColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		float currentFrame = glfwGetTime();
		float time = currentFrame;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	//	glm::vec3 lightPosition(0.0f, 20.0f*cos(0.5f*time), 20.0f*sin(0.5f*time));

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
		camera.setFront(front);
    
		glm::mat4 view(1.0f);
		view = glm::lookAt(camera.getPos(), camera.getPos() + camera.getFront(), camera.getUp());

		objectShader.use();
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);
		objectShader.setVec3("objectColor", objectColor);
		objectShader.setVec3("lampColor", glm::vec3(1.0f, 1.0f, 1.0f));
		objectShader.setVec3("viewPos", camera.getPos());

		objectShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		objectShader.setFloat("material.shininess", 32.0f);
		
		objectShader.setInt("material.diffuse", 0);
		objectShader.setInt("material.specular", 1);

		objectShader.setVec3("directionalLight.direction", glm::vec3(1.0f, -1.0f, -1.0f));
		objectShader.setVec3("directionalLight.ambient", glm::vec3(0.1f));
		objectShader.setVec3("directionalLight.diffuse", glm::vec3(0.4f));
		objectShader.setVec3("directionalLight.specular", glm::vec3(0.5f));
		objectShader.setVec3("pointLight.position", glm::vec3(0.0f, 0.0f, 5.0f*sin(time)));
		objectShader.setVec3("pointLight.diffuse", glm::vec3(1.0f));
		objectShader.setVec3("pointLight.specular", glm::vec3(1.0f));
		objectShader.setFloat("pointLight.constant", 1.0f);
		objectShader.setFloat("pointLight.linear", 0.4f);
		objectShader.setFloat("pointLihgt.quadratic", 0.2f);
		objectShader.setVec3("spotLight.position", camera.getPos());
		objectShader.setVec3("spotLight.direction", camera.getFront());
		objectShader.setVec3("spotLight.diffuse", glm::vec3(2.0f));
		objectShader.setVec3("spotLight.specular", glm::vec3(2.0f));
		objectShader.setFloat("spotLight.constant", 1.0f);
		objectShader.setFloat("spotLight.linear", 0.3f);
		objectShader.setFloat("spotLight.quadratic", 0.1f);
		objectShader.setFloat("spotLight.cutOff", glm::radians(12.5f));
		objectShader.setFloat("spotLight.outerCutOff", glm::radians(17.5f));

		spotLightSwitch(window);
		objectShader.setBool("lightSwitch", light_switch);

		for (int i = 0; i < 10; i++) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = i * 20.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			objectShader.setMat4("model", model);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
    
	
		lightShader.use();
		glm::mat4 lightModel(1.0f);
		//draws the directional light source
		glm::mat4 lightModel_d(1.0f);
		lightModel = glm::translate(lightModel, camera.getPos());
		lightModel = glm::scale(lightModel_d, glm::vec3(5.0f, 5.0f, 5.0f));
		lightModel = glm::translate(lightModel, glm::vec3(-10.0f, 10.0f, 10.0f));
		lightShader.setMat4("model", lightModel);
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		lightShader.setVec3("lampColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//draws the point light source
		glm::mat4 lightModel_p(1.0f);
		lightModel = glm::translate(lightModel_p, glm::vec3(0.0f, 0.0f, 5.0f*sin(time)));
		lightModel = glm::scale(lightModel, glm::vec3(0.3f, 0.3f, 0.3f));
		lightShader.setMat4("model", lightModel);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

    
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}

bool firstMouse = true;
int w_width, w_height;
double lastX = 0.5*window_width;
double lastY = 0.5*window_height;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
}

unsigned int loadTexture(char const* path) {
	unsigned int objTexture;
	glGenTextures(1, &objTexture);
	glBindTexture(GL_TEXTURE_2D, objTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	int obj_width, obj_height, obj_nrChannels;
	unsigned char* data = stbi_load(path, &obj_width, &obj_height, &obj_nrChannels, 0);
	if (data) {
		GLenum format;
		if (obj_nrChannels == 1)
			format = GL_RED;
		else if (obj_nrChannels == 3)
			format = GL_RGB;
		else if (obj_nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, obj_width, obj_height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cout << "FAIL TO LOAD TEXTURE::" << path << std::endl;
	stbi_image_free(data);
	return objTexture;
}

bool spot_lock = false;
void spotLightSwitch(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (spot_lock == false) {
			spot_lock = true;
			if (light_switch == true) light_switch = false;
			else light_switch = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		spot_lock = false;
	}
}