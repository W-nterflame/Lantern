#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // Include the stb_image header

std::vector<std::string> lampTexturePaths = {
    "D:\\XMUM\\Computer_Graphics\\CGAssets\\1.jpg",
    "D:\\XMUM\\Computer_Graphics\\CGAssets\\2.jpg",
    "D:\\XMUM\\Computer_Graphics\\CGAssets\\3.jpg"

};

int currentTextureIndex = 0;

// Rotation angles for the cube
static float angleX = 0.0f;
static float angleY = 0.0f;
static float angleZ = 0.0f;

float zoomage = 4.5;
bool lampOn = true;  // Lamp on/off state

// Variables for rainbow animation
float hue = 0.0f;  // Current hue value (0-360)

// Function to convert HSV to RGB
void hsvToRgb(float h, float s, float v, float& r, float& g, float& b) {
    int i = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
    }
}

float lampColor[3] = {1.0f, 0.0f, 0.0f};  // Initial warm light (RGB)

GLuint lampTextureID;

void loadLampTexture(const char* filepath) {
    glGenTextures(1, &lampTextureID);
    glBindTexture(GL_TEXTURE_2D, lampTextureID);

    int width, height, channels;
    unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);
    if (!image) {
        printf("Failed to load texture: %s\n", filepath);
        exit(1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, image);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB,
                      GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void updateLampColor() {
    // Convert the current hue to RGB
    hsvToRgb(hue, 1.0f, 1.0f, lampColor[0], lampColor[1], lampColor[2]);

    // Increment the hue value
    hue += 0.2;  // Adjust speed (1.0f = slow, higher = faster)
    if (hue >= 360.0f) hue -= 360.0f;  // Wrap around when exceeding 360
}

void setLampMaterial() {
    if (lampOn) {
        GLfloat lampEmission[] = {lampColor[0], lampColor[1], lampColor[2],
                                  1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, lampEmission);
    } else {
        GLfloat lampEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};  // Light off
        glMaterialfv(GL_FRONT, GL_EMISSION, lampEmission);
    }
}

void drawHollowFrameCube(float size) {
    // Use scaled solid cubes to simulate the frame of a hollow cube (the
    // casing)
    float frameThickness = 0.05f;  // Thickness of the frame

    // Draw edges of the frame using elongated solid cubes
    // X-axis edges

    glColor3f(0.388, 0.102, 0.027);

    for (float y = -0.5f; y <= 0.5f; y += 1.0f) {
        for (float z = -0.5f; z <= 0.5f; z += 1.0f) {
            glPushMatrix();
            glTranslatef(0.0f, y * size, z * size);
            glScalef(1.0f, frameThickness, frameThickness);
            glutSolidCube(size);
            glPopMatrix();
        }
    }

    // Y-axis edges
    for (float x = -0.5f; x <= 0.5f; x += 1.0f) {
        for (float z = -0.5f; z <= 0.5f; z += 1.0f) {
            glPushMatrix();
            glTranslatef(x * size, 0.0f, z * size);
            glScalef(frameThickness, 1.0f, frameThickness);
            glutSolidCube(size);
            glPopMatrix();
        }
    }

    // Z-axis edges
    for (float x = -0.5f; x <= 0.5f; x += 1.0f) {
        for (float y = -0.5f; y <= 0.5f; y += 1.0f) {
            glPushMatrix();
            glTranslatef(x * size, y * size, 0.0f);
            glScalef(frameThickness, frameThickness, 1.0f);
            glutSolidCube(size);
            glPopMatrix();
        }
    }
}

void drawLamp() {
    glEnable(GL_TEXTURE_2D);                      // Enable texturing
    glBindTexture(GL_TEXTURE_2D, lampTextureID);  // Bind the lamp texture

    // Draw a textured cube
    glBegin(GL_QUADS);

    // Front face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Top face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Bottom face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    // Right face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    // Left face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glEnd();

    glDisable(GL_TEXTURE_2D);  // Disable texturing
}

