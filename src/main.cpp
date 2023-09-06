

#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include "Atom.h"
#include "Skybox.h"
#include "glm/ext.hpp"
#include "../Dependencies/imgui.h"
#include "../Dependencies/imgui_impl_glfw.h"
#include "../Dependencies/imgui_impl_opengl3.h"

#if __cplusplus >= 201703L
#include <filesystem>
#endif





#include <glm/glm.hpp>
#include "Molecule.h"
#include "TBCamera.h"


// some global variables
bool trackball = false; // if user is dragging on the screen
glm::vec3 prev_mouse;
float zoom = 90;
unsigned int program;


// some functions to load Vertex and fragment. Followed "The Cherno"'s opengl tutorial.
struct ShaderProgramSource{
    std::string VertexSource;
    std::string FragmentSource;
};


static ShaderProgramSource ParseShader(const std::string& path){
    enum class ShaderType{
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(path);
    std::stringstream ss[2];
    std::string line;
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if(line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else{
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if(result == GL_FALSE){
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char* message = (char*)alloca(len*sizeof(char));
        glGetShaderInfoLog(id, len, &len, message);

        std::cout << "Failed to compile shader" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader){
    program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}


// keyboard and mouse input functions
void mousepress(GLFWwindow* window, int button, int action, int mods){

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // x,y mouse position
        double x =0;
        double y =0;

        glfwGetCursorPos(window, &x, &y);

        // vec3 to store latest mouse click position
        prev_mouse = glm::vec3(x, y, -1);
        trackball = true; // if user is clicking
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        trackball = false;
}

void scroll(GLFWwindow* window, double xoffset, double yoffset){
    zoom -= yoffset;

    // clamp a max zoom. Zoom is tied to camera's FOV
    if(zoom > 90.0f)
        zoom = 90.0f;

    if(zoom < 20.0f)
        zoom = 20.0f;
}

void keypress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_PRESS)
    {
        return;
    }



    switch(key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
    }
}

int main(void)
{

    // OpenGL init
    GLFWwindow *window;
    if (!glfwInit()) {
        std::cout << "glfw init err" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    unsigned int width = 1280;
    unsigned int height = 720;
    window = glfwCreateWindow(width, height, "Molecule Viewer", NULL, NULL);
    if (!window) {
        std::cout << " can't create window!!!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cout << "Error ! " << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;

    glfwSetKeyCallback(window, keypress);
    glfwSetMouseButtonCallback(window, mousepress);
    glfwSetScrollCallback(window, scroll);


    // SETUP IMGUI

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Load phong shader for molecule and skybox shader
    ShaderProgramSource source = ParseShader("../res/shaders/phong.glsl");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

    ShaderProgramSource source_skybox = ParseShader("../res/shaders/skybox.glsl");
    unsigned int shader_skybox = CreateShader(source_skybox.VertexSource, source_skybox.FragmentSource);

    glm::vec3 lightPos{5.0f, 1.0f, 0.0f}; // Light position



    // Create camera with FOV, centered at 0,0,0
    TBCamera cam(90.0, (float)width/height, glm::vec3(0.0, 0.0, 0.0));

    glm::mat4 viewProjection;

    // Some OpenGL stuff
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    Skybox skybox; // create skybox

    // Some more OpenGL stuff
    glUseProgram(shader);
    glUseProgram(shader_skybox);
    glUniform1i(glGetUniformLocation(shader_skybox, "skybox"), 0);



    // x-rotation and y-rotation of the camera. Should probably be tied to the camera. Oh well
    float x_rot = 0.0f;
    float y_rot = 0.0f;



    // Get all molecules that can be loaded in molecules directory
    std::string path = "../res/molecules";
    std::vector<std::string> molecule_dir;
    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        if(std::string(entry.path()) == "../res/molecules/.DS_Store")
            continue;
        molecule_dir.push_back(std::string(entry.path()));
    }

    for (const auto & m:molecule_dir){
        std::cout << m.substr(17) << std::endl;
    }

    int current_mol_idx = 0; // index of the loaded molecule
    // Load initial molecule
    Molecule mol(molecule_dir[current_mol_idx]);

    // Duplicates of the view, world, and projection matrices for the skybox.
    // For some reason the skybox was moving the opposite direction it should have been
    glm::mat4 skybox_view = cam.view;
    glm::mat4 skybox_world = cam.world;
    glm::mat4 skybox_proj = glm::perspective(70.0f, (float)width/height, 0.1f, 100.0f);

    // MAIN LOOP
    while (!glfwWindowShouldClose(window)) {
        glUseProgram(shader);

        // Rotate camera based on user input
        if (trackball)
        {
            double x =0;
            double y =0;

            glfwGetCursorPos(window, &x, &y);

            // Calculate the distance, the mouse was moved,
            // between the last and the current frame
            double dx = x - prev_mouse.x;
            double dy = prev_mouse.y - y;

            // Rotation speed
            float scale_x = abs(dx) / width;
            float scale_y = abs(dy) / height;
            float rotSpeed = 350.0f;

            // Horizontal rotation
            if (dx < 0)
            {
                // rotate camera and skybox matrices in different directions
                cam.world = glm::rotate(cam.world, (float)glm::radians(-rotSpeed * scale_x), glm::vec3(0.0f, 1.0f, 0.0f));
                skybox_world = glm::rotate(skybox_world, (float)glm::radians(rotSpeed * scale_x), glm::vec3(0.0f, 1.0f, 0.0f));
                x_rot -= rotSpeed * scale_x;
            }
            else if (dx > 0)
            {
                cam.world = glm::rotate(cam.world, (float)glm::radians(rotSpeed * scale_x), glm::vec3(0.0f, 1.0f, 0.0f));
                skybox_world = glm::rotate(skybox_world, (float)glm::radians(-rotSpeed * scale_x), glm::vec3(0.0f, 1.0f, 0.0f));
                x_rot += rotSpeed * scale_x;
            }

            float rot = rotSpeed * scale_y;

            if (dy < 0)
            {
                // Upper rotation limit (+90 deg)
                if (y_rot + rot > 90)
                    rot = 90 - y_rot;

                cam.view = glm::rotate(cam.view, (float)glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
                skybox_view = glm::rotate(skybox_view, (float)glm::radians(-rot), glm::vec3(1.0f, 0.0f, 0.0f));
                y_rot += rot;

            }
            else if (dy > 0)
            {
                if (y_rot - rot < -90)
                    rot = y_rot + 90;

                cam.view = glm::rotate(cam.view, (float)glm::radians(-rot), glm::vec3(1.0f, 0.0f, 0.0f));
                skybox_view = glm::rotate(skybox_view, (float)glm::radians(rot), glm::vec3(1.0f, 0.0f, 0.0f));
                y_rot -= rot;
            }

            prev_mouse.x = x;
            prev_mouse.y = y;

        }

        cam.fov = zoom; // set camera's fov to zoom value
        viewProjection = cam.getProjMatrix() * cam.view * cam.world;

        // change some uniform values in the shader
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform3fv(glGetUniformLocation(shader, "lightPos"), 1, &lightPos[0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "viewPos"), 1, GL_FALSE, &viewProjection[0][0]);




        // Draw atoms
        for(Atom *a: mol.atoms) {
            // set mvp for each atom in the molecule
            glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, GL_FALSE, &(viewProjection * a->transform)[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shader, "m"), 1, GL_FALSE, &a->transform[0][0]);
            glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, &a->color[0]);
            a->draw();
        }

        // Draw bonds
        for(Bond *b: mol.bonds){
            // set mvp for every bond
            glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, GL_FALSE, &(viewProjection * b->transform)[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shader, "m"), 1, GL_FALSE, &b->transform[0][0]);
            glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, &b->color[0]);
            b->draw();
        }

        // skybox transform. I don't know if it's needed tbh
        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
        t = glm::scale(t, glm::vec3(100.0, 100.0, 100.0));

        // Draw skybox
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glUseProgram(shader_skybox);
        glUniformMatrix4fv(glGetUniformLocation(shader_skybox, "view"), 1, GL_FALSE, &(glm::mat4(glm::mat3(cam.getProjMatrix() * skybox_view * skybox_world*t)))[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader_skybox, "projection"), 1, GL_FALSE, &(skybox_proj*t)[0][0]);
        skybox.draw();
        glDepthMask(GL_TRUE);



        // Draw GUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {

            ImGui::Begin("Molecule Viewer Interface");

            ImGui::Text("Current Molecule: %s", mol.name.c_str()); // Show current molecule
            ImGui::Text("Molecule selector:");
            if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
            {
                for (int n = 0; n < (molecule_dir).size(); n++)
                {
                    const bool is_selected = (current_mol_idx == n);
                    if (ImGui::Selectable(molecule_dir[n].substr(17).c_str(), is_selected))
                        current_mol_idx = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            if (ImGui::Button("Load")){
                std::cout << glm::to_string(cam.view) << std::endl;
                std::cout << glm::to_string(mol.centre) << std::endl;

                try {
                    mol = Molecule(molecule_dir[current_mol_idx]); // load new molecule
                }
                catch(...) {
                    std::cout<< "Could not load this molecule :/" << std::endl;
                }

                // reset camera position
                cam.reset(mol.centre);
                skybox_view = cam.view;
                skybox_world = cam.world;
                x_rot = 0.0f;
                y_rot = 0.0f;
                zoom = 90.0;
                }

            static char buf1[64] = ""; ImGui::InputText("Molecule path",     buf1, 64);
            if (ImGui::Button("Add")){
                std::cout << buf1 << std::endl;
                std::filesystem::path sourceFile = buf1;
                std::filesystem::path targetParent = "../res/molecules";
                auto target = targetParent / sourceFile.filename();

                try // If you want to avoid exception handling, then use the error code overload of the following functions.
                {
                    std::filesystem::create_directories(targetParent); // Recursively create target directory if not existing.
                    std::filesystem::copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
                    molecule_dir.push_back(std::string (target));
                }
                catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.
                {
                    std::cout << e.what();
                }

            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);


        glfwPollEvents();
    }
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
// Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}