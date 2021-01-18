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
//#include "texture/texture.cpp"

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
    int id;
public:
    
    glm::mat4 modelMatrix;
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    
    VBO() {
        id = ++vboID;
        modelMatrix = glm::mat4(1.0);
    }
    
    int getID() {
        return id;
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

        glGenVertexArrays(id, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        
        glGenBuffers(id, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(id, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

        glGenBuffers(id, &normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    }
    
    void handleVertexAttribArray() {
        int idStart = (id - 1) * 3;
        
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(idStart + 0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(idStart + 1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(idStart + 2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

        glDisableVertexAttribArray(idStart + 0);
        glDisableVertexAttribArray(idStart + 1);
        glDisableVertexAttribArray(idStart + 2);
    }
    
    void cleanUp() {
        glDeleteBuffers(id, &vertexbuffer);
        glDeleteBuffers(id, &uvbuffer);
        glDeleteBuffers(id, &normalbuffer);
        glDeleteVertexArrays(id, &VertexArrayID);
    }
    
};

int main(int argc, const char * argv[]) {
    
    if(!initializeWindow())
        return -1;

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/3DProjectDead/shader/StandardShading.vertexshader", "/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/3DProjectDead/shader/StandardShading.fragmentshader" );

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

    
    VBO suzanne;
    suzanne.loadObj("/Users/nikoburkert/Documents/XCode/workspace/First-3D-Project-Yet/3DProjectDead/common/suzanne.obj");
    suzanne.genBuffers();
    
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

        suzanne.rotate(0, glm::sin(glfwGetTime()*2.0f) /10, 0.1f);
        

        
        //light flies around
        glm::vec3 lightPos = glm::vec3(glm::sin(glfwGetTime()*2.0f) * 10,glm::cos(float(glfwGetTime()*3.3f)) * 10,glm::cos(glfwGetTime()*2.0f) * 10);
        //        glm::vec3 lightPos = getCameraPositionVector();
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        
        
        
        glm::mat4 ModelMatrix = suzanne.getModelMatrix();
        glm::scale(ModelMatrix, glm::vec3(1, 1, glm::sin(glfwGetTime())*1000.0f));
        
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        suzanne.handleVertexAttribArray();
        

        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while(glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    suzanne.cleanUp();
    
    glDeleteProgram(programID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}