void drawCornerBumpers(float size) {
    // Size of the bumper cubes at each corner
    float bumperSize = 0.3f * size;  // Adjust size as needed

    glColor3f(0.2, 0.051, 0.016);

    // Iterate over each corner position
    for (float x = -0.5f; x <= 0.5f; x += 1.0f) {
        for (float y = -0.5f; y <= 0.5f; y += 1.0f) {
            for (float z = -0.5f; z <= 0.5f; z += 1.0f) {
                // Position and draw a cube at each corner
                glPushMatrix();
                glTranslatef(x * size, y * size, z * size);
                glScalef(0.3f, 0.3f, 0.3f);  // Make the bumpers smaller
                glutSolidCube(bumperSize);   // Draw bumper as a cube
                glPopMatrix();
            }
        }
    }
}

void drawPanel(float size, float pos, float r, float g,
               float b)  // pos is for y translation
{
    // Panel is a solid cube that connects the bottom of the lamp to the frame
    glPushMatrix();
    glColor3f(r, g, b);
    glTranslatef(0.0f, pos, 0.0f);  // Position it beneath the solid cube
    glScalef(1.0f, 0.1f, 1.0f);  // Make the panel thinner than the solid cube
    glutSolidCube(size);         // Draw the panel as a solid cube
    glPopMatrix();
}

void drawHangPanel(float size, float pos)  // pos is for y translation
{
    // Panel is a solid cube that connects the bottom of the lamp to the frame
    glPushMatrix();
    glColor3f(0.2, 0.051, 0.016);
    glTranslatef(0.0f, pos, 0.0f);  // Position it beneath the solid cube
    glScalef(1.0f, 0.05f, 1.0f);  // Make the panel thinner than the solid cube
    glutSolidCube(size);          // Draw the panel as a solid cube
    glPopMatrix();
}

void drawWire(float x1, float y1, float z1, float x2, float y2, float z2) {
    glLineWidth(2.0);    // Set line thickness
    glColor3d(0, 0, 0);  // Set wire color (e.g., gray)

    glBegin(GL_LINES);       // Start drawing a line
    glVertex3f(x1, y1, z1);  // Starting point (roof top)
    glVertex3f(x2, y2, z2);  // Ending point (torus ring)
    glEnd();
}

void resetMaterial() {
    GLfloat no_emission[] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_EMISSION,
                 no_emission);  // Reset emissive material
}

void displayLantern(void) { // fragile part
    glPushMatrix();
    glRotated(180, 1, 0, 0);  // upside down because
    // Draw the light cube
    glColor3f(0.6, 0.6, 0.6);
    setLampMaterial();
    drawLamp();  // Draw the lamp
    resetMaterial();

    // Draw the hollow frame as a casing around the light cube
    glColor3f(0.8, 0.8, 0.8);

    drawHollowFrameCube(1.2f);  // Slightly larger to act as a casing

    drawPanel(1.2f, -0.55f, 1, 0.855,
              0.573);  // Draw a panel connecting the bottom
    drawPanel(1.05f, 0.55f, 1, 0.855,
              0.573);  // Draw a panel connecting the top

    drawPanel(0.8f, -0.65f, 0.388, 0.102,
              0.027);  // Draw a panel connecting the bottom for connecting rope

    glPushMatrix();  // ring
    glTranslatef(0, -0.69, 0);
    glColor3f(0.2, 0.051, 0.016);
    glutSolidTorus(0.03, 0.05, 20, 20);
    glPopMatrix();

    glColor3f(0.7, 0.7, 0.7);  // Different color for the bumpers if needed
    drawCornerBumpers(1.2f);   // Slightly larger to match the frame size

    glPopMatrix();
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);  // Clear color and depth buffers

    glPushMatrix();

    glRotatef(angleX, 1.0, 0.0, 0.0);  // Rotate around the X-axis
    glRotatef(angleY, 0.0, 1.0, 0.0);  // Rotate around the Y-axis
    glRotatef(angleZ, 0.0, 0.0, 1.0);  // Rotate around the Z-axis

    glPushMatrix(); // Big first lantern
    glTranslatef(0.6, -0.2, 0.6);
    displayLantern();
    glRotated(-90, 1, 0, 0);
    drawWire(0.0, 0.0, 0.0, 0.0, 0.0, 1.8); // Wire
    glPopMatrix();

    glPushMatrix(); // Medium second lantern
    glTranslatef(-0.8, -0.8, -0.8);
    glScalef(0.6, 0.6, 0.6);
    displayLantern();
    glRotated(-90, 1, 0, 0);
    drawWire(0.0, 0.0, 0.0, 0.0, 0.0, 4.0); // Wire
    glPopMatrix();

    glPushMatrix(); // Small third lantern
    glTranslatef(-0.9, 0.5, 0.9);
    glScalef(0.4, 0.4, 0.4);
    displayLantern();
    glRotated(-90, 1, 0, 0);
    drawWire(0.0, 0.0, 0.0, 0.0, 0.0, 2.2); // Wire
    glPopMatrix();

    drawHangPanel(3.5, 1.5f);  // ROOF HANGING POINT

    glPopMatrix();
    glutSwapBuffers();
}

void myReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, zoomage, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void myKeyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'l':  // Toggle lamp on/off
            lampOn = !lampOn;
            if (lampOn)
                glEnable(GL_LIGHT0);
            else
                glDisable(GL_LIGHT0);
            break;
        case 'o':  // Zoom out
            zoomage += 0.2f;
            if (zoomage > 15.0f)
                zoomage = 15.0f;  // Optional: Prevent extreme zoom out
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0.0, 0.0, zoomage, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
        case 'i':  // Zoom in
            zoomage -= 0.2f;
            if (zoomage < 1.0f)
                zoomage = 1.0f;  // Optional: Prevent extreme zoom in
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0.0, 0.0, zoomage, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
        case 'k':
            currentTextureIndex = (currentTextureIndex + 1) % lampTexturePaths.size();
            loadLampTexture(lampTexturePaths[currentTextureIndex].c_str()); // Use .c_str()
            glutPostRedisplay();
            break;
        case 'q':  // Quit program
            exit(0);
            break;
        default:
            break;
    }
    glutPostRedisplay();  // Refresh display
}

void mySpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:  // Rotate cube around X-axis positively
            angleX += 5.0f;
            break;
        case GLUT_KEY_DOWN:  // Rotate cube around X-axis negatively
            angleX -= 5.0f;
            break;
        case GLUT_KEY_LEFT:  // Rotate cube around Y-axis negatively
            angleY -= 5.0f;
            break;
        case GLUT_KEY_RIGHT:  // Rotate cube around Y-axis positively
            angleY += 5.0f;
            break;
        case GLUT_KEY_PAGE_UP:  // Rotate cube around Z-axis positively
            angleZ += 5.0f;
            break;
        case GLUT_KEY_PAGE_DOWN:  // Rotate cube around Z-axis negatively
            angleZ -= 5.0f;
            break;
        default:
            break;
    }
    glutPostRedisplay();  // Refresh display after each change
}

int direction = 1;

void myTimer(int value) {
    if (angleY >= 30.0f) {
        direction =
            -1;  // Set direction to negative (rotate in the opposite direction)
    } else if (angleY <= -30.0f) {
        direction =
            1;  // Set direction to positive (rotate in the opposite direction)
    }

    // Update angleY based on the current direction
    angleY += 0.2f * direction;  // direction will be +1 or -1

    // Update lamp color and trigger a display refresh
    updateLampColor();

    glutPostRedisplay();            // refresh display
    glutTimerFunc(16, myTimer, 0);  // roughly 60 FPS
}

void myInit(void) {
    GLfloat mat_specular[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat mat_shininess[] = {20.0};
    GLfloat light_position[] = {3.0, 3.0, 3.0, 1.0};
    GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat ambient_light[] = {0.2, 0.2, 0.2, 1.0};

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);  // Ensure normals are scaled properly
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    loadLampTexture(lampTexturePaths[currentTextureIndex].c_str());
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(400, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("chinese lantern snake zodiac");

    myInit();



    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecialKeys);
    glutReshapeFunc(myReshape);
    glutTimerFunc(16, myTimer, 0);

    glutMainLoop();
    return 0;
}
