#include <iostream>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>

#include <GL/glew.h> // Always include GLEW before gl.h and glfw3.h, since it's a bit magic.

#include <GLFW/glfw3.h> // GLFW handle the window and the keyboard
GLFWwindow* window;

#include <glm/glm.hpp> //a library for 3D mathematics.
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp> // scaling, rotation, projection matrices...

#include "shader/shader.hpp" // include LoadShaders function.
#include "controls.hpp"  // include keyboard and mouse control
#include "objloader.hpp"

bool initializeWindow() {
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1280, 720, "First-3D-Project-Yet", NULL, NULL );
    
    // Set minimum Limits for Window size
    glfwSetWindowSizeLimits( window, 534, 300, GLFW_DONT_CARE, GLFW_DONT_CARE );
    
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    // Enable depth test (The Z-Buffer)
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // black background color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    return true;
}

int vboID = 0;

class VBO {
    
private:
    //int id;
public:
    glm::vec3 color;
    
    glm::mat4 modelMatrix;
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    
    VBO() {
        color = glm::vec3(0.5,0.5,0.5);
        modelMatrix = glm::mat4(1.0);
    }
    
    void setColor(float r, float g, float b) {
        color = glm::vec3(r,g,b);
    }
    
    glm::vec3 getAmbientColor() {
        return color;
    }
    
    glm::mat4 getModelMatrix() {
        return modelMatrix;
    }
    
    void translate(float x, float y, float z) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
    }
    
    void scale(float x, float y, float z) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(x, y, z));
    }
    
    void rotate(float x, float y, float z) {
        modelMatrix = glm::rotate(modelMatrix, 0.01F, glm::vec3(x, y, z));
    }
    
    void loadObj(const char *path) {
        loadOBJ(path, vertices, uvs, normals);
    }
    
    void genBuffers() {
        // Load it into a VBO

        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    }
    
    void handleVertexAttribArray() {
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
    
    void cleanUp() {
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &uvbuffer);
        glDeleteBuffers(1, &normalbuffer);
        glDeleteVertexArrays(1, &VertexArrayID);
    }
    
};


int main(int argc, const char * argv[]) {
    
    if(!initializeWindow())
        return -1;

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/First3DProject/shader/StandardShading.vertexshader", "/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/First3DProject/shader/StandardShading.fragmentshader" );

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    
    GLuint ColorID = glGetUniformLocation(programID, "AmbientColor");
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    std::vector<VBO*> vbos;
    
    VBO cube;
    cube.loadObj("/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/First3DProject/objects/cube.obj");
    cube.setColor(1, 1, 1);
    cube.translate(0, -0.2, 0);
    vbos.push_back(&cube);
    
    VBO cylinder;
    cylinder.loadObj("/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/First3DProject/objects/cylinder.obj");
    cylinder.setColor(0.396f, 0.262, 0.129);
    
    cylinder.scale(0.1, 1, 0.1);
    vbos.push_back(&cylinder);
    
    VBO suzanne;
    suzanne.translate(0, 2, 0);
    suzanne.setColor(0.396f, 0.262, 0.129); // set suzanne color to brown
    suzanne.loadObj("/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/First3DProject/objects/suzanne.obj");
    vbos.push_back(&suzanne);

    
    for(VBO* vbo : vbos)
        vbo->genBuffers();
    
    // Animation loop
    do{
        // Clear the depth and color:
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        // camera radiates the light
        glm::vec3 lightPos = getCameraPositionVector();
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        // draw all vbos
        for(VBO* vbo : vbos) {
            glm::vec3 ambientColor = vbo->getAmbientColor();
            glUniform3f(ColorID, ambientColor.x, ambientColor.y, ambientColor.z); //xyz = rgb
            
            glm::mat4 ModelMatrix = vbo->getModelMatrix();
            glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
   
            // Send our transformation to the currently bound shader,
            // in the "MVP" uniform
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

            vbo->handleVertexAttribArray();
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while(glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    for(VBO* vbo : vbos) {
        vbo->cleanUp();
    }
    
    glDeleteProgram(programID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}



