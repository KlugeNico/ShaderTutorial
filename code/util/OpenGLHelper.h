//
// Created by kreischenderdepp on 17.03.20.
//

#ifndef TEAMTOINK_OPENGLHELPER_H
#define TEAMTOINK_OPENGLHELPER_H


#include <GL/glew.h>
#include <sstream>
#include <vector>

class OpenGLHelper {

public:
    /**
     * Kompiliere den Shader-code aus einer Datei in OpenGL zur Laufzeit.
     *
     * @param shaderCodeFile Die Datei mit dem zu kompilierende Shader-code.
     * @param type In der Regel GL_VERTEX_SHADER oder GL_FRAGMENT_SHADER.
     * @return die OpenGL-interne ID des Shaders (Auch "handle" genannt). Diese brauchen wir in Zukunft, um den Shader
     * z.B. zu konfigurieren oder zu laden
     */
    static GLuint compileShader(const std::string &shaderCodeFile, GLenum type);

    /**
     * Link das Program. Muss gemacht werden, nachdem die zugehörigen shader attached wurden (glAttachShader)
     *
     * @param ProgramID Das zu Program, das gelinked werden soll.
     */
    static void linkProgram(GLuint ProgramID);

    /**
     * Erstelle ein OpenGL-Program. Diese Funktion ruft compileShader und linkProgram auf.
     *
     * @param vertexShaderFile Die Datei mit dem zu kompilierende vertex-shader-code.
     * @param fragmentShaderFile Die Datei mit dem zu kompilierende fragment-shader-code.
     * @param attribCount Wie viele Attribute besitzt unser vertex-shader? Mit Attribute sind hier die "in" variablen
     * in den Shadern gemeint. Diese unterscheiden sich insofern von den "uniform" Variablen, dass sie für jeden Vertex
     * unterschiedlich sind. Der Ameisen-shader braucht zum Beispiel für jeden Vertex (da jeder Vertex eine Ameise
     * representiert) den x-y-Koordinaten-Vektor und die Rotation.
     * @return die OpenGL-interne ID des Shaders (Auch "handle" genannt). Diese brauchen wir in Zukunft, um den Shader
     * z.B. zu konfigurieren oder zu laden
     */
    static GLuint createProgram(const std::string &vertexShaderFile, const std::string &fragmentShaderFile, GLuint attribCount = 0);
};


#endif //TEAMTOINK_OPENGLHELPER_H
