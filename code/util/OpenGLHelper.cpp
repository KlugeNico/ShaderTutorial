//
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/ on 17.03.20.
//

#include "OpenGLHelper.h"

#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>

GLuint OpenGLHelper::createProgram(const std::string& vertexShaderFile, const std::string& fragmentShaderFile, GLuint attribCount){

    // Erstelle ein Program in OpenGL
    GLuint programID = glCreateProgram();

    // Create and compile the shaders
    GLuint vertexShaderID = compileShader(vertexShaderFile, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = compileShader(fragmentShaderFile, GL_FRAGMENT_SHADER);

    // Füge die kompilierten Shader dem Program hinzu
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    // Link das Program mit den shadern
    linkProgram(programID);

    // Aktiviere die "in"-Attribute im Vertex-Shader
    for (GLuint i = 0; i < attribCount; ++i) {
        glEnableVertexAttribArray(i);
    }

    return programID;
}


GLuint OpenGLHelper::compileShader(const std::string &shaderCodeFile, GLenum type) {
    // Lade Datei und lese Inhalt in shaderCode
    std::ifstream shaderCode_file(shaderCodeFile);
    std::string shaderCode((std::istreambuf_iterator<char>(shaderCode_file)),std::istreambuf_iterator<char>());

    // Erstelle Pointer auf den unseren code-quelltext
    char const * VertexSourcePointer = shaderCode.c_str();

    // Erstelle einen Shader in OpenGL
    GLuint shaderId = glCreateShader(type);

    // Lege den Quellcode des Shaders fest auf unseren code-quelltext und kompiliere ihn
    glShaderSource(shaderId, 1, &VertexSourcePointer , nullptr);
    glCompileShader(shaderId);

    // Check Vertex Shader
    // Prüfe, ob Fehler aufgetreten sind und gebe sie ggf. aus.
    // Optional, aber WICHTIG, da wir hier auf Programmierfehler in unseren Shadern aufmerksam gemacht werden!
    int InfoLogLength;

    // ANMERKUNG: Manche OpenGL-Funktionen haben zwar kein Rückgabewert, allerdings wird eine Variable die man hinein
    // gibt befüllt und soe wird ein Wert quasi zurück gegeben. (z.B. siehe hier InfoLogLength)

    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        char VertexShaderErrorMessage[InfoLogLength+1];
        glGetShaderInfoLog(shaderId, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        std::cout << std::string(VertexShaderErrorMessage) << std::endl;
        throw;
    }

    return shaderId;
}


void OpenGLHelper::linkProgram(GLuint ProgramID) {
    glLinkProgram(ProgramID);

    // Prüfe, ob Fehler aufgetreten sind und gebe sie ggf. aus.
    // Optional, aber WICHTIG, da wir hier auf Programmierfehler in unseren Shadern aufmerksam gemacht werden!
    GLint status;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(ProgramID, length, &length, &log[0]);
        std::cerr << &log[0];
        throw;
    }
}

