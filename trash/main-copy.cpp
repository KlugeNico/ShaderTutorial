#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cmath>

#include "code/util/OpenGLHelper.h"

#define ANT_AMOUNT 10000000
#define ANT_DATA_SIZE 3

uint32_t rand(uint32_t seed)
{
    seed ^= 2747636419u;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    seed ^= seed >> 16;
    seed *= 2654435769u;
    return seed;
}

int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Shader Tests", sf::Style::Resize);
    auto resolution = window.getSize();
    GLfloat resolutionF[2] = { (float) resolution.x, (float) resolution.y };

    // Init Glew
    GLenum err = glewInit();
    if (err != GLEW_OK)
        throw std::system_error();
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        throw std::system_error();



    // Initialize fullscreen texture render program
    GLuint screenProgramID = OpenGLHelper::createProgram("../shader/fullscreenQuad_v.glsl", "../shader/texture_f.glsl", 0);
    glUseProgram(screenProgramID);
    GLint texID = glGetUniformLocation(screenProgramID, "renderedTexture");



    // Initialize ant renderer
    GLuint renderAntsProgramID = OpenGLHelper::createProgram("../shader/antRender_v.glsl", "../shader/solid_f.glsl", 2);
    glUseProgram(renderAntsProgramID);
    glUniform2fv(glGetUniformLocation(renderAntsProgramID, "resolution"), 1, resolutionF);



    // Initialize ant program
    GLuint antProgramID = glCreateProgram();
    GLuint antShaderID = OpenGLHelper::compileShader("../shader/ant_v.glsl", GL_VERTEX_SHADER);
    glAttachShader(antProgramID, antShaderID);

    // specify transform feedback output
    const char *varyings[] = {"positionOut", "angleOut"};
    glTransformFeedbackVaryings(antProgramID, 2, varyings, GL_INTERLEAVED_ATTRIBS);

    OpenGLHelper::linkProgram(antProgramID);
    glUseProgram(antProgramID);

    GLint backbufferUniformID = glGetUniformLocation(antProgramID, "backbuffer");

    // Apply window resolution to shader
    glUniform2fv(glGetUniformLocation(antProgramID, "resolution"), 1, resolutionF);

    for (GLuint i = 0; i < 2; ++i) {
        glEnableVertexAttribArray(i);
    }

    // create ants
    uint32_t iRand = 0;
    size_t cab = 0; // current ant buffer
    std::vector<GLfloat> antData(ANT_AMOUNT * ANT_DATA_SIZE);
    for (size_t i = 0; i < ANT_AMOUNT * ANT_DATA_SIZE; i += ANT_DATA_SIZE) {
        antData[i] =        (GLfloat) ( rand(iRand++) % 1000 );
        antData[i + 1] =    (GLfloat) ( rand(iRand++) % 1000 );
        antData[i + 2] = (((GLfloat) (rand(iRand++) % 100000u)) / 100000.f) * 2.f * (float) M_PI;
    }

    // generate ant buffers
    GLuint antBuffersIDs[2];
    GLuint antVertexArraysIDS[2];
    glGenBuffers(2, antBuffersIDs);
    glGenVertexArrays(2, antVertexArraysIDS);

    for(int i = 0; i < 2; ++i) {
        glBindVertexArray(antVertexArraysIDS[i]);

        glBindBuffer(GL_ARRAY_BUFFER, antBuffersIDs[i]);

        // fill with initial data
        glBufferData(GL_ARRAY_BUFFER, ANT_AMOUNT * ANT_DATA_SIZE * sizeof(GLfloat), &antData[0], GL_STATIC_DRAW);

        // set up generic attrib pointers
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, ANT_DATA_SIZE * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));
        // set up generic attrib pointers
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, ANT_DATA_SIZE * sizeof(GLfloat), (char*)0 + 2 * sizeof(GLfloat));
    }


    // "unbind" vao
    glBindVertexArray(0);

    // we ar blending so no depth testing
    glDisable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    //  and set the blend function to result = 1*source + 1*destination
    glBlendFunc(GL_ONE, GL_ONE);


    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, (GLsizei) resolution.x, (GLsizei) resolution.y, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // Main loop
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                case sf::Event::KeyPressed:
                    window.close();
                    break;

                default:
                    break;
            }
        }

        sf::Time elapsed = clock.restart();
        sf::sleep(sf::seconds(0.05f));

//        glUseProgram(antProgramID);
//
//        // Bind our texture in Texture Unit 0
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, renderedTexture);
//        // Set our "renderedTexture" sampler to use Texture Unit 0
//        glUniform1i(backbufferUniformID, 0);
//
//        // bind the current vao
//        glBindVertexArray(antVertexArraysIDS[(cab+1)%2]);
//
//        // bind transform feedback target
//        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, antBuffersIDs[cab]);
//
//        glEnable(GL_RASTERIZER_DISCARD);
//
//        // perform transform feedback
//        glBeginTransformFeedback(GL_POINTS);
//        glDrawArrays(GL_POINTS, 0, ANT_AMOUNT);
//        glEndTransformFeedback();
//
//        glDisable(GL_RASTERIZER_DISCARD);
//
//
//
//        glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ANT_AMOUNT * ANT_DATA_SIZE * sizeof(GLfloat), &antData[0]);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(renderAntsProgramID);

//        // Render to our framebuffer
//        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//        // Render on the whole framebuffer, complete from the lower left corner to the upper right
//        glViewport(0,0,(GLsizei) resolution.x, (GLsizei) resolution.y);

        glBindBuffer(GL_ARRAY_BUFFER, antBuffersIDs[cab]);
        glBindVertexArray(antVertexArraysIDS[cab]);

        // set up generic attrib pointers
//        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, ANT_DATA_SIZE * sizeof(GLfloat), (char*)0 + 0 * sizeof(GLfloat));
        // set up generic attrib pointers
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, ANT_DATA_SIZE * sizeof(GLfloat), (char*)0 + 2 * sizeof(GLfloat));
        // Iterate over Sprite Data item by item
        for(GLuint i = 0; i < 2; i++) glVertexAttribDivisor(i, 1);
//
//        // Bind our texture in Texture Unit 0
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, renderedTexture);
//        // Set our "renderedTexture" sampler to use Texture Unit 0
//        glUniform1i(backbufferUniformID, 0);
//
//        // bind the current vao
//        glBindVertexArray(antVertexArraysIDS[cab]);

        // draw
        glDrawArraysInstanced(GL_QUADS, 0, 4, ANT_AMOUNT);



//        // Render to the screen
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        // Render on the whole framebuffer, complete from the lower left corner to the upper right
//        glViewport(0,0,(GLsizei) resolution.x, (GLsizei) resolution.y);
//
//        // Clear the screen
//        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        window.clear();
//
//        // Use our shader
//        glUseProgram(screenProgramID);
//
//        // Bind our texture in Texture Unit 0
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, renderedTexture);
//        // Set our "renderedTexture" sampler to use Texture Unit 0
//        glUniform1i(texID, 0);

//        glDrawArrays(GL_QUADS, 0, 4);

        // Swap buffers
        window.display();



        cab = 1 - cab;
    }

    return 0;
}
