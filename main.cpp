#include <iostream>

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <vector>

#define ANT_AMOUNT 400000
#define ANT_DATA_SIZE 3


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

    /**
     * Hilfsfunktion, um eine OpenGL Textur zu erzeugen, die wir verwenden können, um darauf zu zeichnen, bzw. davon
     * zu lesen.
     *
     * @param size Die Höhe und Breite der Textur.
     * @return die OpenGL-interne ID der Textur
     */
    static GLuint createGlTexture(sf::Vector2u size);

};


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
    GLint length;

    // ANMERKUNG: Manche OpenGL-Funktionen haben zwar kein Rückgabewert, allerdings wird eine Variable die man hinein
    // gibt befüllt und soe wird ein Wert quasi zurück gegeben. (z.B. siehe hier InfoLogLength)

    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
    if ( length > 0 ){
        std::vector<char> log(length);
        glGetShaderInfoLog(shaderId, length, nullptr, &log[0]);
        std::cerr << "Error in file: " << shaderCodeFile << std::endl;
        std::cerr << &log[0] << std::endl;
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
        std::cerr << &log[0] << std::endl;
        throw;
    }
}

GLuint OpenGLHelper::createGlTexture(sf::Vector2u size) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, (GLsizei) size.x, (GLsizei) size.y, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}


// Hilfsfunktion, um eine Zufallszahl zu generieren. inValue sollte sich allerdings verändern (z.B. inValue++)
// Generiert für den gleichen inValue immer die gleiche Zahl
uint32_t rand(uint32_t inValue) {
    inValue ^= 2747636419u;
    inValue *= 2654435769u;
    inValue ^= inValue >> 16;
    inValue *= 2654435769u;
    inValue ^= inValue >> 16;
    inValue *= 2654435769u;
    return inValue;
}


