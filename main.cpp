#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>

// Rotation angles for the cube
static float angleX = 0.0f;
static float angleY = 0.0f;
static float angleZ = 0.0f;


void myInit(void)
{
    // Lighting configuration
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };  // Adjust shininess for specular highlights
    GLfloat light_position[] = { 0.0, 3.0, 0.0, 1.0 };  // Position the light above the scene
    GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat no_ambient[] = { 0.0, 0.0, 0.0, 1.0 };  // Disable ambient light

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   // Diffuse light for the main color
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);  // Specular highlights
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, no_ambient);  // No global ambient light

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Enable color tracking with material properties
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH); // Smooth shading for better lighting effects
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D rendering
    glLineWidth(15.0f); // Set line width for frame edges
}

void drawHollowFrameCube(float size)
{
    // Use scaled solid cubes to simulate the frame of a hollow cube (the casing)
    float frameThickness = 0.05f; // Thickness of the frame

    // Draw edges of the frame using elongated solid cubes
    // X-axis edges
    for (float y = -0.5f; y <= 0.5f; y += 1.0f)
    {
        for (float z = -0.5f; z <= 0.5f; z += 1.0f)
        {
            glPushMatrix();
            glTranslatef(0.0f, y * size, z * size);
            glScalef(1.0f, frameThickness, frameThickness);
            glutSolidCube(size);
            glPopMatrix();
        }
    }

    // Y-axis edges
    for (float x = -0.5f; x <= 0.5f; x += 1.0f)
    {
        for (float z = -0.5f; z <= 0.5f; z += 1.0f)
        {
            glPushMatrix();
            glTranslatef(x * size, 0.0f, z * size);
            glScalef(frameThickness, 1.0f, frameThickness);
            glutSolidCube(size);
            glPopMatrix();
        }
    }

    // Z-axis edges
    for (float x = -0.5f; x <= 0.5f; x += 1.0f)
    {
        for (float y = -0.5f; y <= 0.5f; y += 1.0f)
        {
            glPushMatrix();
            glTranslatef(x * size, y * size, 0.0f);
            glScalef(frameThickness, frameThickness, 1.0f);
            glutSolidCube(size);
            glPopMatrix();
        }
    }
}

void drawPanel(float size, float pos) // pos is for y translation
{
    // Panel is a solid cube that connects the bottom of the lamp to the frame
    glPushMatrix();
    glTranslatef(0.0f, pos, 0.0f);  // Position it beneath the solid cube
    glScalef(1.0f, 0.1f, 1.0f);  // Make the panel thinner than the solid cube
    glutSolidCube(size);  // Draw the panel as a solid cube
    glPopMatrix();
}

void drawCornerBumpers(float size)
{
    // Size of the bumper cubes at each corner
    float bumperSize = 0.3f * size;  // Adjust size as needed

    // Iterate over each corner position
    for (float x = -0.5f; x <= 0.5f; x += 1.0f)
    {
        for (float y = -0.5f; y <= 0.5f; y += 1.0f)
        {
            for (float z = -0.5f; z <= 0.5f; z += 1.0f)
            {
                // Position and draw a cube at each corner
                glPushMatrix();
                glTranslatef(x * size, y * size, z * size);
                glScalef(0.3f, 0.3f, 0.3f); // Make the bumpers smaller
                glutSolidCube(bumperSize); // Draw bumper as a cube
                glPopMatrix();
            }
        }
    }
}



void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color and depth buffers

    glPushMatrix();

    // Apply rotations based on the current angles
    glRotatef(angleX, 1.0, 0.0, 0.0); // Rotate around the X-axis
    glRotatef(angleY, 0.0, 1.0, 0.0); // Rotate around the Y-axis
    glRotatef(angleZ, 0.0, 0.0, 1.0); // Rotate around the Z-axis

    // Draw the solid cube (acting as the light)
    glColor3f(0.6, 0.6, 0.6);
    glutSolidCube(1.0);

    // Draw the hollow frame as a casing around the solid cube
    glColor3f(0.8, 0.8, 0.8);
    drawHollowFrameCube(1.2f); // Slightly larger to act as a casing

    glColor3f(0.8, 0.8, 0.8);
    drawPanel(1.2f, -0.55f);  // Draw a panel connecting the bottom
    drawPanel(1.05f, 0.55f);  // Draw a panel connecting the top

    glColor3f(0.7, 0.7, 0.7); // Different color for the bumpers if needed
    drawCornerBumpers(1.2f); // Slightly larger to match the frame size

    glPopMatrix();
    glutSwapBuffers(); // swap buffers for double buffering
}


void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void myKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'q': // Press 'q' to exit the program
            exit(0);
            break;
        default:
            break;
    }
}

void mySpecialKeys(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP:    // Rotate cube around X-axis positively
            angleX += 5.0f;
            break;
        case GLUT_KEY_DOWN:  // Rotate cube around X-axis negatively
            angleX -= 5.0f;
            break;
        case GLUT_KEY_LEFT:  // Rotate cube around Y-axis negatively
            angleY -= 5.0f;
            break;
        case GLUT_KEY_RIGHT: // Rotate cube around Y-axis positively
            angleY += 5.0f;
            break;
        case GLUT_KEY_PAGE_UP:   // Rotate cube around Z-axis positively
            angleZ += 5.0f;
            break;
        case GLUT_KEY_PAGE_DOWN: // Rotate cube around Z-axis negatively
            angleZ -= 5.0f;
            break;
        default:
            break;
    }
    glutPostRedisplay(); // Refresh display after each change
}

void myTimer(int value)
{
    glutPostRedisplay(); // refresh display
    glutTimerFunc(16, myTimer, 0); // roughly 60 FPS
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(400, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Rotating Cube");
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutSpecialFunc(mySpecialKeys); // Use special keys (arrow keys)
    glutReshapeFunc(myReshape);
    glutTimerFunc(16, myTimer, 0);
    myInit();
    glutMainLoop();
}
