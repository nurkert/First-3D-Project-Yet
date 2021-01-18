// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <cmath> // getting absolute values

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::vec3 position = glm::vec3( 0, 1, 10 ); // Initial position of camera

bool playerControl = false;
double lastPlayerControlSwitch = -1;

glm::vec3 getCameraPositionVector(){
    return position;
}

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

bool isPlayerControl(){
    return playerControl;
}


// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.004f;



void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
    
    // Get window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Mouse Calculation only if wanted
    if(playerControl) {
        float mouseOffsetX = float( width / 2 - xpos );
        float mouseOffsetY = float( height / 2 - ypos );

        glfwSetCursorPos(window, width / 2 , height / 2);

        horizontalAngle += mouseSpeed * mouseOffsetX;
        verticalAngle   += mouseSpeed * mouseOffsetY;
    }

    // Move down
    if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS){
        position -= glm::vec3( 0, 1, 0) * deltaTime * speed;
    }
    // Move up
    if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS){
        position += glm::vec3( 0, 1, 0) * deltaTime * speed;
    }

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
    
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

    if(playerControl) {
        // Move forward
        if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
            position += direction * deltaTime * speed;
        }
        // Move backward
        if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
            position -= direction * deltaTime * speed;
        }
        // Move right
        if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
            position += right * deltaTime * speed;
        }
        // Move left
        if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
            position -= right * deltaTime * speed;
        }
    } else {
        
    }
        
    // Switch mouse control mode
    if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS){
        // Cooldown, as executed without too often
        if(float(currentTime - lastPlayerControlSwitch) > 0.25f) {
            if(playerControl) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            } else {
                
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetCursorPos(window, width/2, height/2);
                 // to avoid a jump at the beginning
                
                position = glm::vec3( 0, 2, 5 );
                horizontalAngle = 3.14f;
                verticalAngle = 0.0f;
            }
            playerControl = !playerControl;
            lastPlayerControlSwitch = glfwGetTime();
        }
    }
	float FoV = 45.0f;
	// Projection matrix : 45° Field of View, ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), ((float)width / (float)height), 0.1f, 100.0f);
    

    if(playerControl) {
        // Camera matrix
        // position: Camera is here
        // position+direction: and looks here : at the same position, plus "direction"
        // up: Head is up (set to 0,-1,0 to look upside-down)
        ViewMatrix = glm::lookAt(position, position+direction, up );
    } else {
        position = glm::vec3(glm::sin(glfwGetTime()) * 7.5, glm::sin(glfwGetTime()/1.68)*2+2, glm::cos(glfwGetTime()) * 7.5);
        ViewMatrix = glm::lookAt(position, glm::vec3(0,2,0), up );
    }
    
	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}
