#include "app.h"
#include "imgui.h"

void run_app(windows currentWindow) {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (currentWindow == windows::Demo) {
        ImGui::ShowDemoWindow();
    }
    else if (currentWindow == windows::Test) {
        /*iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            std::cout << "Bytes received: " << iResult << std::endl;
        else if (iResult == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;*/
            //static float f = 0.0f;
            //static int counter = 0;

            //ImGui::Begin("betterscope");                          // Create a window called "Hello, world!" and append into it.

            //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            //    counter++;
            //ImGui::SameLine();
            //ImGui::Text("counter = %d", counter);

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            //ImGui::End();
    }
}

void cleanup() {

}