int main() {
    // Create window
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Shader Tests", sf::Style::Fullscreen);


    // Init uniforms
    // Uniforms sind variablen für shader. Diese sind für jede shader-instance gleich. Da Die Bildschrimauflösung und
    // die Initialzahl für den Zufallswert für alle Ameisen gleich sein darf, können wir sie als uniforms übergeben.
    auto resolution = window.getSize();
    GLfloat resolutionF[2] = { (float) resolution.x, (float) resolution.y };
    GLuint randInValue = 0;


    // Init Glew
    GLenum err = glewInit();
    if (err != GLEW_OK)
        throw std::runtime_error("Error with glew!");
    // Wir wollen mindestens GLEW version 2.1
    if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
        throw std::runtime_error("Needs minimum glew 2.1!");



    // Initialize fullscreen texture render program
    // Dieses Program dient einfach nur dazu eine Textur über den ganzen Bildschirm zu zeichnen
    GLuint screenProgramID = OpenGLHelper::createProgram("../shader/fullscreenQuad_v.glsl", "../shader/texture_f.glsl");
    glUseProgram(screenProgramID);


    // Dieses Program dient dazu einen Fading-Effekt zu erzeugen. So blassen die Spuren langsam aus, anstatt direkt zu verschwinden.
    GLuint fadeTextureProgramID = OpenGLHelper::createProgram("../shader/fullscreenQuad_v.glsl", "../shader/fadeTexture_f.glsl");
    glUseProgram(fadeTextureProgramID);
    // Befülle das "resolution" uniform mit der Bildschirmauflösung
    glUniform2fv(glGetUniformLocation(fadeTextureProgramID, "resolution"), 1, resolutionF);
    // Definiere, dass das "tex" uniform die Texture Unit 0 verwendet (Das ist die Standarttextur die aktiviert ist)
    // Man könnte die Standarttextur ändern mit glActiveTexture(GL_TEXTURE1), glActiveTexture(GL_TEXTURE2) usw.
    glUniform1i(glGetUniformLocation(fadeTextureProgramID, "tex"), 0);


    // Initialize ant renderer
    // Dieses Program dient zum Zeichnen der Ameisen.
    GLuint renderAntsProgramID = OpenGLHelper::createProgram("../shader/antRender_v.glsl", "../shader/antRender_f.glsl", 2);
    glUseProgram(renderAntsProgramID);
    glUniform2fv(glGetUniformLocation(renderAntsProgramID, "resolution"), 1, resolutionF);


    // Initialize ant program
    // Dieses Program dient zum Bewegen der Ameisen. Hierbei wird nichts gezeichnet, sondern nur Daten verändert.
    // Wir erzeugen dieses Program nicht mit OpenGLHelper::createProgram, da wir keinen Fragmentshader haben und
    // wir das feedback konfigurieren müssen (shader Ausgaben)
    GLuint antProgramID = glCreateProgram();
    GLuint antShaderID = OpenGLHelper::compileShader("../shader/ant_v.glsl", GL_VERTEX_SHADER);
    glAttachShader(antProgramID, antShaderID);

    // specify transform feedback output
    // Hier registrieren wir die out-Variablen aus dem vertex-shader (feedback konfigurieren)
    const char *varyings[] = {"positionOut", "angleOut"};
    glTransformFeedbackVaryings(antProgramID, 2, varyings, GL_INTERLEAVED_ATTRIBS);

    OpenGLHelper::linkProgram(antProgramID);
    glUseProgram(antProgramID);

    // Apply window resolution to shader
    glUniform2fv(glGetUniformLocation(antProgramID, "resolution"), 1, resolutionF);
    glUniform1i(glGetUniformLocation(antProgramID, "tex"), 0);

    for (GLuint i = 0; i < 2; ++i) {
        glEnableVertexAttribArray(i);
    }

    // create ants in ram
    uint32_t iRand = 0;
    std::vector<GLfloat> antData(ANT_AMOUNT * ANT_DATA_SIZE);
    for (size_t i = 0; i < ANT_AMOUNT * ANT_DATA_SIZE; i += ANT_DATA_SIZE) {
        antData[i] =       (GLfloat) ( rand(iRand++) % resolution.x );  // random x
        antData[i + 1] =   (GLfloat) ( rand(iRand++) % resolution.y );  // random y
        antData[i + 2] = (((GLfloat) ( rand(iRand++) % 100000u)) / 100000.f) * 2.f * 3.14159f;  // random rotation
    }

    // generate ant buffers
    GLuint antBuffersIDs[2];
    // Hier legen wir 2 Daten-Buffer in OpenGL an
    // Später werden wir abwechselnd einen zum Schreiben und einen zum Lesen verwenden
    glGenBuffers(2, antBuffersIDs);

    // Schreibe die Daten aus unserem im ram gespeicherten buffer in die Grafikkarte
    for(int i = 0; i < 2; ++i) {
        // Aktiviere den Buffer: Alle folgenden Buffer-Befehle werden diesen Buffer betreffen.
        glBindBuffer(GL_ARRAY_BUFFER, antBuffersIDs[i]);
        // fill buffer with initial data
        glBufferData(GL_ARRAY_BUFFER, ANT_AMOUNT * ANT_DATA_SIZE * sizeof(GLfloat), &antData[0], GL_STATIC_DRAW);
    }

    // we ar blending so no depth testing
    // Deaktiviere Tiefen-Test (Wir haben keine Tiefe, da kein 3D)
    glDisable(GL_DEPTH_TEST);

    // Blending definiert wie Dinge übereinander gezeichnet werden sollen. Mehr infos dazu im Internet
    // z.B. https://learnopengl.com/Advanced-OpenGL/Blending
    // enable blending
    glEnable(GL_BLEND);
    //  and set the blend function to result = 1*source + 1*destination
    glBlendFunc(GL_ONE, GL_ONE);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    // Der Framebuffer beinhaltet das was am Ende auch auf den Bildschirm kommt. Wir zeichnen also auf ihn, um auf den
    // Bildschirm zu zeichnen
    GLuint framebufferID;
    glGenFramebuffers(1, &framebufferID);


    // The textures we're going to render to
    // Auch hier haben wir zwei Texturen, um von einer Lesen zu können und auf die andere schreiben zu können. Wir lesen
    // quasi immer von der Textur des letzten Laufes (Frame).
    GLuint textures[2];
    textures[0] = OpenGLHelper::createGlTexture(resolution);
    textures[1] = OpenGLHelper::createGlTexture(resolution);

    // Current Framebuffer Texture
    // Dieser Wert wird jedes Frame zwischen 0 und 1 wechseln.
    size_t cft = 0;


    // Main loop
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) { }
        // Bei Escape-Eingabe Program beenden
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
            break;
        }

        // hier könnten wir eine Regulierung einbauen, um z.B. 60 FPS zu erhalten..
        // sf::Time elapsed = clock.restart(); ...
        // Aber wir warten einfach jeden Frame 0.01 Sekunden
        sf::sleep(sf::seconds(0.01f));


        // Abwechseln treten die antBuffers in ihre beiden Rollen.
        // Erste Rolle: Wir lesen vom Buffer die Ameisendaten (position, rotation)
        // Wir brauchen die Ameisendaten und deshalb diesen Buffer für 2 Programme: antProgramID und renderAntsProgramID
        glBindBuffer(GL_ARRAY_BUFFER, antBuffersIDs[cft]);
        // Hier ordnen wir zu von wo sich die Shader-Instancen die Ameisendaten holen sollen
        glVertexAttribPointer(
                0, //index
                2, // Größe (x und y also 2)
                GL_FLOAT, // Datentyp
                GL_FALSE, // normalisiert. Normalerweise false
                ANT_DATA_SIZE * sizeof(GLfloat), // Gesamtgröße des Eintrages mit anderen Daten (x,y,rotation)
                (char*)(0 * sizeof(GLfloat))   // offset der Daten im Eintrag (0, da Position an erster Stelle)
        );
        // set up generic attrib pointers
        glVertexAttribPointer(
                1, //index
                1, // Größe (Rotation also 1)
                GL_FLOAT, // Datentyp
                GL_FALSE, // normalisiert. Normalerweise false
                ANT_DATA_SIZE * sizeof(GLfloat), // Gesamtgröße des Eintrages mit anderen Daten (x,y,rotation)
                (char*)(2 * sizeof(GLfloat))   // offset der Daten im Eintrag (2 * sizeof(GLfloat), da die Rotation hinter der Position gespeichert ist)
        );


        // Wir lassen das Ameisen-Program laufen. Also alle Ameisen bewegen sich.
        glUseProgram(antProgramID);

        // Aktualisiere uniform
        glUniform1ui(glGetUniformLocation(antProgramID, "randInValue"), randInValue);
        randInValue += ANT_AMOUNT * 4;

        // bind transform feedback target
        // Zweite Rolle des Ameisen-Buffers: In diesen Buffer werden die neuen Ameisendaten geschrieben.
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, antBuffersIDs[1 - cft]);

        // Fragmentshader deaktivieren (Wir haben keinen für antProgramID, da wir nichts zeichnen)
        glEnable(GL_RASTERIZER_DISCARD);

        // perform transform feedback
        // Brauchen wir, da wir Daten vom shader schreiben lassen
        glBeginTransformFeedback(GL_POINTS);
        // Führe den vertex-shader für alle Ameisen aus.
        glDrawArrays(GL_POINTS, 0, ANT_AMOUNT);
        // Brauchen wir, da wir Daten vom shader schreiben lassen
        glEndTransformFeedback();

        // Fragmentshader wieder aktivieren
        glDisable(GL_RASTERIZER_DISCARD);

        // Mit diesem Befehl könnten wir die Daten wieder aus der Grafikkarte heraus bekommen. Brauchen wir aber nicht,
        // da wir den Ein- und Ausgabebuffer switchen.
        //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ANT_AMOUNT * ANT_DATA_SIZE * sizeof(GLfloat), &antData[0]);



        // Start render to texture
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[cft], 0);
        // Render on the whole framebuffer, complete from the lower left corner to the upper right
        glViewport(0,0,(GLsizei) resolution.x, (GLsizei) resolution.y);

        // Buffer leeren
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Aktiviere Textur
        glBindTexture(GL_TEXTURE_2D, textures[1 - cft]);



        // Als erstes zeichnen wir eine faded Version des Bilderschirms des letzten Frames.
        // Dazu aktivieren wir unser fading Program
        glUseProgram(fadeTextureProgramID);
        // Und zeichnen ein Viereck (Es wird über den ganzen Bildschirm gezeichnet)
        glDrawArrays(GL_QUADS, 0, 4);



        // Dann zeichnen wir die Ameisen
        glUseProgram(renderAntsProgramID);

        // Wie müssen für die glDraw Funktion festlegen, dass Eintrag für Eintrag über die Attribute iteriert werden soll
        for(GLuint i = 0; i < 2; i++) glVertexAttribDivisor(i, 1);
        // draw all ants as quads (squares)
        glDrawArraysInstanced(GL_QUADS, 0, 4, ANT_AMOUNT);
        // Anschliesend stellen wir es wieder um.
        for(GLuint i = 0; i < 2; i++) glVertexAttribDivisor(i, 0);



        // Render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Render on the whole framebuffer, complete from the lower left corner to the upper right
        glViewport(0,0,(GLsizei) resolution.x, (GLsizei) resolution.y);

        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(screenProgramID);

        // Draw one quad with the whole picture
        glDrawArrays(GL_QUADS, 0, 4);

        // Swap buffers
        window.display();


        cft = 1 - cft;
    }

    return 0;
}
