#include "SeasonManager.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_win32.h"
#include "imgui.h"
#include <GL/gl.h>
#include <algorithm>
#include <string>
#include <tchar.h>
#include <vector>
#include <windows.h>

// Data
static SeasonManager *seasonManager = nullptr;
static bool isRaceRunning = false;
static float raceSpeed = 1.0f;
static float weatherControl = 0.0f;

// Forward declarations of helper functions
bool CreateDeviceWGL(HWND hWnd, HDC *hDc);
void CleanupDeviceWGL(HWND hWnd, HDC *hDc);
void ResetDeviceWGL();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char **) {
  // Create application window
  // ImGui_ImplWin32_EnableDpiAwareness();
  WNDCLASSEXW wc = {sizeof(wc), CS_OWNDC,     WndProc,
                    0L,         0L,           GetModuleHandle(nullptr),
                    nullptr,    nullptr,      nullptr,
                    nullptr,    L"RaceLogic", nullptr};
  ::RegisterClassExW(&wc);
  HWND hwnd = ::CreateWindowW(
      wc.lpszClassName, L"RaceLogic v0.1 - F1 Simulator", WS_OVERLAPPEDWINDOW,
      100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

  // Initialize OpenGL
  HDC hDc;
  if (!CreateDeviceWGL(hwnd, &hDc)) {
    CleanupDeviceWGL(hwnd, &hDc);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWDEFAULT);
  ::UpdateWindow(hwnd);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplWin32_InitForOpenGL(hwnd);
  ImGui_ImplOpenGL3_Init("#version 130");

  // Initialize Season Manager
  seasonManager = new SeasonManager();
  seasonManager->loadDriversFromFile("data/drivers.txt");

  // Main loop
  bool done = false;
  while (!done) {
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT)
        done = true;
    }
    if (done)
      break;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. Simulation Logic
    if (isRaceRunning) {
      // Simple timer to control speed
      static float timeAccumulator = 0.0f;
      timeAccumulator += io.DeltaTime;
      if (timeAccumulator > (1.0f / raceSpeed)) {
        seasonManager->setWeather(weatherControl);
        seasonManager->simulateRaceStep();
        timeAccumulator = 0.0f;
      }
    }

    // 2. UI Layout
    {
      ImGui::Begin("Race Control");

      ImGui::Text("Simulation Control");
      if (ImGui::Button(isRaceRunning ? "Pause" : "Start")) {
        isRaceRunning = !isRaceRunning;
      }
      ImGui::SameLine();
      if (ImGui::Button("Step")) {
        seasonManager->simulateRaceStep();
      }

      ImGui::SliderFloat("Sim Speed", &raceSpeed, 1.0f, 20.0f);
      ImGui::SliderFloat("Weather (0=Dry, 1=Rain)", &weatherControl, 0.0f,
                         1.0f);

      ImGui::Separator();
      ImGui::Text("Application Average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }

    {
      ImGui::Begin("Live Leaderboard");

      MaxHeap *lb = seasonManager->getLeaderboard();
      const std::vector<Driver *> &drivers = lb->getUnderlyingContainer();

      std::vector<Driver *> sortedDrivers = drivers;
      std::sort(sortedDrivers.begin(), sortedDrivers.end(),
                [](Driver *a, Driver *b) {
                  return a->rankingScore > b->rankingScore;
                });

      if (ImGui::BeginTable("LeaderboardTable", 5,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Rank");
        ImGui::TableSetupColumn("Driver");
        ImGui::TableSetupColumn("Team");
        ImGui::TableSetupColumn("Score");
        ImGui::TableSetupColumn("Last Lap");
        ImGui::TableHeadersRow();

        int rank = 1;
        for (auto *d : sortedDrivers) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("%d", rank++);
          ImGui::TableNextColumn();
          ImGui::Text("%s", d->name.c_str());
          ImGui::TableNextColumn();
          ImGui::Text("%s", d->team.c_str());
          ImGui::TableNextColumn();
          ImGui::Text("%.2f", d->rankingScore);
          ImGui::TableNextColumn();
          ImGui::Text("%.3fs", d->lastLapTime);
        }
        ImGui::EndTable();
      }

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    RECT rect;
    ::GetClientRect(hwnd, &rect);
    int display_w = rect.right - rect.left;
    int display_h = rect.bottom - rect.top;
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ::SwapBuffers(hDc);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceWGL(hwnd, &hDc);
  ::DestroyWindow(hwnd);
  ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

  return 0;
}

// Helper functions
bool CreateDeviceWGL(HWND hWnd, HDC *hDc) {
  *hDc = ::GetDC(hWnd);
  PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
                               1,
                               PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                                   PFD_DOUBLEBUFFER,
                               PFD_TYPE_RGBA,
                               32,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               0,
                               24,
                               8,
                               0,
                               PFD_MAIN_PLANE,
                               0,
                               0,
                               0,
                               0};
  int format = ::ChoosePixelFormat(*hDc, &pfd);
  if (format == 0)
    return false;
  if (::SetPixelFormat(*hDc, format, &pfd) == FALSE)
    return false;
  HGLRC hRC = wglCreateContext(*hDc);
  if (hRC == NULL)
    return false;
  if (wglMakeCurrent(*hDc, hRC) == FALSE)
    return false;
  return true;
}

void CleanupDeviceWGL(HWND hWnd, HDC *hDc) {
  wglMakeCurrent(NULL, NULL);
  ::ReleaseDC(hWnd, *hDc);
}

void ResetDeviceWGL() {}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg) {
  case WM_SIZE:
    return 0;
  case WM_SYSCOMMAND:
    if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
      return 0;
    break;
  case WM_DESTROY:
    ::PostQuitMessage(0);
    return 0;
  }
  return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
